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
        }
        else if (erratic == 0 && tunneling == 0 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = '1';
        }
        else if (erratic == 0 && tunneling == 0 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = '2';
        }
        else if (erratic == 0 && tunneling == 0 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = '3';
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = '4';
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = '5';
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = '6';
        }
        else if (erratic == 0 && tunneling == 1 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = '7';
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = '8';
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = '9';
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = 'a';
        }
        else if (erratic == 1 && tunneling == 0 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = 'b';
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 0 && intelligence == 0)
        {
            mons[i].symbol = 'c';
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 0 && intelligence == 1)
        {
            mons[i].symbol = 'd';
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 1 && intelligence == 0)
        {
            mons[i].symbol = 'e';
        }
        else if (erratic == 1 && tunneling == 1 && telepathy == 1 && intelligence == 1)
        {
            mons[i].symbol = 'f';
        }
        mons[i].next_turn = 0;
        mons[i].priority = prio;
        mons[i].speed = (rand() % (20 - 5 + 1)) + 5; //should get a random speed between 5 and 20

        //NEED TO GET THE POSITION OF THE MONSTER
        int mon_pos_y = d->rooms[i].position[dim_y] + (rand() % (d->rooms[i].size[dim_y] + 1));
        int mon_pos_x = d->rooms->position[dim_x] + (rand() % (d->rooms[i].size[dim_x] + 1));
        int placed = 0;
        while(!placed)
        {
            //NEED TO PREVENT MONSTERS BEING PLACED ON OTHER MONSTERS
            // for (int j = 0; j < i+1; j++)
            // {
            //     if ()
            // }

            mons[i].position[dim_y] = mon_pos_y; //these go into the if statement
            mons[i].position[dim_x] = mon_pos_x; //these go into the if statement
            placed = 1;
        }

        prio++;
    }

    

}