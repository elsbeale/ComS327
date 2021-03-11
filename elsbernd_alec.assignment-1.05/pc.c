#include <stdlib.h>
#include <curses.h>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"

void pc_delete(pc_t *pc)
{
  if (pc) {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  memset(&d->pc, 0, sizeof (d->pc));
  d->pc.symbol = '@';

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = calloc(1, sizeof (*d->pc.pc));
  d->pc.npc = NULL;
  d->pc.kills[kill_direct] = d->pc.kills[kill_avenged] = 0;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir){

  int input = 0;
  int flag = 1;

  character_t monsters[d->max_monsters];
  int mon_count = 0;
  //should find all monsters in the 2d character array and add them to the monsters array
  for (int i = 0; i < DUNGEON_Y; i++)
  {
    for (int j = 0; j < DUNGEON_X; j++)
    {
      if (d->character[i][j]->alive)
      {
        monsters[mon_count] = d->character[i][j];
        mon_count++;
      }
    }
  }
  
  while(flag){
    input = getch();
    dir[dim_y] = 0;
    dir[dim_x] = 0;

    
    mvprintw(0,0,"%d",input);
    switch(input){
      //Q
    case 113:
      d->pc.alive = 0;
      flag =  0;
      break;
      //upper left
      //
    case 55:
    case 121:
       if (d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]-1] == ter_floor_room || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]-1] == ter_floor_hall || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]-1] == ter_stairs_up || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]-1] == ter_stairs_down)
      {
        dir[dim_y] = -1;
        dir[dim_x] = -1;
	flag = 0;
	break;
      }
       
      //up
    case 56:
    case 107:
      if (d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]] == ter_floor_room || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]] == ter_floor_hall || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]] == ter_stairs_up || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]] == ter_stairs_down)
      {
        dir[dim_y] = -1;
	flag = 0;
      }
      break;
      //up right
    case 57:
    case 117:
       if (d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]+1] == ter_floor_room || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]+1] == ter_floor_hall || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]+1] == ter_stairs_up || d->map[d->pc.position[dim_y]-1][d->pc.position[dim_x]+1] == ter_stairs_down)
      {
        dir[dim_y] = -1;
        dir[dim_x] = 1;
	flag = 0;
      }
      break;
      //right 
    case 54:
    case 108:
       if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]+1] == ter_floor_room || d->map[d->pc.position[dim_y]][d->pc.position[dim_x]+1] == ter_floor_hall || d->map[d->pc.position[dim_y]][d->pc.position[dim_x]+1] == ter_stairs_up || d->map[d->pc.position[dim_y]][d->pc.position[dim_x]+1] == ter_stairs_down)
      {
        dir[dim_x] = 1;
	flag = 0;
      }
      break;
      //down right
    case 51:
    case 110:
      if (d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]+1] == ter_floor_room || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]+1] == ter_floor_hall || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]+1] == ter_stairs_up || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]+1] == ter_stairs_down)
      {
        dir[dim_y] = 1;
        dir[dim_x] = 1;
	flag = 0;
      }
      break;
      //down
    case 50:
    case 106:
       if (d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]] == ter_floor_room || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]] == ter_floor_hall || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]] == ter_stairs_up || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]] == ter_stairs_down)
       {
	 dir[dim_y] = 1;
	 flag = 0;
       } 
      break;
      //down left
    case 49:
    case 98:
      if (d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]-1] == ter_floor_room || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]-1] == ter_floor_hall || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]-1] == ter_stairs_up || d->map[d->pc.position[dim_y]+1][d->pc.position[dim_x]-1] == ter_stairs_down)
      {
        dir[dim_y] = 1;
        dir[dim_x] = -1;
	flag = 0;
      }
      break;
      //left
    case 52:
    case 104:
      if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]-1] == ter_floor_room || d->map[d->pc.position[dim_y]][d->pc.position[dim_x]-1] == ter_floor_hall || d->map[d->pc.position[dim_y]][d->pc.position[dim_x]-1] == ter_stairs_up || d->map[d->pc.position[dim_y]][d->pc.position[dim_x]-1] == ter_stairs_down)
      {
        dir[dim_x] = -1;
	flag = 0;
      }
      break;
      //downstairs or rest if not on stairs
    case 46:
      dungeon_t new_d;
      if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_down)
      {
        //generate a new dungeon, pc position, and monsters
        d = new_d;
        init_dungeon(&d);
        gen_dungeon(&d);
        config_pc(&d);
        gen_monsters(&d);
        render_dungeon(&d);

        refresh();

        while (pc_is_alive(&d) && dungeon_has_npcs(&d)) {
          render_dungeon(&d);
          refresh();
          do_moves(&d);
        }

        render_dungeon(&d);
      }
      break;
      //upstairs
    case 44:
      dungeon_t new_d;
      
      if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_up)
      {
        //generate a new dungeon, pc position, and monsters
        d = new_d;
        init_dungeon(&d);
        gen_dungeon(&d);
        config_pc(&d);
        gen_monsters(&d);
        render_dungeon(&d);

        refresh();

        while (pc_is_alive(&d) && dungeon_has_npcs(&d)) {
          render_dungeon(&d);
          refresh();
          do_moves(&d);
        }

        render_dungeon(&d);
      }
      break;
      //rest
    case 32:
    case 53:
      flag = 0;
      break;
      //m lists monsters
    case 109: //NEEDS WORK. need to check if number of monsters > 21. if so stop printing.
      int y_position = 1; 
      int north, east, tmp_x, tmp_y;
      clear();
      if (d->max_monsters < 21)
      {
        for (int i = 0; i < d->max_monsters; i++)
        {
          tmp_x = monsters[i].position[dim_x] - d->pc.position[dim_x];
          tmp_y = monsters[i].position[dim_y] - d->pc.position[dim_y];
          if (tmp_y > 0)
          {
            //the monster is below the pc. monster is south by tmp_y distance
            north = 0;
          }
          else
          {
            //monster is above the pc. monster is north by tmp_y distance
            tmp_y = abs(tmp_y); //taking absolute value for printing purposes
            north = 1;
          }
          if (tmp_x > 0)
          {
            //the monster is to the right of the pc. monster is east by tmp_x distance.
            east = 1;
          }
          else
          {
            //the monster is to the left of the pc. monster is west by tmp_x distance.
            tmp_x = abs(tmp_x); //taking absolute value for printing purposes
            east = 0;
          }
          if (!north && !east)
          {
            mvprintw(y_position,0, "%d, %d south and %d west", monsters[i].symbol, tmp_y, tmp_x);
            y_position++;
          }
          else if (!north && east)
          {
            mvprintw(y_position,0, "%d, %d south and %d east", monsters[i].symbol, tmp_y, tmp_x);
            y_position++;
          }
          else if (north && !east)
          {
            mvprintw(y_position,0, "%d, %d north and %d west", monsters[i].symbol, tmp_y, tmp_x);
            y_position++;
          }
          else
          {
            mvprintw(y_position,0, "%d, %d north and %d east", monsters[i].symbol, tmp_y, tmp_x);
            y_position++;
          }
        }
      }
      else
      {
        int arr_count = 0;
        while (arr_count < 20)
        {
          tmp_x = monsters[arr_count].position[dim_x] - d->pc.position[dim_x];
          tmp_y = monsters[arr_count].position[dim_y] - d->pc.position[dim_y];
          if (tmp_y > 0)
          {
            //the monster is below the pc. monster is south by tmp_y distance
            north = 0;
          }
          else
          {
            //monster is above the pc. monster is north by tmp_y distance
            tmp_y = abs(tmp_y); //taking absolute value for printing purposes
            north = 1;
          }
          if (tmp_x > 0)
          {
            //the monster is to the right of the pc. monster is east by tmp_x distance.
            east = 1;
          }
          else
          {
            //the monster is to the left of the pc. monster is west by tmp_x distance.
            tmp_x = abs(tmp_x); //taking absolute value for printing purposes
            east = 0;
          }
          if (!north && !east)
          {
            mvprintw(y_position,0, "%d, %d south and %d west", monsters[arr_count].symbol, tmp_y, tmp_x);
            y_position++;
          }
          else if (!north && east)
          {
            mvprintw(y_position,0, "%d, %d south and %d east", monsters[arr_count].symbol, tmp_y, tmp_x);
            y_position++;
          }
          else if (north && !east)
          {
            mvprintw(y_position,0, "%d, %d north and %d west", monsters[arr_count].symbol, tmp_y, tmp_x);
            y_position++;
          }
          else
          {
            mvprintw(y_position,0, "%d, %d north and %d east", monsters[arr_count].symbol, tmp_y, tmp_x);
            y_position++;
          }
          arr_count++;
        }
      }
      refresh();
      break;
      // scroll up list if list is up
    case 259:
      break;
      //scroll down list is if list is up
    case 258:
      break;
      // need to add ecaape key still
    default:
      flag = 1;
    }
  }
  return 0;
}

uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->pc.position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc.position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->pc.position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc.position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}
