#ifndef _MPLAYER_SPUDEC_H
#define _MPLAYER_SPUDEC_H

void spudec_heartbeat(void *pthis, unsigned int pts100);
void spudec_assemble(void *pthis, unsigned char *packet, unsigned int len, unsigned int pts100);
void spudec_draw(void *pthis, void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride));
void spudec_draw_scaled(void *pthis, unsigned int dxs, unsigned int dys, void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride));
void spudec_update_palette(void *pthis, unsigned int *palette);
void *spudec_new_scaled(unsigned int *palette, unsigned int frame_width, unsigned int frame_height);
void *spudec_new_scaled_vobsub(unsigned int *palette, unsigned int *cuspal, unsigned int custom, unsigned int frame_width, unsigned int frame_height);
void *spudec_new(unsigned int *palette);
void spudec_free(void *pthis);
void spudec_reset(void *pthis);	// called after seek
int spudec_visible(void *pthis); // check if spu is visible
void spudec_set_font_factor(void * pthis, double factor); // sets the equivalent to ffactor
void spudec_set_hw_spu(void *pthis, vo_functions_t *hw_spu);
int spudec_changed(void *pthis);
void spudec_calc_bbox(void *me, unsigned int dxs, unsigned int dys, unsigned int* bbox);
void spudec_draw_scaled(void *me, unsigned int dxs, unsigned int dys, void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride));
#endif

