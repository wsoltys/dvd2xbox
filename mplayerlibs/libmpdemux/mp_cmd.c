/*
 *
 * mp_cmd.c
 *
 * Created on March 29/2003 to replicate mp_cmd for even callbacks from dvdnav_stream.
 * 
 * Notes: Probably not the cleanest place to put this, but definately the easiest and it 
 *        doesn't break anything else.  It can be broken out of here if something else ends
 *        up needing it.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mp_cmd.h"


#define CMD_QUEUE_SIZE 100


static mp_cmd_filter_t* cmd_filters = NULL;
static unsigned int cmd_queue_length = 0,cmd_queue_start = 0, cmd_queue_end = 0;
static mp_cmd_t* cmd_queue[CMD_QUEUE_SIZE];

static mp_cmd_t*
mp_input_get_queued_cmd(void) {
  mp_cmd_t* ret;

  if(cmd_queue_length == 0)
    return NULL;

  ret = cmd_queue[cmd_queue_start];
  
  cmd_queue_length--;
  cmd_queue_start = (cmd_queue_start + 1) % CMD_QUEUE_SIZE;
  
  return ret;
}  

mp_cmd_t* mp_input_get_cmd(int time, int paused) {
  mp_cmd_t* ret = NULL;
  mp_cmd_filter_t* cf;

  while(1) {
    ret = mp_input_get_queued_cmd();
/* We dont need to handle keys or cmds yet, just queued cmds to pop off and return to CMoviePlayer
    if(ret) break;
    ret = mp_input_read_keys(time,paused);
    if(ret) break;
    ret = mp_input_read_cmds(time); */
    break;
  }
  if(!ret) return NULL;

  for(cf = cmd_filters ; cf ; cf = cf->next) {
    if(cf->filter(ret,paused,cf->ctx))
      return NULL;
  }

  return ret;
}

int
mp_input_queue_cmd(mp_cmd_t* cmd) {
  if(cmd_queue_length  >= CMD_QUEUE_SIZE)
    return 0;
  cmd_queue[cmd_queue_end] = cmd;
  cmd_queue_end = (cmd_queue_end + 1) % CMD_QUEUE_SIZE;
  cmd_queue_length++;
  return 1;
}