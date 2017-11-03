#ifndef __PALE_VID_H__
#define __PALE_VID_H__


void initialise_display(void);
Uint8 getpixel(Uint16 offset, Uint8 bit);
int draw_scanline(int scanline);
void query_6845(char *emu_query3,char *emu_query4);
extern unsigned int crtc_reg;
extern unsigned char CRTC_reg[18];
void video_resize(int w,int h,int ressizable);
extern int StretchEnable; 
void set_screenres();
void set_screenres_tweak();
void video_fullscreen(int);

void draw_overlay();
void draw_overlay2();



int get_display_w();
int get_display_h();

extern unsigned char *vidmem;
extern unsigned char *vga_mem;
extern unsigned char vid_page;

extern int horiz_vidbytes;

void Retrace(void);
void vidpage_flip(unsigned int dont_flip);

#define VIDEO_PAGE1 0xa0000
//#define VIDEO_PAGE2 0xa4000
extern unsigned int VIDEO_PAGE2;	//changed in initialise display


#define DISPLAY_CUSTOM 0
#define DISPLAY_1X 1
#define DISPLAY_2X 2
#define DISPLAY_FULLSCREEN 3

#define LCD_LEFT_BORDER 12
#define HORIZ_BYTES_640x480 80
#define HORIZ_BYTES_LYNX 64

           
#endif 

