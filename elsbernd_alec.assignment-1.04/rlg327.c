#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#include "dungeon.h"
#include "path.h"

static int32_t monster_compare(const void *key, const void *with);
void temp_code(dungeon_t *d, int s);

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  int numMonsters = -1;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
	case 'n':
	  if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%d", &numMonsters) /* Argument is not an integer */) {
            usage(argv[0]);
	  }
	    break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  if (!do_load) {
    /* Set a valid position for the PC */
    d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  render_dungeon(&d);

  dijkstra(&d);
  dijkstra_tunnel(&d);
  render_distance_map(&d);
  render_tunnel_distance_map(&d);
  render_hardness_map(&d);
  render_movement_cost_map(&d);
  temp_code(&d,numMonsters);

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);
    

  

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}

//added code here
void temp_code(dungeon_t *d, int s)
{

  int num_monsters;

  if(s == -1){
  //checks to see if the switch is present.
  //if not then hardcode the amount of monsters
  num_monsters = 10;
  }
  else{
    num_monsters = s;
  }

  
  //array of monsters
  monster_type_t mons[num_monsters];

  //type of monster
  int intelligence;
  int telepathy;
  int tunneling;
  int erratic;


  //setting the pc settings
  d->pc.symbol = '@';
  d->pc.priority = 0; //always 0
  d->pc.speed = 10; //always 10
  d->pc.next_turn = 0;

  //priority for monsters
  int prio = 1; 

  //checking what type of monster they are and putting them into the array
  //also initializing it
  //binary numbers 0000 0001 and so on for monsters
  //erratic/tunneling/telepathy/intelligence
  //this is for the 16 types of possible monsters
  for (int i = 0; i < num_monsters; i++)
  {
    //setting the type
    intelligence = rand() % 2;
    telepathy = rand() % 2;
    tunneling = rand() % 2;
    erratic = rand() % 2;

    if (erratic == 0 && tunneling == 0 && telepathy == 0 && intelligence == 0)
    {
      mons[i].symbol = '0';
      mons[i].erra = 0;
      mons[i].tunnel = 0;
      mons[i].tele = 0;
      mons[i].intelli = 0;
    }
    else if (erratic == 0 && tunneling == 0 && telepathy == 0 && intelligence == 1)
    {
      mons[i].symbol = '1';
      mons[i].erra = 0;
      mons[i].tunnel = 0;
      mons[i].tele = 0;
      mons[i].intelli = 1;
    }
    else if (erratic == 0 && tunneling == 0 && telepathy == 1 && intelligence == 0)
    {
      mons[i].symbol = '2';
      mons[i].erra = 0;
      mons[i].tunnel = 0;
      mons[i].tele = 1;
      mons[i].intelli = 0;
    }
    else if (erratic == 0 && tunneling == 0 && telepathy == 1 && intelligence == 1)
    {
      mons[i].symbol = '3';
      mons[i].erra = 0;
      mons[i].tunnel = 0;
      mons[i].tele = 1;
      mons[i].intelli = 1;
    }
    else if (erratic == 0 && tunneling == 1 && telepathy == 0 && intelligence == 0)
    {
      mons[i].symbol = '4';
      mons[i].erra = 0;
      mons[i].tunnel = 1;
      mons[i].tele = 0;
      mons[i].intelli = 0;
    }
    else if (erratic == 0 && tunneling == 1 && telepathy == 0 && intelligence == 1)
    {
      mons[i].symbol = '5';
      mons[i].erra = 0;
      mons[i].tunnel = 1;
      mons[i].tele = 0;
      mons[i].intelli = 1;
    }
    else if (erratic == 0 && tunneling == 1 && telepathy == 1 && intelligence == 0)
    {
      mons[i].symbol = '6';
      mons[i].erra = 0;
      mons[i].tunnel = 1;
      mons[i].tele = 1;
      mons[i].intelli = 0;
    }
    else if (erratic == 0 && tunneling == 1 && telepathy == 1 && intelligence == 1)
    {
      mons[i].symbol = '7';
      mons[i].erra = 0;
      mons[i].tunnel = 1;
      mons[i].tele = 1;
      mons[i].intelli = 1;
    }
    else if (erratic == 1 && tunneling == 0 && telepathy == 0 && intelligence == 0)
    {
      mons[i].symbol = '8';
      mons[i].erra = 1;
      mons[i].tunnel = 0;
      mons[i].tele = 0;
      mons[i].intelli = 0;
    }
    else if (erratic == 1 && tunneling == 0 && telepathy == 0 && intelligence == 1)
    {
      mons[i].symbol = '9';
      mons[i].erra = 1;
      mons[i].tunnel = 0;
      mons[i].tele = 0;
      mons[i].intelli = 1;
    }
    else if (erratic == 1 && tunneling == 0 && telepathy == 1 && intelligence == 0)
    {
      mons[i].symbol = 'a';
      mons[i].erra = 1;
      mons[i].tunnel = 0;
      mons[i].tele = 1;
      mons[i].intelli = 0;
    }
    else if (erratic == 1 && tunneling == 0 && telepathy == 1 && intelligence == 1)
    {
      mons[i].symbol = 'b';
      mons[i].erra = 1;
      mons[i].tunnel = 0;
      mons[i].tele = 1;
      mons[i].intelli = 1;
    }
    else if (erratic == 1 && tunneling == 1 && telepathy == 0 && intelligence == 0)
    {
      mons[i].symbol = 'c';
      mons[i].erra = 1;
      mons[i].tunnel = 1;
      mons[i].tele = 0;
      mons[i].intelli = 0;
    }
    else if (erratic == 1 && tunneling == 1 && telepathy == 0 && intelligence == 1)
    {
      mons[i].symbol = 'd';
      mons[i].erra = 1;
      mons[i].tunnel = 1;
      mons[i].tele = 0;
      mons[i].intelli = 1;
    }
    else if (erratic == 1 && tunneling == 1 && telepathy == 1 && intelligence == 0)
    {
      mons[i].symbol = 'e';
      mons[i].erra = 1;
      mons[i].tunnel = 1;
      mons[i].tele = 1;
      mons[i].intelli = 0;
    }
    else if (erratic == 1 && tunneling == 1 && telepathy == 1 && intelligence == 1)
    {
      mons[i].symbol = 'f';
      mons[i].erra = 1;
      mons[i].tunnel = 1;
      mons[i].tele = 1;
      mons[i].intelli = 1;
    }
    mons[i].next_turn = 0;
    mons[i].priority = prio;
    mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20

    //position of monster
    int mon_pos_y;
    int mon_pos_x;
    int placed = 0;
    int random_room;

    while(!placed) //prevents monsters spawning on top of monsters
    {
      random_room = (rand() % (d->num_rooms)); //picks a random room
      //getting the position
      mon_pos_y = d->rooms[random_room].position[dim_y] + (rand() % (d->rooms[random_room].size[dim_y]));
      mon_pos_x = d->rooms[random_room].position[dim_x] + (rand() % (d->rooms[random_room].size[dim_x]));
      int is_good = 1;
      for (int j = 0; j < i+1; j++) //goes through every monster and checks their position
      {
	      if (mon_pos_y == mons[j].position[dim_y] && mon_pos_x == mons[j].position[dim_y])
        {
          is_good = -1; //sets to -1 to indicate that there is already a monster there
          break;
        }
      }
      if (is_good) //should be 1 if spot is empty
      {
        mons[i].position[dim_y] = mon_pos_y; //places monster position
        mons[i].position[dim_x] = mon_pos_x; //places monster position
        placed = 1;
      }
    }
    prio++;
  }

 

  heap_t test;
  heap_init(&test, monster_compare, NULL);
  heap_insert(&test, &d->pc);
  for (int i = 0; i < num_monsters; i++)
  {
    heap_insert(&test, &mons[i]);
  }
  
  //loops galore here
  int game_over = 0;
  while (game_over == 0)
  {
    monster_type_t *temp = (monster_type_t *) heap_remove_min(&test);
    temp->next_turn = temp->next_turn + 1000 / temp->speed;
    
    heap_insert(&test, &temp);
  }
  heap_delete(&test);

  pair_t p;
  int i;
  int flag;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      flag = 0;
      for(i = 0; i < num_monsters;i++){
	if (mons[i].position[dim_y] == p[dim_y] && mons[i].position[dim_x] == p[dim_x]){
	  putchar(mons[i].symbol);
	  flag = 1;
	}
      }
      if(flag){
      }
      else if (d->pc.position[dim_x] == p[dim_x] && d->pc.position[dim_y] == p[dim_y]) {
	putchar('@');
      }
      else {
	switch (mappair(p)) {
        case ter_wall:
	case ter_wall_immutable:
          putchar(' ');
          break;
        case ter_floor:
	case ter_floor_room:
          putchar('.');
	  break;
	case ter_floor_hall:
          putchar('#');
	  break;
	case ter_debug:
	  putchar('*');
	  fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
	  break;
	case ter_stairs_up:
          putchar('<');
	  break;
	case ter_stairs_down:
          putchar('>');
	  break;
	default:
	  break;
	  }
	}
      }
      putchar('\n');
   }
}


static int32_t monster_compare(const void *key, const void *with)
{
  if (((monster_type_t *) key)->next_turn == ((monster_type_t *) with)->next_turn)
  {
    return ((monster_type_t *) key)->priority - ((monster_type_t *) with)->priority;
  }
  return ((monster_type_t *) key)->next_turn - ((monster_type_t *) with)->next_turn;
}
