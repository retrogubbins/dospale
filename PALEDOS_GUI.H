#ifndef __PALEDOSGUI_H__
#define __PALEDOSGUI_H__
void update_status();
void init_gui();
void update_gui_mtype(int x);
void update_gui_speed(int x);
void update_fps_counter(int x);
void post_init_gui();
void gui_loop();
void gui_end();
void gui_error(char *lbl);
void update_ROMfiles();
void update_6845();
void load_TAP_file();
void set_memmap(unsigned int banknum,unsigned int bankres);
void update_sysvar();
void update_memmap();
void save_TAP_file();
void load_LEV9_tap();
void save_LSF_file();   // LYNX SNAP FORMAT
void load_LSF_file();   //LYNX SNAP FORMAT
void set_display(int x);
char* itoa_mine( int val, char* result, int base );
void set_SoundEnable(int x);

void set_memscan_start(int start);
void set_memscan_bno(int bannkno);
void gui_loop_more();
void gui_loop_x(int x);

void toggle();
void set_show_status(int x);
void set_show_memmap(int x);
void set_show_sysvars(int x);
void set_show_memscan(int x);
void set_show_tapemon(int x);
void set_memmap_bankno(int x);
void load_tape();
void save_tape();

void load_disk();
void save_disk();
void set_d_num(int c);

void update_tape_gui();
void update_memscan();
void update_disk_gui();

extern unsigned int memscan_bno;
extern unsigned int memscan_start;
extern unsigned int memmap_bankno;
extern unsigned int memmap_bankres;
extern int emu_display; //0 = custom  1=2x  2=3x (but I lie and 2x is passed off as 1x for example)

extern int show_disas;
extern int show_memmap;
extern int show_sysvars;
extern int show_memscan;
extern int show_tapemon;
extern int disas_bank;


//extern Window *window2;
//extern Window *window3;
extern unsigned char monitor_pic[256*128];

void update_tape_monitor();

/*
class MonitorWidget : public Widget {
        int w,h;
        unsigned char *imag;
        void draw() {
                imag=monitor_pic;
                drawimage(imag,MONO,Rectangle(w,h)); 
        };
public: 
        MonitorWidget(int x, int y, int imageW, int imageH)
                : Widget(x,y,imageW,imageH)
        {
                w = imageW;
                h = imageH;
        };
};


class TiffWidget : public Widget {
        int w,h;
        unsigned char *imag;
        void draw() {

        switch(memmap_bankno)
        {
                case 0:imag=bank0;
                                break;
                case 1:imag=bank1;
                                break;
                case 2:imag=bank2;
                                break;
                case 3:imag=bank3;
                                break;
                case 4:imag=bank4;
                                break;
        }
        drawimage(imag,MONO,Rectangle(w,h)); 
        };
public: 
        TiffWidget(int x, int y, int imageW, int imageH)
                : Widget(x,y,imageW,imageH)
        {
                w = imageW;
                h = imageH;
        };
};
*/


#endif
