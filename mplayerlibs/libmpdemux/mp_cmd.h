#ifndef _MP_CMD_H_INCLUDED
#define _MP_CMD_H_INCLUDED

#define MP_CMD_DVDNAV 22
#define MP_CMD_DVDNAV_EVENT     6000

// The args types
#define MP_CMD_ARG_INT 0
#define MP_CMD_ARG_FLOAT 1
#define MP_CMD_ARG_STRING 2
#define MP_CMD_ARG_VOID 3

#ifndef MP_CMD_MAX_ARGS
#define MP_CMD_MAX_ARGS 10
#endif

typedef union mp_cmd_arg_value {
  int i;
  float f;
  char* s;
  void* v;
} mp_cmd_arg_value_t;

typedef struct mp_cmd_arg {
  int type;
  mp_cmd_arg_value_t v;
} mp_cmd_arg_t;

typedef struct mp_cmd {
  int id;
  char* name;
  int nargs;
  mp_cmd_arg_t args[MP_CMD_MAX_ARGS];
} mp_cmd_t;


// Should return 1 if the command was processed
typedef int (*mp_input_cmd_filter)(mp_cmd_t* cmd, int paused, void* ctx);

typedef struct mp_cmd_filter_st mp_cmd_filter_t;

struct mp_cmd_filter_st {
  mp_input_cmd_filter filter;
  void* ctx;
  mp_cmd_filter_t* next;
};

static mp_cmd_t* mp_input_get_queued_cmd(void);

// This function retrive the next avaible command waiting no more than time msec.
// If pause is true, the next input will always return a pause command.
mp_cmd_t* mp_input_get_cmd(int time, int paused);

int mp_input_queue_cmd(mp_cmd_t* cmd);
#endif