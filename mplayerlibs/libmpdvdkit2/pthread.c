#include "pthread.h"

void pthread_mutex_lock(HANDLE mutexHandle)
{
	WaitForSingleObject(mutexHandle, INFINITE);
}

void pthread_mutex_unlock(HANDLE mutexHandle)
{
	ReleaseMutex(mutexHandle);
}

HANDLE pthread_mutex_init()
{
	return CreateMutex(NULL, FALSE, NULL);
}

void pthread_mutex_destroy(HANDLE mutexHandle)
{
	CloseHandle(mutexHandle);
}
