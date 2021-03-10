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
  while(!input){
    input = getch();
    dir[dim_y] = 0;
    dir[dim_x] = 0;

    clear();
    render_dungeon(d);
    refresh();
    
    mvprintw(0,0,"%d",input);
    switch(input){
      //Q
    case 113:
      d->pc.alive = 0;
      break;
      //upper left
      //
    case 55:
    case 121:
      dir[dim_y] = -1;
      dir[dim_x] = -1;
      break;
      //up
    case 56:
    case 107:
      dir[dim_y] = -1;
      break;
      //up right
    case 57:
    case 117:
      dir[dim_y] = -1;
      dir[dim_x] = 1;
      break;
      //right 
    case 54:
    case 108:
      dir[dim_x] = 1;
      break;
      //down right
    case 51:
    case 110:
      dir[dim_y] = 1;
      dir[dim_x] = 1;
      break;
      //down
    case 50:
    case 106:
      dir[dim_y] = 1;
      break;
      //down left
    case 49:
    case 98:
      dir[dim_y] = 1;
      dir[dim_x] = -1;
      break;
      //left
    case 52:
    case 104:
      dir[dim_x] = -1;
      break;
      //downstairs or rest if not on stairs
    case 46:
      break;
      //upstairs
    case 44:
      break;
      //rest
    case 32:
    case 53:
      break;
      //m lists monsters
    case 109:
      break;
      // scroll up list if list is up
    case 259:
      break;
      //scroll down list is if list is up
    case 258:
      break;
      // need to add ecaape key still
    default:
      input = 0;
      break;
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
