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
      if (d->map[dim_y][dim_x] == ter_stairs_down)
      {
        //generate a new dungeon, pc position, and monsters
      }
      break;
      //upstairs
    case 44:
      if (d->map[dim_y][dim_x] == ter_stairs_up)
      {
        //generate a new dungeon, pc position, and monsters
      }
      break;
      //rest
    case 32:
    case 53:
      flag = 0;
      break;
      //m lists monsters
    case 109:
      for (int i = 0; i < DUNGEON_Y; i++)
      {
        for (int j = 0; j < DUNGEON_X; j++)
        { //STILL NEEDS WORK, not sure what to compare it to to check if there is a monster there
          if (d->character[i][j]->alive) //need to check if there is a monster at the spot
          {
            int tmp_x = d->character[i][j]->position[dim_x] - d->pc.position[dim_x];
            int tmp_y = d->character[i][j]->position[dim_y] - d->pc.position[dim_y];
            if (tmp_x > 0)
            {
              //the monster is to the right of the pc. monster is east by tmp_x distance.

            }
            else if (tmp_x < 0)
            {
              //the monster is to the left of the pc. monster is west by tmp_x distance.
              tmp_x = abs(tmp_x); //taking absolute value for printing purposes
            }
            else
            {
              //the monster is on the same column as the pc
              //monster is 0 distance east of the pc
            }
            if (tmp_y > 0)
            {
              //the monster is below the pc. monster is south by tmp_y distance
            }
            else if (tmp_y < 0)
            {
              //monster is above the pc. monster is north by tmp_y distance
              tmp_y = abs(tmp_y); //taking absolute value for printing purposes
            }
            else
            {
              //the monster is on the same row as the pc
              //monster is 0 distance north of the pc
            }

          }
        }
      }
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
