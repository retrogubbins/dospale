#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dos.h>
#include <i86.h>
#include <graph.h>



//This next line includes one of the two Z80 emulations
//either the RAZE x86 assembly core, or the compiled C KOGEL core
#include "EMUSWITCH.h"

#include "PALESDL.H"
#include "PALE_VID.H"
#include "PALESDL_IO.H"

#include "PALEDOS_GUI.H"
#include "PALETAPS.H"
#include "PALESND.H"
#include "PALESDL_CONFIG.H"
#include "PALEMEM.H"
#include "PALEROM.H"
#include "PALE_KEYS.H"
#include "PALERAWTAPE.H"
#include "PALEDISK.H"

//Some status info routines and disassembler for the Z80 core
#include "CORE_STATUS.H"


#ifdef WIN32
#include "PALEWIN32_HEAD.H"
#endif // WIN32 Routine to Position the main SDL window

#define DEBUG 0

//using namespace std;
//using namespace fltk;

//MEMORY IO STUFF
UBYTE    z80ports_in[0x10000];          /* I/O ports */
UBYTE    z80ports_out[0x10000];          /* I/O ports */
UBYTE    bank0[LYNX_MAXMEM];              /* Rom */
UBYTE    bank1[LYNX_MAXMEM];              /* User Ram */
UBYTE    bank2[LYNX_MAXMEM];          /* Red 0x0000/Blue 0x2000*/
UBYTE    bank3[LYNX_MAXMEM];              /* AltGreen 0x0000/Green 0x2000*/
UBYTE    bank4[LYNX_MAXMEM];

float emuspeeds[10]={25,50,100,200,400,800,1000,2000,5000,32000};
int emu_speed=2;
int hw_type=LYNX_HARDWARE_48;
int mc_type=0;
int ss_emu=0;
int run_emu=1;
int finish_emu=0;
int debug=0;
int SoundEnable=0;
int fps_counter=0;

int useLCDmode = 0;
int usePageFlipping = 1;
int useScanlineUpdates = 1;

int trap_pc=0xffff;
double  time_per_fps_disp;
double  last_fps_time;
int newtime;
int framecount;
unsigned int scanline;

long emu_cycles_scanline=256;
long emu_cycles_lineblank=88;
long emu_cycles_vblank=400;
long emu_cycles=13000;
//long emu_cycles=4;

unsigned int sound_cycles=0;
unsigned int tapecyc=0; 
int trace=0;
FILE *tracedump_file;
char *loadfilename;


#define XMAX 600
#define YMAX 400


float get_pale_rev()
{
        return((float)PALE_REV);
}

void start_tracedump()
{
                char lbl[50];
                tracedump_file=fopen("TRACEDUMP.TXT","wt");
        if(tracedump_file==NULL)
        {
                sprintf(lbl,"Cannot open PALE TraceDUMP File for writing");
                                gui_error(lbl);
                return;
        }
}
void stop_tracedump()
{
        fclose(tracedump_file);
}

void tracedump(UWORD pc)
{
        char lbl[200];
        static int start=0;
  
        if(z80_get_reg(Z80_REG_PC)==0xe000)
                start=1;
        if(start==1)
        {
                get_statusregs(lbl);
                fprintf(tracedump_file,lbl);
                fprintf(tracedump_file,"\n");
        }
}

void bit(unsigned char *vari,unsigned char posit,unsigned char valu)
{
        if (valu==0)
                *vari=(*vari & (255-((unsigned char)1 << posit)));
        else
                *vari=(*vari | ((unsigned char)1 << posit));
}

void set_speed(int x)
{
        emu_speed=x;
        update_gui_speed(x);
        saveconfigfile();

        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        emu_cycles_scanline=(long)(emuspeeds[x]*256)/100; //256 = 64us Scanline Period
                        emu_cycles_lineblank=(long)(emuspeeds[x]*88)/100; //88 = 22us LineBlank Period
                        emu_cycles_vblank=(long)(emuspeeds[x]*160)/100;  //160= 40us Vblank(IRQ LOW) Period
                        emu_cycles=(long)(emuspeeds[x]*13600)/100;  //13600 = 3.4ms Vblank period
                        break;
                case LYNX_HARDWARE_96:
                        emu_cycles_scanline=(long)(emuspeeds[x]*256)/100; //256 = 64us Scanline Period
                        emu_cycles_lineblank=(long)(emuspeeds[x]*88)/100; //88 = 22us LineBlank Period
                        emu_cycles_vblank=(long)(emuspeeds[x]*160)/100;  //160= 40us Vblank(IRQ LOW) Period
                        emu_cycles=(long)(emuspeeds[x]*13600)/100;  //13600 = 3.4ms Vblank period
                        break;
                case LYNX_HARDWARE_128:
                        emu_cycles_scanline=(long)(emuspeeds[x]*384)/100; // = 64us Scanline Period
                        emu_cycles_lineblank=(long)(emuspeeds[x]*132)/100; // = 22us LineBlank Period
                        emu_cycles_vblank=(long)(emuspeeds[x]*240)/100;  //= 40us Vblank(IRQ LOW) Period
                        emu_cycles=(long)(emuspeeds[x]*20400)/100;  // = 3.4ms Vblank period
                        break;
        }
}

void init_fps()
{
        // called once, during the init phase 
        time_per_fps_disp = CLOCKS_PER_SEC * .5; // .5 seconds 
}

void single_step_emu()
{
        ss_emu=1;
}

void tick_fps()
{
        // called every frame
        last_fps_time = newtime;
        newtime = clock();
        framecount++;
        if ((newtime-last_fps_time) > time_per_fps_disp)
        {
            update_fps_counter( (int)((CLOCKS_PER_SEC * framecount) / (newtime -last_fps_time)));
            framecount=0;
        }
}



void initialise_Lynx(void)
{

        //gui_error("In Init Lynx");
        memset(&bank0[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank1[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank2[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank3[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank4[0x0000], 0xFF, LYNX_MAXMEM);
        clearports();
        load_romset(mc_type);
        if(!load_lynx_rom())
        {
                gui_error("Couldn't open the working ROM");
                exit(1);
        }
        //save_memdump();
        initmem();//Must come after ROMset load so we know what the hardware is
        set_t_mode(tape_mode);
        init_tapestuff();
        init_diskstuff();

    // z80_set_fetch_callback(&tracedump);
        z80_reset(); 
        //gui_error("OUT OF Init Lynx");

}


void set_machine(int x)
{
        mc_type=x;
        update_gui_mtype(x);

        initialise_Lynx();
        saveconfigfile();

        set_speed(emu_speed);
}


  
// ------------------------------------------------------------------ 
//#undef main
int main(int argc, char *argv[])
{
        FILE *handle;
        char *filenam;
        static char scan_lbl[10];
        static int last_time;
        int f_delay=10;
        int this_time,wait_time,frame_time;

        char cmd[20];
        int i;
        for (i = 1; i < argc; )
        {
          printf("%s", argv[i]);
          if (++i < argc)
               putchar(' ');
        }
 //      init_keys();
//printf("OK  waiting for keypress");

//        putchar('\n');
//waitkey(SDLK_F2);
//close_keys();
//exit(1);

        //MUST BE HERE before Initgui FOR WINXP to WORK
        initialise_display();//Must be here because loading COnfig file will reset the display size
        
        start_fonts();
      
        printf("Starting fonts\n");
  

        //strcpy(argv[1],&cmd);
  //      loadfilename = argv[1];
    //    printf("Load FIlename is %s\n",argv[1]);

for(i = 1;i<argc;i++)
{
    if(strcmp(argv[i],"-lcd") == 0)
    {
        useLCDmode = 1;
    }

    if(strcmp(argv[i],"-noflip") == 0)
    {
        usePageFlipping = 0;
    }
}


//// my_print("TESTING\n");
//waitkey(SDLK_F2);
//close_keys();
//exit(1);
        
        init_gui();
        init_ROMsets();
        if(!loadROMconfigfile())
        {
                saveROMconfigfile();
                loadROMconfigfile();
        }

        #  ifdef WIN32
        #include "PALEWIN32_GET.H"
        #  endif

        printf("loading config\n");
        if(!loadconfigfile())
        {
                saveconfigfile();
                loadconfigfile();
        }
        printf("Initialising display\n");
        initialise_display();//Must be here because loading COnfig file will reset the display size

   //     printf("Initialising sound\n");
        init_sound();

   //     printf("Initialising Lynx\n");
        initialise_Lynx();
        //init_fps();
   //     printf("Initialising Keyboard\n");
        init_keys();

        post_init_gui();

        scanline = 0;

        gui_loop_more();        //Little dose of GUI to start with - get all the windows open etc.

        //start_tracedump();
        //z80_set_fetch_callback(void (*handler)(UWORD pc));
        //z80_set_fetch_callback(&tracedump);

        show_status =0;
        run_emu = 1;

    while (finish_emu==0) 
    {   
                                CheckKeys();
                
                                if(run_emu==1)
                {
                        //to get at least 22khz sampling on sound 250*50fps ish chek!
                        //NOTE!! This division will copmletely screw up the 
                        //point of setting a definite noof cycles
                        //so change this NOW!!!
                        
                                                //Line_Blank is a global for the blanking monostable
                                                //when portXX is set the z80 freezes till end of next scanline i.e. here!
                                                //it is then reset

                                                Line_Blank=0;
//This bit needs fixing for PENGO to work without tearing appearing
//pengo works on performing video writes durin gthe line blanking period
//wwhen linkblank bit is set the z80 should freeze until the start of the blanking periood
                                        sound_cycles+=z80_emulate(emu_cycles_lineblank);//22 us Line Blanking Execution
//These next two shouldn't be run here, certainly not if the emulate above got stopped by z80_stop_emulating()
//which is set off by the line mono being written to (out port)

//if(Line_Blank == 0)
//                                             sound_cycles+=z80_emulate(emu_cycles_scanline/2);//64 us Scanline execution
//if(Line_Blank == 0)
//                                             sound_cycles+=z80_emulate(emu_cycles_scanline/2);//64 us Scanline execution

                                             sound_cycles+=z80_emulate(emu_cycles_scanline);//64 us Scanline execution

//                                        sound_cycles+=z80_emulate(emu_cycles_lineblank + emu_cycles_scanline );//22 us Line Blanking Execution

                }
                else
                {
                        if(ss_emu==1)
                        {
                                        z80_emulate(1);
                                ss_emu=0;
                        }
                        //                      else
                        //                              gui_loop_more();                        //GIVE SOME EXTRA TIME TO THE FLTK GUI
                                                                                                                //WHEN NOT RUNNING
                                                                                                                //Otherwise there are problems with Input Text Boxes
                }               

                // FRAME BLANKING PERIOD
                // FRAME BLANKING PERIOD
                // FRAME BLANKING PERIOD
                if(draw_scanline(scanline++)==0)        //draw returns zero if its end of frame
                {
                        if(show_sysvars)update_sysvar();
                        if(show_memmap)update_memmap();
                        if(show_status)draw_overlay();
                        if(show_status2)draw_overlay2();
                        if(show_keycodes)draw_overlay3();
                        if(show_tapemon)update_tape_monitor();
                        if(show_memscan)update_memscan();

Retrace();
                                    vidpage_flip(0);//send 0 in to flip the screen pages
                        update_keyrate(); //Update the keyboard repetition and delay rates for diff emu speeds
                        CheckKeys();
                        scanline = 0;
                        fps_counter++;
                        gui_loop();
                                
                        z80_raise_IRQ(0xFF);
                        //Should be 160 T States with IRQ held low by CURSOR 6845 signal
                        //if(run_emu==1)        sound_cycles+=z80_emulate(emu_cycles_vblank);
                        z80_lower_IRQ();

                        if(run_emu==1)
                        {
                                sound_cycles+=z80_emulate(emu_cycles);//main impact of speedup
                        }
                }
        }
        //stop_tracedump();
        kill_disks();
        gui_end();
        close_display();
        close_keys();
        return 1;
}

void stop_emu()
{
        run_emu=0;
}
void start_emu()
{
        run_emu=1;
}
void quit_emu()
{
        finish_emu=1;
}
void reset_emu()
{
        initialise_Lynx();
}
