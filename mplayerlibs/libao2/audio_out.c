
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>

#include "../cygwin_inttypes.h"

#include "audio_out.h"
#include "afmt.h"

#include "../mp_msg.h"
#include "../help_mp.h"

// there are some globals:
ao_data_t ao_data={0,0,0,0,OUTBURST,-1,0};
char *ao_subdevice = NULL;


#ifdef HAVE_WIN32WAVEOUT
extern ao_functions_t audio_out_win32;
#endif

extern ao_functions_t audio_out_mpegpes;
extern ao_functions_t audio_out_pcm;
extern ao_functions_t audio_out_pss;
extern ao_functions_t audio_out_plugin;

ao_functions_t* audio_out_drivers[] =
    {
        // vo-related:   will fail unless you also do -vo mpegpes/dxr2
        &audio_out_win32,
        NULL
    };

void list_audio_out()
{
    int i=0;
    mp_msg(MSGT_AO, MSGL_INFO, MSGTR_AvailableAudioOutputDrivers);
    while (audio_out_drivers[i])
    {
        const ao_info_t *info = audio_out_drivers[i++]->info;
//        printf("\t%s\t%s\n", info->short_name, info->name);
    }
    //printf("\n");
}

ao_functions_t* init_best_audio_out(char** ao_list,int use_plugin,int rate,int channels,int format,int flags)
{
    int i;
    // first try the preferred drivers, with their optional subdevice param:
    if(ao_list && ao_list[0])
        while(ao_list[0][0])
        {
            char* ao=strdup(ao_list[0]);
            ao_subdevice=strchr(ao,':');
            if(ao_subdevice)
            {
                ao_subdevice[0]=0;
                ++ao_subdevice;
            }
            for(i=0;audio_out_drivers[i];i++)
            {
                ao_functions_t* audio_out=audio_out_drivers[i];
                if(!strcmp(audio_out->info->short_name,ao))
                {
                    // name matches, try it
                    if(use_plugin)
                    {
                        audio_out_plugin.control(AOCONTROL_SET_PLUGIN_DRIVER,audio_out);
                        audio_out=&audio_out_plugin;
                    }
                    if(audio_out->init(rate,channels,format,flags))
                        return audio_out; // success!
                }
            }
            // continue...


            ++ao_list;
            if(!(ao_list[0]))
                return NULL; // do NOT fallback to others
        }
    // now try the rest...


    ao_subdevice=NULL;
    for(i=0;audio_out_drivers[i];i++)
    {
        ao_functions_t* audio_out=audio_out_drivers[i];
				if(use_plugin)
				{
					audio_out_plugin.control(AOCONTROL_SET_PLUGIN_DRIVER,(int)audio_out);
					audio_out=&audio_out_plugin;
				}

        if(audio_out->init(rate,channels,format,flags))
            return audio_out; // success!
    }
    return NULL;
}

