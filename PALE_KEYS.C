#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include "EMUSWITCH.h"

#include "PALESDL.H"
#include "PALEDOS_VID.H"
#include "PALEDOS_GUI.H"
#include "PALEDISK.H"
#include "PALESDL_IO.H"
#include "PALERAWTAPE.H"

#ifdef USE_DOS
#include <conio.h>
#include <bios.h>
#include "PALE_KEYS.H"
#include "keydrv.h"
#endif

#include "PALE48K.H"
#include "PALE96K.H"
#include "PALE128K.H"
#include "DOSDIR.H"



char key[256];
int pressed;

void init_keys()
{
    hook_keys(); 

        
}

void close_keys()
{
    release_keys();

}


void update_keyrate()
{
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        update_keyrate48K();
                        break;
                case LYNX_HARDWARE_96:
                        update_keyrate96K();
                        break;
                case LYNX_HARDWARE_128:
                        update_keyrate128K();
                        break;
        }
}

#include <graph.h>


void CheckKeys(void)
{
    //    Uint8 *key;
    int f;
    Uint8 mykey = 0;
     /* Only need a near pointer in the flat model */
    char *ptr;
    char name[200];
    char lbl[2000];
    char cmdresult[4000];
    static int key_toggle = 0;
        /* Clear all keypresses */
        z80ports_in[0x0080] = 0xFF;
        z80ports_in[0x0180] = 0xFF;
        z80ports_in[0x0280] = 0xFF;
        z80ports_in[0x0380] = 0xFF;
        z80ports_in[0x0480] = 0xFF;
        z80ports_in[0x0580] = 0xFF;
        z80ports_in[0x0680] = 0xFF;
        z80ports_in[0x0780] = 0xFF;
        z80ports_in[0x0880] = 0xFF;
        z80ports_in[0x0980] = 0xFF;

//if (key[SDLK_ESCAPE])finish_emu =1;
/*
for(f=0;f<256;f++)
{
    if(key[f])
        printf("%d ",f);
}
*/

//        SDL_PumpEvents();
  //      key = SDL_GetKeyState(NULL);

        /* User Keys */ 
/*
        if (key[SDLK_F1])run_emu=0;
        if (key[SDLK_F2])run_emu=1;
        if (key[SDLK_F3]) debug=1;
        if (key[SDLK_F4]) debug=0;
        if (key[SDLK_F7])
    {
        //ssize_t ret; 
        char linbuf[200];
        //ret=getline (&linbuf, NULL, stdin)
        gets(linbuf);
        // strtol (const char *string, char **tailptr, int base)
        trap_pc=strtol (linbuf, NULL, 16);
        printf("trap PC Set to %04X\n",trap_pc);
        }
*/
//iff (key[SDLK_F8])dump_mem();

//PPUT THIS BACK IN FOR DOSPALE

//if (key[SDLK_F7]) video_fullscreen(1);
//if (key[SDLK_F8]) video_fullscreen(0);
//if (key[SDLK_F3]) 
//if (key[SDLK_F4]) 

        if (key[SDLK_F1])
        {
                vidpage_flip(1); //Dont flip vidpage - write direct comes next
                show_help();
    //            waitkeyup(SDLK_F1);
      //      if(show_keycodes ==0)show_keycodes = 1;
        //    else show_keycodes = 0;

            }
        if (key[SDLK_F2])
        {
            waitkeyup(SDLK_F2);
            if(show_status ==0)show_status = 1;
            else show_status = 0;
        }
        if (key[SDLK_F3])
        {
            waitkeyup(SDLK_F3);
            if(run_emu ==0)run_emu = 1;
            else run_emu = 0;
        }
        
        if (key[SDLK_F4])
        {
            waitkeyup(SDLK_F4);
            emu_speed = (emu_speed + 1) % 6;
            set_speed(emu_speed);
        }
        if (key[SDLK_F5])
        {
        vidpage_flip(1); //Dont flip vidpage - write direct comes next
              my_move(0,10);
                          my_print("\f0");
                          my_paint(0,32,10,22);
                          my_print("\f5");
              my_input("Machine Type:\n\n\f10 = 48    \f45 = n/a \n\f21 = 96    \f53 = 96D\n\f32 = 128   \f64 = 128D\n          \f76=96DSK\n\n: ",name);
            set_machine(atoi(name));
            initialise_display();
        }
        if (key[SDLK_F6])
        {
            waitkeyup(SDLK_F6);
            if(show_status2 ==0)show_status2 = 1;
            else show_status2 = 0;
        }

        if (key[SDLK_F7])
        {
                vidpage_flip(1); //Dont flip vidpage - write direct comes next
            my_filechooser("lsf",name);
            load_LSF(name);
        }

        if (key[SDLK_F8])
        {
        vidpage_flip(1); //Dont flip vidpage - write direct comes next
            my_filechooser("tap",name);
            set_t_mode(0);
            if(load_lynx_tap(name,&cmdresult,0))
            {
                sprintf(lbl,"\nTape Loaded:\n%s\n",cmdresult);
                
                my_print("\f1");
                my_paint(0,30,0,20);
                my_print("\f7\a1");
                my_move(0,0);
                
                my_print(lbl);
            }
            my_input("\nPRESS RETURN ",name);
        }

        if (key[SDLK_F9])
                {
            waitkeyup(SDLK_F9);
                        initialise_Lynx();
                        return;
                } 

        if (key[SDLK_F10])
        {
                vidpage_flip(1); //Dont flip vidpage - write direct comes next
            my_filechooser("ldf",name);
                        load_ldisk(0,name);
        }


        if (key[SDLK_F11])
                {
            waitkeyup(SDLK_F11);
                        z80_reset();
                        return;
                }      
        if (key[SDLK_F12]) finish_emu=1;

        

      
        // Real Keyboard Table                          
        if (key[SDLK_1]) z80ports_in[0x0080] &= 0xFE;
//      if (key[SDLK_CAPSLOCK]) z80ports_in[0x0080] &= 0xF7; //NOT WORKING , LEADS TO REPEATING KEY

        if (key[SDLK_UP]) z80ports_in[0x0080] &= 0xEF;
        if (key[SDLK_DOWN]) z80ports_in[0x0080] &= 0xDF;
        if (key[SDLK_ESCAPE]) z80ports_in[0x0080] &= 0xBF;
        
        if ((key[SDLK_RSHIFT]) || (key[SDLK_LSHIFT])) z80ports_in[0x0080] &= 0x7F;

        if (key[SDLK_3]) z80ports_in[0x0180] &= 0xFE; // 01
        if (key[SDLK_4]) z80ports_in[0x0180] &= 0xFD;
// 02
        if (key[SDLK_e]) z80ports_in[0x0180] &= 0xFB;
// 04
        if (key[SDLK_x]) z80ports_in[0x0180] &= 0xF7;
// 08
        if (key[SDLK_d]) z80ports_in[0x0180] &= 0xEF;
// 10
    if (key[SDLK_c]) z80ports_in[0x0180] &= 0xDF; // 20
        if (key[SDLK_2]) z80ports_in[0x0280] &= 0xFE;
        if (key[SDLK_q]) z80ports_in[0x0280] &= 0xFD;
        if (key[SDLK_w]) z80ports_in[0x0280] &= 0xFB;
        if (key[SDLK_z]) z80ports_in[0x0280] &= 0xF7;
        if (key[SDLK_s]) z80ports_in[0x0280] &= 0xEF;
        if (key[SDLK_a]) z80ports_in[0x0280] &= 0xDF;
        if ((key[SDLK_RCTRL]) || (key[SDLK_LCTRL])) z80ports_in[0x0280] &= 0xBF;
        
        if (key[SDLK_5]) z80ports_in[0x0380] &= 0xFE;
        if (key[SDLK_r]) z80ports_in[0x0380] &= 0xFD;
        if (key[SDLK_t]) z80ports_in[0x0380] &= 0xFB;
        if (key[SDLK_v]) z80ports_in[0x0380] &= 0xF7;
        if (key[SDLK_g]) z80ports_in[0x0380] &= 0xEF;
        if (key[SDLK_f]) z80ports_in[0x0380] &= 0xDF;


        if (key[SDLK_6]) z80ports_in[0x0480] &= 0xFE;
        if (key[SDLK_y]) z80ports_in[0x0480] &= 0xFD;
        if (key[SDLK_h]) z80ports_in[0x0480] &= 0xFB;
        if (key[SDLK_SPACE]) z80ports_in[0x0480] &= 0xF7;
        if (key[SDLK_n]) z80ports_in[0x0480] &= 0xEF;
        if (key[SDLK_b]) z80ports_in[0x0480] &= 0xDF;

        if (key[SDLK_7]) z80ports_in[0x0580] &= 0xFE;
        if (key[SDLK_8]) z80ports_in[0x0580] &= 0xFD;
        if (key[SDLK_u]) z80ports_in[0x0580] &= 0xFB;
        if (key[SDLK_m]) z80ports_in[0x0580] &= 0xF7;
        if (key[SDLK_j]) z80ports_in[0x0580] &= 0xDF;

        if (key[SDLK_9]) z80ports_in[0x0680] &= 0xFE;
        if (key[SDLK_i]) z80ports_in[0x0680] &= 0xFD;
        if (key[SDLK_o]) z80ports_in[0x0680] &= 0xFB;
        if (key[SDLK_COMMA]) z80ports_in[0x0680] &= 0xF7;
        if (key[SDLK_k]) z80ports_in[0x0680] &= 0xDF;

        if (key[SDLK_0]) z80ports_in[0x0780] &= 0xFE;
        if (key[SDLK_p]) z80ports_in[0x0780] &= 0xFD;
        if (key[SDLK_l]) z80ports_in[0x0780] &= 0xFB;
        if (key[SDLK_PERIOD]) z80ports_in[0x0780] &= 0xF7;
        if (key[SDLK_SEMICOLON]) z80ports_in[0x0780] &= 0xDF;
        
        if (key[SDLK_MINUS]) z80ports_in[0x0880] &= 0xFE;
        if (key[SDLK_EQUALS]) z80ports_in[0x0880] &= 0xFD;
        if (key[SDLK_LEFTBRACKET]) z80ports_in[0x0880] &= 0xFB;
        if (key[SDLK_SLASH]) z80ports_in[0x0880] &= 0xF7;
        if (key[SDLK_QUOTE]) z80ports_in[0x0880] &= 0xDF;

        if (key[SDLK_BACKSPACE]) z80ports_in[0x0980] &= 0xFE;
        if (key[SDLK_RIGHTBRACKET]) z80ports_in[0x0980] &= 0xFD;        
        if (key[SDLK_LEFT]) z80ports_in[0x0980] &= 0xFB;
        if (key[SDLK_RETURN]) z80ports_in[0x0980] &= 0xF7;
        if (key[SDLK_RIGHT]) z80ports_in[0x0980] &= 0xDF;



}

void pump_key(char k)
{
        Uint8 *key;
        int f;
    //  int PUMP_DELAY=2000;
     //   int PUMP_DELAY=40;

      int PUMP_DELAY=80;


     k=tolower (k);
     
        // Real Keyboard Table                          
        if (k=='1') z80ports_in[0x0080] &= 0xFE;
//      if key[SDLK_CAPSLOCK]) z80ports_in[0x0080] &= 0xF7;
//      if key[SDLK_UP]) z80ports_in[0x0080] &= 0xEF;
//      if key[SDLK_DOWN]) z80ports_in[0x0080] &= 0xDF;
//      if key[SDLK_ESCAPE]) z80ports_in[0x0080] &= 0xBF;
        
        if(k=='\"')
        {
                z80ports_in[0x0080] &= 0x7F;//SHIFT
                z80ports_in[0x0280] &= 0xFE;// 2
        }

//      if ((key[SDLK_RSHIFT]) || (key[SDLK_LSHIFT])) z80ports_in[0x0080] &= 0x7F;

        if (k=='3') z80ports_in[0x0180] &= 0xFE; // 01
        if (k=='4') z80ports_in[0x0180] &= 0xFD;
// 02
        if (k=='e') z80ports_in[0x0180] &= 0xFB;
// 04
        if (k=='x') z80ports_in[0x0180] &= 0xF7;
// 08
        if (k=='d') z80ports_in[0x0180] &= 0xEF;
// 10
    if (k=='c') z80ports_in[0x0180] &= 0xDF; // 20
        if (k=='2') z80ports_in[0x0280] &= 0xFE;
        if (k=='q') z80ports_in[0x0280] &= 0xFD;
        if (k=='w') z80ports_in[0x0280] &= 0xFB;
        if (k=='z') z80ports_in[0x0280] &= 0xF7;
        if (k=='s') z80ports_in[0x0280] &= 0xEF;
        if (k=='a') z80ports_in[0x0280] &= 0xDF;
//      if ((key[SDLK_RCTRL]) || (key[SDLK_LCTRL])) z80ports_in[0x0280] &= 0xBF;
        
        if (k=='5') z80ports_in[0x0380] &= 0xFE;
        if (k=='r') z80ports_in[0x0380] &= 0xFD;
        if (k=='t') z80ports_in[0x0380] &= 0xFB;
        if (k=='v') z80ports_in[0x0380] &= 0xF7;
        if (k=='g') z80ports_in[0x0380] &= 0xEF;
        if (k=='f') z80ports_in[0x0380] &= 0xDF;


        if (k=='6') z80ports_in[0x0480] &= 0xFE;
        if (k=='y') z80ports_in[0x0480] &= 0xFD;
        if (k=='h') z80ports_in[0x0480] &= 0xFB;
        if (k==' ') z80ports_in[0x0480] &= 0xF7;
        if (k=='n') z80ports_in[0x0480] &= 0xEF;
        if (k=='b') z80ports_in[0x0480] &= 0xDF;

        if (k=='7') z80ports_in[0x0580] &= 0xFE;
        if (k=='8') z80ports_in[0x0580] &= 0xFD;
        if (k=='u') z80ports_in[0x0580] &= 0xFB;
        if (k=='m') z80ports_in[0x0580] &= 0xF7;
        if (k=='j') z80ports_in[0x0580] &= 0xDF;

        if (k=='9') z80ports_in[0x0680] &= 0xFE;
        if (k=='i') z80ports_in[0x0680] &= 0xFD;
        if (k=='o') z80ports_in[0x0680] &= 0xFB;
        if (k==',') z80ports_in[0x0680] &= 0xF7;
        if (k=='k') z80ports_in[0x0680] &= 0xDF;

        if (k=='0') z80ports_in[0x0780] &= 0xFE;
        if (k=='p') z80ports_in[0x0780] &= 0xFD;
        if (k=='l') z80ports_in[0x0780] &= 0xFB;
// unsure about these
        if (k=='.') z80ports_in[0x0780] &= 0xF7;
        if (k==';') z80ports_in[0x0780] &= 0xDF;
        
        if (k=='-') z80ports_in[0x0880] &= 0xFE;
//      if (key[SDLK_AT]) z80ports_in[0x0880] &= 0xFD;
        if (k=='(') z80ports_in[0x0880] &= 0xFB;
        if (k=='/') z80ports_in[0x0880] &= 0xF7;
        if (k==':') z80ports_in[0x0880] &= 0xDF;

//      if (key[SDLK_BACKSPACE]) z80ports_in[0x0980] &= 0xFE;
        if (k==')') z80ports_in[0x0980] &= 0xFD;        
//      if (key[SDLK_LEFT]) z80ports_in[0x0980] &= 0xFB;
        if (k=='\x0d') z80ports_in[0x0980] &= 0xF7;
//      if (key[SDLK_RIGHT]) z80ports_in[0x0980] &= 0xDF;

        //Make sure the keypress is recognised
        //in future check in the keyboard buffer on the ynx that they key gets there
        
        for(f=0;f<PUMP_DELAY;f++)
                z80_emulate(emu_cycles);


        // Clear all keypresses
        z80ports_in[0x0080] = 0xFF;
        z80ports_in[0x0180] = 0xFF;
        z80ports_in[0x0280] = 0xFF;
        z80ports_in[0x0380] = 0xFF;
        z80ports_in[0x0480] = 0xFF;
        z80ports_in[0x0580] = 0xFF;
        z80ports_in[0x0680] = 0xFF;
        z80ports_in[0x0780] = 0xFF;
        z80ports_in[0x0880] = 0xFF;
        z80ports_in[0x0980] = 0xFF;

        //Make sure the keypress is recognised
        //in future check in the keyboard buffer on the ynx that they key gets there
        for(f=0;f<PUMP_DELAY;f++)
                z80_emulate(emu_cycles);


}

void pump_string(char *s)
{
        int f;
        for(f=0;f<strlen(s);f++)
        {
                pump_key(s[f]);
        }
        for(f=0;f<100;f++)      //Give it time to settle
                z80_emulate(emu_cycles);

}

