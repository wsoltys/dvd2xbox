#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <xtl.h>
#include <sys/timeb.h>

#ifndef DVD_FUNC_INCLUDED
#define DVD_FUNC_INCLUDED
static int mystrcasecmp(const char *c1, const char *c2)

{
	int l1;
	int l2;
	int i;

	if ((!c1) && (!c2)) return 0;

	if (!c1) return -1;

	if (!c2) return 1;

#ifndef strcasecmp

	l1=strlen(c1);
	l2=strlen(c2);

	i=0;

	while ((i<l1) && (i<l2))

	{

		char a1, a2;

		a1=toupper(c1[i]);

		a2=toupper(c2[i]);

		if (a1<a2) return -1;

		if (a1>a2) return 1;

		i++;

	}

	if (l1<l2) return -1;

	if (l1>l2) return 1;

	return 0;

#else

	return strcasecmp(c1, c2);

#endif

}
 
#ifndef TIMEZONE_DEFINED_
#define TIMEZONE_DEFINED_
struct timezone {
	 int tz_minuteswest;
     int tz_dsttime;
};
#endif 


int gettimeofday(struct timeval *tp, struct timezone *z)
{
   if (tp) {
           struct _timeb t;
           _ftime(&t);
           tp->tv_sec = t.time;
           tp->tv_usec = (long)t. millitm * 1000;
    }
    if (z) {
            TIME_ZONE_INFORMATION tz;
            GetTimeZoneInformation(&tz);
            z->tz_minuteswest = tz.Bias ;
            z->tz_dsttime = tz.StandardBias != tz.Bias;    
    }

    return 0;
}

#ifndef DIRENT_INCLUDED
#define DIRENT_INCLUDED
 
#ifdef __cplusplus
extern "C" {
#endif
 
typedef struct DIR DIR;
 
struct dirent
{
    char *d_name;
};

DIR           *opendir(const char *);
int           closedir(DIR *);
struct dirent *readdir(DIR *);
void          rewinddir(DIR *);
 
#ifdef __cplusplus
}
#endif
#endif

#endif