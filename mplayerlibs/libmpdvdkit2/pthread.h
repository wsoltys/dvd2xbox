/* 
	Header to wrap pthread calls
*/
#ifndef PTHREAD_INCLUDED
#define PTHREAD_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <xtl.h>
#include "config-dvdnav.h"

void pthread_mutex_lock(HANDLE mutexHandle);
void pthread_mutex_unlock(HANDLE mutexHandle);
HANDLE pthread_mutex_init();
void pthread_mutex_destroy(HANDLE mutexHandle);

#endif
