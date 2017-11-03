#include<conio.h>
#include<dos.h>

#include "keydrv.h"

extern int pressed;
extern char key[];

static void (__interrupt  *old9h ) () ;  //holds dos key int



void __interrupt Key_handler()  {
    int scan,cv;

   // irenable();
    scan = inp(0x60);                //read scan code
    cv = inp(0x61);
    outp( 0x61, cv | 0x80 );
    outp( 0x61, cv );
    outp(0x20, 0x20);  //reset key int
    
    if(scan > 127)  {               //on break set keymap 0, dec pressed
        key[(scan & 127)] = 0;
        pressed -= 1;
    }
    else if(key[scan] == 0)  {  //on make set keymap 1, inc pressed
            pressed += 1;           //if not already set
        key[scan] = 1;
    }
    
}

void hook_keys()  {
    int i,cv;
    
    for(i=0; i<128; i++)  //set all keys to off
        key[i] = 0;
    pressed = 0;  //set number keys pressed to 0
        
    old9h = _dos_getvect ( 0x9 ); //save old key int
    _dos_setvect ( 0x9 , Key_handler );  //set key int to new handler
  cv = inp(0x61);
    outp( 0x61, cv | 0x80 );
    outp( 0x61, cv );
    outp(0x20, 0x20);  //reset key int
    //    outp(0x61,(inp(0x61)&~0x9));//get IRQ mask and enable new IRQ on PIC

}

void release_keys()  {
    _dos_setvect ( 0x9 , old9h ) ;  // Return to DOS keyboard driver
}

void waitkey(int key2)  {
    int temp = 0;
    while(!temp)
        if(key[key2])
            temp = 1;
}

void waitkeyup(int key2)  {
    int temp = 0;
    while(!temp)
        if(!key[key2])
            temp = 1;
}//end waitkeyup



