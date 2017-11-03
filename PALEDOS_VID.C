#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#ifdef USE_DOS
#include <graph.h>
#endif

#ifdef USE_SDL
#include <SDL/SDL.h>
#include "sge_surface.h"
#include "sge_rotation.h"
#endif

#include "EMUSWITCH.h"


#include "PALESDL_IO.H"
#include "PALESDL.H"
#include "PALE48K.H"
#include "PALE96K.H"
#include "PALE128K.H"
#ifdef USE_FLTK
#include "PALE_FLTKGUI.H"
#endif
#include "PALE_VID.H"

//#define VIDEO_METHOD SDL_SWSURFACE|SDL_HWPALETTE
#define VIDEO_METHOD SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWPALETTE

#ifdef USE_SDL
//static SDL_Surface *screen;
static SDL_Surface *surface;
static SDL_Surface *surface2;
static SDL_Surface *screen;
#endif

int StretchEnable=0;    //Enables VIdeo Stretch function
unsigned char CRTC_reg[18];
unsigned int crtc_reg=0;


//  ONLY FOR SCALING
//  ONLY FOR SCALING
//  ONLY FOR SCALING
   /* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
       as expected by OpenGL for textures */
    Uint32 rmask, gmask, bmask, amask;


#ifdef USE_SDL
SDL_Color LynxPalette[8] = {{0,0,0,0},{0,0,255,0},{255,0,0,0},{255,0,255,0}, \
        {0,255,0,0},{0,255,255,0},{255,255,0,0},{255,255,255,0}};
#endif

void clearcrtc()
{
        for(int f=0;f<18;f++)
                CRTC_reg[f]=0;
}

void SetPixel ( int x , int y , Uint8 r, Uint8 g, Uint8 b)
{
/*
  //convert color
  Uint32 col = SDL_MapRGB ( pSurface->format ,r ,g ,b ) ;
  //determine position
  char* pPosition = ( char* ) pSurface->pixels ;
  //offset by y
  pPosition += ( pSurface->pitch * y ) ;
  //offset by x
  pPosition += ( pSurface->format->BytesPerPixel * x ) ;
  //copy pixel data
  memcpy ( pPosition , &col , pSurface->format->BytesPerPixel ) ;
*/
}

void video_fullscreen(int x)
{
        if(x)
        {
                emu_display=DISPLAY_FULLSCREEN;
                //Do This twice to make sure w2k blacksout the border area
//              screen = SDL_SetVideoMode(800,600, 8,  VIDEO_METHOD|SDL_FULLSCREEN);
//                screen = SDL_SetVideoMode(512,512, 8,  SDL_FULLSCREEN);
        }
}


void set_screenres()
{
        //Setup screen
        
        // Set palette
        SDL_SetColors(screen, LynxPalette, 0, 8);
}


void initialise_display(void)
{
    /* Initialize the SDL library (Video/Keyboard/Audio/Timer)*/
    set_screenres();
}


Uint8 getpixel(Uint16 offset, Uint8 bit)
{
        Uint8 colour;
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        colour=getpixel_48( offset,  bit);
                        break;
                case LYNX_HARDWARE_96:
                        colour=getpixel_96( offset,  bit);
                        break;
                case LYNX_HARDWARE_128:
                        colour=getpixel_128( offset,  bit);
                        break;
        }
        return colour;
}



//Returns ZERO if end of screen
int draw_scanline(int scanline)
{
        Uint8 *bits, *start_line;
        int memy, memx;
        unsigned int vidsize,vid_start,start_offset;
        int ret,hor_disp_pix,hor_disp_bytes,vrt_disp_pix,line,vrt_disp_6845pixperchar,vrt_disp_chars;
        float dest_1pix_height;
        int dest_height=256;
        int dest_width=512;
        char lbl[30];
        int sync_width,vert_sync_pos;


//      if(((video_latch & 0x22)!=0x20))
        if(((video_latch & 0x20)!=0x20))
        {
                hor_disp_bytes=CRTC_reg[1];
                if(hor_disp_bytes>64)hor_disp_bytes=64;
                if(hor_disp_bytes<2)hor_disp_bytes=2;
                hor_disp_pix=hor_disp_bytes*8;
                //32*8 = 256 for 4896   512 for 128
                vrt_disp_6845pixperchar=CRTC_reg[9]+1;  //usually 3+1 = 4
                vrt_disp_chars=CRTC_reg[6];  //usually 3f, 40 in Pengo
                vrt_disp_pix=vrt_disp_chars*vrt_disp_6845pixperchar;
if(vrt_disp_pix<32)vrt_disp_pix=32;
if(vrt_disp_pix>256)vrt_disp_pix=250;
                if (CRTC_reg[3]>0x37)
                        sync_width=(CRTC_reg[3]-0x37)/4;
                else
                        sync_width=0;
                if (CRTC_reg[6]>0x3f)
                        vert_sync_pos=(CRTC_reg[6]-0x3f)/4;
                else
                        vert_sync_pos=0;

        if(scanline>(vrt_disp_pix-1))
        {
                //Blank out last bit of display which doesnt get written
                //when there are only 252 lines
                //this can be changed once a stretch function is implemented
                if(scanline<255)
                {
                        while(scanline<255)
                        {
                                //Get Bits from screen backbuffer (lynx format)
                                //bits=((Uint8 *)screen->pixels+((scanline*1024)));
                                for(memx=0;memx<hor_disp_bytes;memx++)
                                {
                                        for(int x=0;x<8;x++)
                                        {
                                                //*((Uint8 *)(bits++)) = 0;
                                                //if(hw_type!=LYNX_HARDWARE_128)*((Uint8 *)(bits++)) =0;
                                        }
                                }
                                for(memx=0;memx<512;memx++) {
                                        //*((Uint8 *)(bits++)) = *((Uint8 *)(bits-512));
                                }
                                //Update screen
                                //SDL_UpdateRect(screen, 0,scanline*2,512,2);
                                scanline++;
                        }
                }
                return(0);//end of frame
        }
                //4000h for 128, 2000h for 48/96
        //      vidsize=hor_disp_bytes*vrt_disp_pix; //0x4000; horiz tot * (vert total*(char rast count +1))
                vid_start=(CRTC_reg[12]*256+CRTC_reg[13])*4 ;//0000 for 48/96  4000h for 128

                //if (vrt_disp_pix<10)vrt_disp_pix=10;
                //if(dest_height<vrt_disp_pix)dest_height=vrt_disp_pix+1;
                //dest_1pix_height=(float) dest_height /(float)vrt_disp_pix;

                //256 for 4896   512 for 128


if(hw_type!=LYNX_HARDWARE_128)
                vid_start=vid_start%0x2000;//for 4896, 4000 for 128
else
                vid_start=vid_start%0x4000;//for 4896, 4000 for 128

                start_offset=vid_start+(scanline*hor_disp_bytes);       //this is a CRTC register pete
                //start_offset=start_offset%0x4000;

//QUICK BODGE TO GET 128 MODE TO WORK WITH THIS SCALING STUFF
//Get start address of the scanline
//                bits=((Uint8 *)screen->pixels+((scanline*1024)+sync_width));

                start_line=bits;
                memy=start_offset;
                for(memx=0;memx<hor_disp_bytes;memx++) {
                        *((Uint8 *)(bits++)) = getpixel(memy + memx, 7);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy + memx, 6);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy+ memx, 5);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy+ memx, 4);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy+ memx, 3);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy+ memx, 2);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy+ memx, 1);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                        *((Uint8 *)(bits++)) = getpixel(memy+ memx, 0);
if(hw_type!=LYNX_HARDWARE_128)                  *((Uint8 *)(bits++)) = *((Uint8 *)(bits-1));
                }

if(!StretchEnable)
{
                for(memx=0;memx<512;memx++)
                        *((Uint8 *)(bits++)) = *((Uint8 *)(bits-512)); // Scan double the vertical lines
            SDL_UpdateRect(screen, 0,scanline*2,512,2);
}


        }       
        return(1);
}      


void query_6845(char *emu_query3,char *emu_query4)
{
        int ret,f,g;

        //Query the CRTC registers
        sprintf(emu_query3,"H TOT: %02x H DISP: %02x HS Pos: %02x HS Wid: %02x V TOT: %02x VT Adj: %02x V Disp: %02x VS Pos: %02x",
                CRTC_reg[0],
                CRTC_reg[1],
                CRTC_reg[2],
                CRTC_reg[3],
                CRTC_reg[4],
                CRTC_reg[5],
                CRTC_reg[6],
                CRTC_reg[7]);
                sprintf(emu_query4,"IntSkew: %02x MRastAddr: %02x CRS Start: %02x CRS End: %02x START: %04x CRS Addr: %04x LP: %04x",
                CRTC_reg[8],
                CRTC_reg[9],
                CRTC_reg[10],
                CRTC_reg[11],
                CRTC_reg[12]*256+CRTC_reg[13],
                CRTC_reg[14]*256+CRTC_reg[15],
                CRTC_reg[16]*256+CRTC_reg[17]);
        return;
}

int get_display_w()
{
        return(512);
}
int get_display_h()
{
        return(256);
}


void check_for_resize()
{
//    if(event.type==SDL_VIDEORESIZE)
//              video_resize(event.resize.w,event.resize.h,1);  
}


/*
void update_fps()
{
                //Every second update the fps counter
//FIXME get timer tick
//                this_time=SDL_GetTicks();
                if((this_time-last_time)>1000)
                {
                        update_fps_counter( fps_counter );
                        fps_counter=0;
                        last_time=this_time;
*/
                        
                
