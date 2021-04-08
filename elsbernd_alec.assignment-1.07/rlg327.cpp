#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "utils.h"
#include "io.h"

class monsterdesc
{
  public:
    std::string name;
    std::string color;
    std::string description;
    int hp;
    int damage;
    int rarity;
    std::string ability;
    int sbase;
    int sdice;
    int ssides;
    int dbase;
    int ddice;
    int dsides;
    int hpbase;
    int hpdice;
    int hpsides;
    char symbol;
};

const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /   Rest in   \\           ||\n"
  "            |     |    Pieces     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <count>]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon d;
  time_t seed;
  struct timeval tv;
  int32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  
  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;

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
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
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

  srand(seed);

  //io_init_terminal();
  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  /* Ignoring PC position in saved dungeons.  Not a bug. */
  config_pc(&d);
  gen_monsters(&d);

  //io_display(&d);
  if (!do_load && !do_image) {
    io_queue_message("Seed is %u.", seed);
  }

  std:: ifstream testf("monster_desc.txt");
  std:: string tests;
  int monster_amount = 0;
  std::string word;
  bool tnameflag = false;
  bool tsymbolflag = false;
  bool tcolorflag = false;
  bool tdescflag = false;
  bool tspeedflag = false;
  bool tdamageflag = false;
  bool thpflag = false;
  bool trarityflag = false;
  bool tabilityflag = false;

  while (getline(testf,tests)) //goes through the file first, to see how many monsters there are
  {
    if (tests == "BEGIN MONSTER")
    {
      while (tests != "END")
      {
        getline(testf,tests);
        std::istringstream iss(tests);
        while (iss >> word)
        {
          if(word == "NAME")
          {
            tnameflag = true;
          }
          else if (word == "SYMB")
          {
            tsymbolflag = true;
          }
          else if (word == "COLOR")
          {
            tcolorflag = true;
          }
          else if (word == "DESC")
          {
            tdescflag = true;
          }
          else if (word == "SPEED")
          {
            tspeedflag = true;
          }
          else if (word == "DAM")
          {
            tdamageflag = true;
          }
          else if (word == "HP")
          {
            thpflag = true;
          }
          else if (word == "RRTY")
          {
            trarityflag = true;
          }
          else if (word == "ABIL")
          {
            tabilityflag = true;
          }
        }
      }
      if (tnameflag && tsymbolflag && tcolorflag && tdescflag && tspeedflag && tdamageflag && thpflag && trarityflag && tabilityflag)
      {
        monster_amount++;
        tnameflag = false;
        tsymbolflag = false;
        tcolorflag = false;
        tdescflag = false;
        tspeedflag = false;
        tdamageflag = false;
        thpflag = false;
        trarityflag = false;
        tabilityflag = false;
      }
      
    }
  }
  monsterdesc npc_arr[monster_amount]; //makes the npc array
  int array_pos = 0;
  std:: ifstream f("monster_desc.txt"); //actually going through the file now
  std:: string s;
  int flag1 = 1;
  int flag2;
  int monsters =0;
  std::string name;
  monsterdesc temp;
  
  std::string desc;
  //flags to see if the monster is valid
  bool nameflag = false;
  bool symbolflag = false;
  bool colorflag = false;
  bool descflag = false;
  bool speedflag = false;
  bool damageflag = false;
  bool hpflag = false;
  bool rarityflag = false;
  bool abilityflag = false;

  getline(f,s);

  std:: cout << "\n";
  std:: cout << s << std::endl;

  while(flag1){
    flag1 = 0;
    flag2 = 1;
    while((char) f.peek() != 'B'){
      f.get();
      if(f.eof()){
	flag2 =0;
	break;
      }
    }
    getline(f,s);
    if(!s.compare("BEGIN MONSTER")){
	    flag1 = 1;
    }

    while(s != "END") //goes through each line till it hits END
    {
      getline(f,s);
      std::istringstream iss(s);
      while (iss >> word) //goes through word by word in the line
      {
        if(word == "NAME") //gets and sets the name
        {
          while (iss >> word)
          {
            if(word != "NAME")
            {
              name.append(word);
              name.append(" ");
            }
          }
          temp.name = name;
          name.clear();
          nameflag = true;
        }
        else if (word == "SYMB") //gets and sets the symbol
        {
          while (iss >> word)
          {
            if (word != "SYMB")
            {
              assert(word.size() == 1);
              char symbol = word[0];
              temp.symbol = symbol;
            }
          }
          symbolflag = true;
        }
        else if (word == "COLOR") //gets and sets the color
        {
          iss >> word;
          temp.color = word;
          colorflag = true;
        }
        else if (word == "DESC") //gets and sets the description
        {
          getline(f,s);
          while (s != ".")
          {
            desc.append(s);
            getline(f,s);
            if (s != ".")
            {
              desc.append("\n");
            }
          }
          temp.description = desc;
          desc.clear();
          descflag = true;
        }
        else if (word == "SPEED") //gets and sets the speed
        {
          iss >> word;
          int char_count = 0;
          for (char &c: word)
          {
	          c = c;
            char_count++;
          }

          char temp_arr[char_count];
          
          int count = 0;
          for (char &c: word)
          {
            temp_arr[count] =  c;
            count++;
          }
          std::string stbase; //string base for appending to the end of a string
          std::string stdice; //string dice for appending to the end of a string
          std::string stsides; //string sides for appending to the end of a string
          bool plus_trigger = false; //trigger for +
          bool d_trigger = false; //trigger for d
          
          for (int i = 0; i < char_count; i++)
          {
            if (temp_arr[i] == '+') //finds the + and skips to the next loop
            {
              plus_trigger = true;
              continue;
            }
            if (temp_arr[i] == 'd') //finds the d and skips to the next loop
            {
              d_trigger = true;
              continue;
            }
            if(!plus_trigger)
            {
              stbase.append(std::to_string(temp_arr[i] - 48)); //base
            }
            else if(plus_trigger && !d_trigger)
            {
              stdice.append(std::to_string(temp_arr[i] - 48)); //dice
            }
            else if(plus_trigger && d_trigger)
            {
              stsides.append(std::to_string(temp_arr[i] - 48)); //sides
            }
          }
          temp.sbase = stoi(stbase); //turning the string base into int
          temp.sdice = stoi(stdice); //turning the string dice into int
          temp.ssides = stoi(stsides); //turning the string sides into int
          speedflag = true;
        }
        else if (word == "DAM") //same logic and similar code as speed
        {
          iss >> word;
          int char_count = 0;
          for (char &c: word)
          {
	          c = c;
            char_count++;
          }

          char temp_arr[char_count];
          
          int count = 0;
          for (char &c: word)
          {
            temp_arr[count] =  c;
            count++;
          }
          std::string stbase;
          std::string stdice;
          std::string stsides;
          bool plus_trigger = false;
          bool d_trigger = false;
          for (int i = 0; i < char_count; i++)
          {
            if (temp_arr[i] == '+')
            {
              plus_trigger = true;
              continue;
            }
            if (temp_arr[i] == 'd')
            {
              d_trigger = true;
              continue;
            }
            if(!plus_trigger)
            {
              stbase.append(std::to_string(temp_arr[i] - 48));
            }
            else if(plus_trigger && !d_trigger)
            {
              stdice.append(std::to_string(temp_arr[i] - 48));
            }
            else if(plus_trigger && d_trigger)
            {
              stsides.append(std::to_string(temp_arr[i] - 48));
            }
          }
          temp.dbase = stoi(stbase); //damage base
          temp.ddice = stoi(stdice); //damage dice
          temp.dsides = stoi(stsides); //damage sides
          damageflag = true;
        }
        else if (word == "HP") //same logic as speed and damage and similar code with them too
        {
          iss >> word;
          int char_count = 0;
          for (char &c: word)
          {
	          c = c;
            char_count++;
          }

          char temp_arr[char_count];
          
          int count = 0;
          for (char &c: word)
          {
            temp_arr[count] =  c;
            count++;
          }
          std::string stbase;
          std::string stdice;
          std::string stsides;
          bool plus_trigger = false;
          bool d_trigger = false;
          
          for (int i = 0; i < char_count; i++)
          {
            if (temp_arr[i] == '+')
            {
              plus_trigger = true;
              continue;
            }
            if (temp_arr[i] == 'd')
            {
              d_trigger = true;
              continue;
            }
            if(!plus_trigger)
            {
              stbase.append(std::to_string(temp_arr[i] - 48));
            }
            else if(plus_trigger && !d_trigger)
            {
              stdice.append(std::to_string(temp_arr[i] - 48));
            }
            else if(plus_trigger && d_trigger)
            {
              stsides.append(std::to_string(temp_arr[i] - 48));
            }
          }
          temp.hpbase = stoi(stbase); //hp base
          temp.hpdice = stoi(stdice); //hp dice
          temp.hpsides = stoi(stsides); //hp sides
          hpflag = true;
        }
        else if (word == "RRTY") //gets and sets rarity
        {
          iss >> word;
          std::stringstream ss;
          ss << word;
          std::string tmp;
          int tmpnum;

          ss >> tmp;
          std::stringstream(tmp) >> tmpnum;
          temp.rarity = tmpnum;
          rarityflag = true;
        }
        else if (word == "ABIL")
        {
          std::string abilwords;
          while (!iss.eof())
          {
            iss >> word;
            abilwords.append(word);
            abilwords.append(" "); //if multiple abilities, it adds a space
          }
          
          temp.ability = abilwords;
          abilityflag = true;
        }
      }

    }
    //happens if every flag gets triggered aka it's a valid npc
    if (nameflag && symbolflag && colorflag && descflag && speedflag && damageflag && hpflag && rarityflag && abilityflag)
    {
      monsters++;
      npc_arr[array_pos] = temp;
      array_pos++;
      //resets the flags for the next npc
      nameflag = false;
      symbolflag = false;
      colorflag = false;
      descflag = false;
      speedflag = false;
      damageflag = false;
      hpflag = false;
      rarityflag = false;
      abilityflag = false;
    }

    while(flag2){
      getline(f,s);
      if(!s.compare("END")){
	      flag2 = 0;
      }
    }
  }

  //prints each monster with all of its parameters
  for (int i = 0; i < monsters; i++)
  {
    std::cout << npc_arr[i].name << std::endl; //name
    std::cout << npc_arr[i].description << std::endl; //description
    std::cout << npc_arr[i].symbol << std::endl; //symbol
    std::cout << npc_arr[i].color << std::endl; //color
    std::cout << std::to_string(npc_arr[i].sbase) << "+" + std::to_string(npc_arr[i].sdice) + "d" + std::to_string(npc_arr[i].ssides) << std::endl; //speed
    std::cout << npc_arr[i].ability << std::endl; //abilities
    std::cout << std::to_string(npc_arr[i].hpbase) + "+" + std::to_string(npc_arr[i].hpdice) + "d" + std::to_string(npc_arr[i].hpsides) << std::endl; //hp
    std::cout << std::to_string(npc_arr[i].dbase) + "+" + std::to_string(npc_arr[i].ddice) + "d" + std::to_string(npc_arr[i].dsides) << std::endl; //damage
    std::cout << npc_arr[i].rarity << std::endl;//rarity
    std::cout <<  std::endl;
  }

  
  // while (pc_is_alive(&d) && dungeon_has_npcs(&d) && !d.quit) {
  // do_moves(&d);
  // }
  // io_display(&d);

  io_reset_terminal();

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = (char *) malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = (char *) malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  // printf("%s", pc_is_alive(&d) ? victory : tombstone);
  //printf("You defended your life in the face of %u deadly beasts.\n"
  // "You avenged the cruel and untimely murders of %u "
  //   "peaceful dungeon residents.\n",
  //    d.PC->kills[kill_direct], d.PC->kills[kill_avenged]);

  if (pc_is_alive(&d)) {
    /* If the PC is dead, it's in the move heap and will get automatically *
     * deleted when the heap destructs.  In that case, we can't call       *
     * delete_pc(), because it will lead to a double delete.               */
    character_delete(d.PC);
  }

  delete_dungeon(&d);

  return 0;
}
