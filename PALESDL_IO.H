#ifndef __PALESDL_IO_H__
#define __PALESDL_IO_H__


#ifdef USE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_keyboard.h>
#include <SDL/SDL_keysym.h>
#endif

unsigned char lynx_inp( unsigned short port);
void lynx_outp(unsigned short port, unsigned char value);
void clearports();
//MOVE
extern unsigned char sound_port;
extern unsigned char video_latch,bank_latch;
extern unsigned char Line_Blank;
extern unsigned int scanline;
extern unsigned char show_allbanks,show_bank1,show_bank2,show_bank3,show_alt_green,speaker_enable;
void update_vid_bank_latches();

#endif 
