#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <dos.h>
#include <i86.h>
#include <graph.h>

#include "PALESDL.H"

#ifdef USE_SDL
  #include <SDL/SDL.h>
  #include "sge_surface.h"
  #include "sge_rotation.h"
#endif

#include "EMUSWITCH.h"

#include "PALESDL_IO.H"
#include "PALE48K.H"
#include "PALE96K.H"
#include "PALE128K.H"

#ifdef USE_FLTK
  #include "PALE_FLTKGUI.H"
#endif

#include "PALE_VID.H"

#ifdef USE_DOS
  #include "VGA_REG.H"
#endif

//#define VIDEO_METHOD SDL_SWSURFACE|SDL_HWPALETTE
#define VIDEO_METHOD SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWPALETTE

#ifdef USE_SDL
    static SDL_Surface *surface;
    static SDL_Surface *surface2;
    static SDL_Surface *screen;
#endif

int StretchEnable=0;    //Enables VIdeo Stretch function
unsigned char CRTC_reg[18];
unsigned int crtc_reg=0;

unsigned char *vidmem = VIDEO_PAGE1;

unsigned char *vga_mem = VIDEO_PAGE1;
unsigned int VIDEO_PAGE2 = VIDEO_PAGE1;

unsigned char vid_page = 0;


int horiz_vidbytes = 80;

#ifdef USE_DOS
   struct RGBcolour
   {
      unsigned char R,G,B;
   };

   struct RGBcolour a_palette[256];

void setpal(struct RGBcolour *pal) /* Set palette */
{
   int i;

   outp(0x3c6,0xff);
   for(i=0;i<256;++i)
   {
          outp(0x3c8,i);
          outp(0x3c9,pal[i].R);
          outp(0x3c9,pal[i].G);
          outp(0x3c9,pal[i].B);
   };
}

void set_palette()
{
 int x;
  
   a_palette[0].R=0;
   a_palette[0].G=0;
   a_palette[0].B=0;
   a_palette[1].R=0;
   a_palette[1].G=0;
   a_palette[1].B=63;
   a_palette[2].R=63;
   a_palette[2].G=0;
   a_palette[2].B=0;
   a_palette[3].R=63;
   a_palette[3].G=0;
   a_palette[3].B=63;
   a_palette[4].R=0;
   a_palette[4].G=63;
   a_palette[4].B=0;
   a_palette[5].R=0;
   a_palette[5].G=63;
   a_palette[5].B=63;
   a_palette[6].R=63;
   a_palette[6].G=63;
   a_palette[6].B=0;
   a_palette[7].R=63;
   a_palette[7].G=63;
   a_palette[7].B=63;
  
}

void plane(Uint8 x)
{
    Uint8 planeno;
    planeno = ( x & 3);
    /* select the map mask register */
    outp(0x3c4, 0x02);
    /* write 2^plane */
    outp(0x3c5, (Uint8)1 << planeno);

}


void Retrace(void)
{
   // while(inp(0x3DA) & 0x08 );
    while(!(inp(0x3DA) & 0x08 ));
}
             
#include "twkuser.h"

extern Register *vgamod256;
extern Register *vgamod512;


void initialise_display(void)
{
    char *ptr;
    int i;

    _setvideomode(_ERESCOLOR);//needed before ourreg below ?
    horiz_vidbytes = HORIZ_BYTES_640x480;//goes with ERESCOLOR mode

        if(useLCDmode == 0)
        {
            if(hw_type!=LYNX_HARDWARE_128)
            {
                outRegArray(&vgamod256, 29);
                horiz_vidbytes = HORIZ_BYTES_LYNX;
            }
            else
            {
                outRegArray(&vgamod512, 29);
                horiz_vidbytes = HORIZ_BYTES_LYNX;
            }
        }

        if(horiz_vidbytes == HORIZ_BYTES_LYNX)
                VIDEO_PAGE2 = 0xa4000;
        else
                VIDEO_PAGE2 = 0xa7000;
    set_palette();
    setpal(a_palette);
}


void close_display(void)
{
    _setvideomode(-1);
}

void vidpage_flip(unsigned int dont_flip)
{
        unsigned int high_addr;
        unsigned int low_addr;
        
        //Flip Video Pages
        if(usePageFlipping == 0)return;

        if(vid_page == 0)
        {
                vid_page = 1;
                vidmem=(unsigned char *)VIDEO_PAGE2;
                vga_mem=(unsigned char *)VIDEO_PAGE1;
        }
        else  
        {
                vid_page = 0;
                vidmem=(unsigned char *)VIDEO_PAGE1;
                vga_mem=(unsigned char *)VIDEO_PAGE2;
        }


     if(dont_flip == 0)
     {
        high_addr = (unsigned int)0x0C | ((unsigned int)vga_mem & 0xff00);
        low_addr = (unsigned int)0x0D  | ((unsigned int)vga_mem << 8);

        while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
        outpw(CRTC_INDEX, high_addr);
        outpw(CRTC_INDEX, low_addr);
        while (!(inp(INPUT_STATUS_1) & VRETRACE));
     }
//    Retrace();
}

#endif









void clearcrtc()
{
     int f;
     for(f=0;f<18;f++)
     {
                CRTC_reg[f]=0;
     }
}


Uint8 getbyte(Uint8 bank, Uint16 offset, Uint8 byte)
{
        Uint8 retbyte;
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        retbyte=getbyte_48(bank, offset,  byte);
                        break;
                case LYNX_HARDWARE_96:
                        retbyte=getbyte_96(bank, offset,  byte);
                        break;
                case LYNX_HARDWARE_128:
                        retbyte=getbyte_128(bank, offset,  byte);
                        break;
        }
        return retbyte;
/* // Put in draw scanline below for doing it byte by byte
                for(memx=0;memx<hor_disp_bytes;memx++)
                {
                    retbyte =  getbyte(bank,memy, memx);
                    if(!show_status)
                    {
                        *((Uint8 *)(bytes+memx)) = retbyte;
                        if(hw_type==LYNX_HARDWARE_128) *((Uint8 *)(bytes+memx)) = retbyte;
                    }
                }
*/
}

unsigned int getword(Uint8 bank, Uint16 offset, Uint8 byte)
{
        //NOT used - for speed all inline below
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        return getword_48(bank, offset,  byte);
                        break;
                case LYNX_HARDWARE_96:
                        return getword_96(bank, offset,  byte);
                        break;
                case LYNX_HARDWARE_128:
                        return getword_128(bank, offset,  byte);
                        break;
        }
        return 0;
}

//THIS VERSION IS INLINED FOR SPEED
//ALL FUNCTIONS CALLS REMOVED, AND MEMORY ACCESS FROM PALE4896128K.C have been brought inline
//Returns ZERO if end of screen
int draw_scanline(int scanline)
{
        Uint8 *bits, *start_line, *bytes, bank,retbyte;
        unsigned int *bnk2;
        unsigned int retword,skip;
                int memy, memx, memy2;
        int left_border;
        int top_border;
        unsigned int vidsize,vid_start,start_offset;
        int x,ret,hor_disp_pix,hor_disp_bytes,vrt_disp_pix,line,vrt_disp_6845pixperchar,vrt_disp_chars;
        float dest_1pix_height;
        char lbl[30];
        int sync_width,vert_sync_pos;

                //This afects Level9 games
                //and is bound to the lineblanking monostable
                //0x20 != 0x20  doesnt work for lev9
//0x60 != 0x20 seems to be pretty ok at 100% speed and works for lev9 but possibly some artifacts in Twinkle GEM logo and pengo

                
        if(((video_latch & 0x20)!=0x20))
    //    if(((video_latch & 0x60)!=0x20))
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
               //when there are only 252 lines  ?254 lines  (248 BASIC + 6 above/below)?  See LU1
               //this can be changed once a stretch function is implemented
               if(scanline<255)
               {
                   while(scanline<255)
                   {
                        bytes=vidmem+(scanline*horiz_vidbytes);   //80 bytes
                        if(horiz_vidbytes == HORIZ_BYTES_640x480)
                            bytes+=LCD_LEFT_BORDER;  //shift for 64x480 mode
                        for(memx=0;memx<hor_disp_bytes;memx+=4)
                        {
                           *((unsigned int *)(bytes+memx)) = 0;
                        }
                        scanline++;
                   }
               }

  /*              if(useScanlineUpdates == 0) // Update full screen
                         SDL_UpdateRect(screen, 0,0,512,512);
               {
                    for(bank = 0;bank < 3; bank++)
                    {
                                plane(bank);
                                if(skip == 0)//this isnt right yet - skip should be two flags one for each visiblke bank red/blu and green/alt
                                {
                                        //memcpy ( void * destination, const void * source, size_t num );
                                        memcpy ( bytes, bnk2, hor_disp_bytes );  //copies ?32 64 or 80 bytes to the screen (or backbuffer)
                                }
                                else
                                {
                                        memset ( bytes, 0x00,hor_disp_bytes ); //emulates effect of lynx bank visibility bits
                                }
                 }
              */ 
               return(0);//end of frame
            }

            //4000h for 128, 2000h for 48/96
            vid_start=(CRTC_reg[12]*256+CRTC_reg[13])*4 ;//0000 for 48/96  4000h for 128
            if(hw_type!=LYNX_HARDWARE_128)
                vid_start=vid_start%0x2000;//for 4896, 4000 for 128
            else
                vid_start=vid_start%0x4000;//for 4896, 4000 for 128

            start_offset=vid_start+(scanline*hor_disp_bytes);       //this is a CRTC register pete

            //Get start address of the scanline
            memy=start_offset;

            bytes=vidmem+(scanline*horiz_vidbytes);   //80 bytes
            if(horiz_vidbytes == HORIZ_BYTES_640x480)bytes+=LCD_LEFT_BORDER;

            for(bank = 0;bank < 3;bank++)
            {
                                skip = 0;
                                switch(hw_type)
                                {
                                    case LYNX_HARDWARE_48:
                                    case LYNX_HARDWARE_96:
                                                                switch(bank)
                                                                {
                                                                        case 0:
                                                                            if(show_bank2==1)
                                                                            {
                                                                                if (memy>0x1fff)
                                                                                {
                                                                                    bnk2 = (unsigned int *)&bank2[0x8000+memy];//blue mirror
                                                                                }
                                                                                else
                                                                                {
                                                                                    bnk2 = (unsigned int *)&bank2[0xa000+memy];//blue
                                                                                }
                                                                                break;
                                                                            }
                                                                            else
                                                                                skip =  1;
                                                                                break;          
                                                                        case 1:
                                                                            if(show_bank2==1)
                                                                            {
                                                                                if (memy>0x1fff)
                                                                                {
                                                                                    bnk2 = (unsigned int *)&bank2[0xa000+memy];//red mirror
                                                                                }
                                                                                else
                                                                                {
                                                                                    bnk2 = (unsigned int *)&bank2[0xc000+memy];//red
                                                                                }
                                                                                break;
                                                                            }
                                                                            else
                                                                                skip =  1;
                                                                                break;
                                                                        case 2:
                                                                            if(show_bank3==1 )
                                                                            {
                                                                                if (memy>0x1fff)
                                                                                {
                                                                                    if(show_alt_green==1)
                                                                                    {
                                                                                        bnk2 = (unsigned int *)&bank3[0x8000+memy];//alt green mirror
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        bnk2 = (unsigned int *)&bank3[0xa000+memy];//green mirror
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    if(show_alt_green==1)
                                                                                    {
                                                                                        bnk2 = (unsigned int *)&bank3[0xa000+memy];//alt green 
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        bnk2 = (unsigned int *)&bank3[0xc000+memy];//green 
                                                                                    }
                                                                                }
                                                                                break;
                                                                            }
                                                                            else
                                                                                skip =  1;
                                                                            break;
                                                                     }
                                                                     break;
                                case LYNX_HARDWARE_128:
                                                      //Important for CPM
                                                        memy2=memy%0x4000;
                                                        if(show_allbanks==1)
                                                        {
                                                                switch(bank)
                                                                {
                                                                        case 0:
                                                                                bnk2 =  (unsigned int *)&bank2[0x4000+memy2];  //red
                                                                                break;
                                                                        case 1:
                                                                                bnk2 =  (unsigned int *)&bank2[0x0000+memy2]; //blue
                                                                                break;
                                                                        case 2:
                                                                                if(show_alt_green==1)
                                                                                   bnk2 =  (unsigned int *)&bank2[0xc000+memy2];  //alt green mirror
                                                                                else
                                                                                   bnk2 =   (unsigned int *)&bank2[0x8000+memy2];  //green mirror
                                                                                break;
                                                                }
                                                                break;
                                                        }
                                                        else
                                                                skip =  1;
                                        break;
                        }
                
            if(usePageFlipping == 0 && (show_status == 1 || show_status2 == 1 || show_keycodes ==1))
            {

            }
            else 
            {
                                plane(bank);
                                if(skip == 0)//this isnt right yet - skip should be two flags one for each visiblke bank red/blu and green/alt
                                {
                                        //memcpy ( void * destination, const void * source, size_t num );
                                        memcpy ( bytes, bnk2, hor_disp_bytes );  //copies ?32 64 or 80 bytes to the screen (or backbuffer)
                                }
                                else
                                {
                                        memset ( bytes, 0x00,hor_disp_bytes ); //emulates effect of lynx bank visibility bits
                                }
            }
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


void draw_overlay()
{
    char txt1[1000],txt2[1000];

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
if(usePageFlipping == 0)vidmem = vga_mem; //
    my_print("\f0");
    my_paint(0,32,12,24);
    my_print("\f4\a0");
    my_move(0,12);
    disassemble (&txt2,0,0);
    my_print(txt2);

    my_print("\f0");
    my_paint(0,32,6,12);
    my_print("\f5");
    my_move(0,7);
    get_statusregs(&txt2);
    my_print(txt2);
    my_print("\f4");
    sprintf(txt1,"\n\f2Machine Type: \f5%d\f2   Emu Speed: \f5%d\f4",mc_type,emu_speed);
    my_print(txt1);
    my_print("\f7");
}

void draw_overlay2()
{
    char txt1[1000],txt2[1000];
if(usePageFlipping == 0)vidmem = vga_mem;//     vidpage_flip(1); //Dont flip vidpage - write direct comes next

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
    my_print("\f0");
    my_paint(0,32,5,24);
    my_print("\f5\a0");
    my_move(0,12);
        dpmi_status(&txt2);
    my_move(0,5);
    my_print(txt2);
    my_print("\f7");
}


void draw_overlay3()
{
    char txt1[1000],txt2[1000];
if(usePageFlipping == 0)vidmem = vga_mem;//     vidpage_flip(1); //Dont flip vidpage - write direct comes next

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
    my_print("\f0");
    my_paint(0,32,5,24);
    my_print("\f5\a0");
    my_move(0,12);
        keycodes_status(&txt2);
    my_move(0,5);
    my_print(txt2);
    my_print("\f7");
}



void doit()
{
    char txt1[500],txt2[500];

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);

    my_move(0,10);
    get_statusregs(&txt2);
    my_print(txt2);
    
//    my_print("Hello World\b\tKILLER\bD\n");
   // my_print(fred);

  //  my_print("\f3Hello\f4World");
}



void start_fonts()
{
    init_fonts();
    load_font("vga8x8.fnt");
}


void stop_fonts()
{
//    _unregisterfonts();

}



// non memcpy version
/*
                for(memx=0;memx<hor_disp_bytes;memx+=4)
                {
                    if(!show_status)
                    {
                                       switch(hw_type)
                                        {
                                                case LYNX_HARDWARE_48:
                                                case LYNX_HARDWARE_96:
                                                                                switch(bank)
                                                                                {
                                                                                        case 0:
                                                                                        if(show_bank2==1)
                                                                                                {
                                                                                                        if (memy>0x1fff)
                                                                                                        {
                                                                                                                bnk2 = (unsigned int *)&bank2[0x8000+memy+memx];//blue mirror
                                                                                                                retword =  bnk2[0];
                                                                                    }
                                                                                                        else
                                                                                                        {
                                                                                                                bnk2 = (unsigned int *)&bank2[0xa000+memy+memx];//blue
                                                                                                                retword =  bnk2[0];
                                                                                                }
                                                                                                }
                                                                                                else
                                                                                    retword =  0;
                                                                                                break;          
                                                                                        case 1:
                                                                                        if(show_bank2==1)
                                                                                        {
                                                                                        if (memy>0x1fff)
                                                                                                        {
                                                                                                                bnk2 = (unsigned int *)&bank2[0xa000+memy+memx];//red mirror
                                                                                                                retword =  bnk2[0];
                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                                bnk2 = (unsigned int *)&bank2[0xc000+memy+memx];//red
                                                                                                                retword =  bnk2[0];
                                                                                                        }
                                                                                }
                                                                                            else
                                                                                    retword =  0;
                                                                                                break;
                                                                                        case 2:
                                                                                        if(show_bank3==1 )
                                                                                        {
                                                                                        if (memy>0x1fff)
                                                                                        {
                                                                                        if(show_alt_green==1)
                                                                                                                {
                                                                                                                        bnk2 = (unsigned int *)&bank3[0x8000+memy+memx];//alt green mirror
                                                                                                                        retword = bnk2[0];
                                                                                        }
                                                                                                                else
                                                                                                                {
                                                                                                                        bnk2 = (unsigned int *)&bank3[0xa000+memy+memx];//green mirror
                                                                                                                        retword =  bnk2[0];
                                                                                                                }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                        if(show_alt_green==1)
                                                                                                                {
                                                                                                                        bnk2 = (unsigned int *)&bank3[0xa000+memy+memx];//alt green 
                                                                                                                        retword =  bnk2[0];
                                                                                        }
                                                                                                                else
                                                                                                                {
                                                                                                                        bnk2 = (unsigned int *)&bank3[0xc000+memy+memx];//green 
                                                                                                                        retword =  bnk2[0];
                                                                                                                }
                                                                                        }
                                                                                                }
                                                                                        else
                                                                                        retword =  0;
                                                                                                break;
                                                                                }
                                                        break;
                                                case LYNX_HARDWARE_128:
                                                                      //Important for CPM
                                                                        memy2=memy%0x4000;
                                                                        if(show_allbanks==1)
                                                                        {
                                                                                switch(bank)
                                                                                                {
                                                                                                        case 0:
                                                                                                                bnk2 =  (unsigned int *)&bank2[0x4000+memy2+memx];  //red
                                                                                                                break;
                                                                                                        case 1:
                                                                                                                bnk2 =  (unsigned int *)&bank2[0x0000+memy2+memx]; //blue
                                                                                                                break;
                                                                                        case 2:
                                                                                                                if(show_alt_green==1)
                                                                                                bnk2 =  (unsigned int *)&bank2[0xc000+memy2+memx];  //alt green mirror
                                                                                                else
                                                                                                bnk2 =   (unsigned int *)&bank2[0x8000+memy2+memx];  //green mirror
                                                                                                                break;
                                                                                                }
                                                                                        retword = bnk2[0];
                                                                        }
                                                                        else
                                                                                                retword =  0;
                                                        break;
                                        }
                        *((unsigned int *)(bytes+memx)) = retword;
                    }
                }


            }
*/











/*  Calling Version
//Returns ZERO if end of screen
int draw_scanline(int scanline)
{
        Uint8 *bits, *start_line, *bytes, bank,retbyte;
        unsigned int retword;
                int memy, memx;
        int left_border;
        int top_border;
        unsigned int vidsize,vid_start,start_offset;
        int x,ret,hor_disp_pix,hor_disp_bytes,vrt_disp_pix,line,vrt_disp_6845pixperchar,vrt_disp_chars;
        float dest_1pix_height;
        char lbl[30];
        int sync_width,vert_sync_pos;

        //VGA Memory Start
        unsigned char *vidmem=(unsigned char *)0xa0000;
                //return 0;




                //This afects Level9 games
                //and is bound to the lineblanking monostable
                //0x20 != 0x20  doesnt work for lev9
//0x60 != 0x20 seems to be pretty ok at 100% speed and works for lev9

        if(((video_latch & 0x60)!=0x20))
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
                        bytes=vidmem+(scanline*horiz_vidbytes);   //80 bytes
                        if(horiz_vidbytes == HORIZ_BYTES_640x480)bytes+=LCD_LEFT_BORDER;  //shift for 64x480 mode
                        //Get Bits from screen backbuffer (lynx format)
                        for(memx=0;memx<hor_disp_bytes;memx++)
                        {
                           *((Uint8 *)(bytes+memx)) = 0;
                           if(hw_type==LYNX_HARDWARE_128) *((Uint8 *)(bytes+memx)) = 0;
                        }
                        scanline++;
                   }
               }
            Retrace();
               return(0);//end of frame
            }
            //4000h for 128, 2000h for 48/96
            vid_start=(CRTC_reg[12]*256+CRTC_reg[13])*4 ;//0000 for 48/96  4000h for 128
            if(hw_type!=LYNX_HARDWARE_128)
                vid_start=vid_start%0x2000;//for 4896, 4000 for 128
            else
                vid_start=vid_start%0x4000;//for 4896, 4000 for 128

            start_offset=vid_start+(scanline*hor_disp_bytes);       //this is a CRTC register pete

            //Get start address of the scanline
            memy=start_offset;
            for(bank = 0;bank < 3;bank++)
            {
                plane(bank);
                bytes=vidmem+(scanline*horiz_vidbytes);   //80 bytes
                if(horiz_vidbytes == HORIZ_BYTES_640x480)bytes+=LCD_LEFT_BORDER;
                for(memx=0;memx<hor_disp_bytes;memx+=4)
                {
                    if(!show_status)
                    {
                                       switch(hw_type)
                                        {
                                                case LYNX_HARDWARE_48:
                                                        retword = getword_48(bank, memy, memx);
                                                        break;
                                                case LYNX_HARDWARE_96:
                                                        retword = getword_96(bank, memy, memx);
                                                        break;
                                                case LYNX_HARDWARE_128:
                                                        retword = getword_128(bank, memy, memx);
                                                        break;
                                        }
                        *((unsigned int *)(bytes+memx)) = retword;
                    }
                }


            }
        }       
        return(1);
                
}      
*/




