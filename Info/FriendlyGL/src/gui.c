
#include "gui.h"

int main() {
	
	FB_fb fb;
	FT_ft ft;
	TS_ts ts;
	
	printf("Ouverture du framebuffer …\n");
	if (FB_open(&fb, "/dev/fb0") == -1) return 1;
	printf("\tRésolution : %dx%d, %dbpp\n", fb.var_info.xres, fb.var_info.yres, fb.var_info.bits_per_pixel); 
	
	FB_rect(&fb, 0, 0, 480, 272, 0, 0, 31);	// Font bleu
	
	printf("Initialisation de la lib freetype …\n");
	FT_init(&ft);
	printf("\tFont : %s\n", getenv("GUI_FONT"));
	FT_setColor(&ft, 31, 63, 31);
	FT_setFont(&ft, getenv("GUI_FONT"));
	FT_setSize(&ft, 16);
	
	FT_print(&ft, "Salut bonhomme de bois !",&fb, 50, 100);
	
	pthread_t pth;
	FT_ft TS_ft;
	FT_init(&TS_ft);
	FT_setColor(&TS_ft, 31, 63, 31);
	FT_setFont(&TS_ft, getenv("GUI_FONT"));
	TS_init(&ts, &fb, &TS_ft, &pth);
	
	TS_addButton(&ts, 408, 0, 72, 34, "Button 1", 12, 5, 10, onEvent);
	TS_addButton(&ts, 408, 34, 72, 34, "Button 2", 12, 5, 10, onEvent);
	/*TS_addButton(&ts, 408, 68, 50, 50, "Button 3", 12, 10, 10, onEvent);
	TS_addButton(&ts, 408, 150, 50, 50, "Button 4", 12, 10, 10, onEvent);
	TS_addButton(&ts, 408, 150, 50, 50, "Button 5", 12, 10, 10, onEvent);
	TS_addButton(&ts, 408, 150, 50, 50, "Button 6", 12, 10, 10, onEvent);
	TS_addButton(&ts, 408, 150, 50, 50, "Button 7", 12, 10, 10, onEvent);
	TS_addButton(&ts, 408, 150, 50, 50, "Button 8", 12, 10, 10, onEvent);*/
	
	pthread_join(pth, NULL);
	FB_close(&fb);
	return 0;

}

void onEvent(int id, int state) {
	if (state) {
		printf("Button %i pressed !\n", id);
	} else {
		printf("Button %i released !\n", id);
	}
}

