//=================== VideoCD ==========================

extern void vcd_read_toc(int fd);
extern int vcd_get_track_end(int fd,int track);
extern int  vcd_seek_to_track(int fd,int track);
extern void vcd_set_msf(unsigned int sect);
extern void vcd_close(int fd);
extern unsigned int vcd_get_msf();
extern int vcd_open(char* szDevice ,int iWhence);

