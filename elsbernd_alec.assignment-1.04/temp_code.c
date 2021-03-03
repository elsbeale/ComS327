#include "dungeon.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>



void temp_code(dungeon_t *d)
{
    srand(time(0));


    //NEED a check to see if the switch is present.
    //if not then hardcode the amount of monsters
    int num_monsters = 10;
    
    //array of monsters
    monster_type_t mons[num_monsters];

    //type of monster
    int intelligence = rand() % 2;
    int telepathy = rand() % 2;
    int tunneling = rand() % 2;
    int erratic = rand() % 2;


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
        if (erratic == 0 && tunneling == 0 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = '0';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 0 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = '1';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 0 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = '2';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 0 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = '3';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = '4';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = '5';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = '6';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = '7';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = '8';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = '9';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = 'a';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = 'b';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = 'c';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = 'd';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = 'e';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = 'f';
            mons[i].next_turn = 0;
            mons[i].priority = prio;
            mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20
            //need to get position of monster
        }
        prio++;
    }

}

//typedef struct pc {
//   pair_t position;
//   char symbol;
//   int next_turn;
//   int priority;
//   int speed;
// } pc_t;