/*
        MOUSE.C - The following program demonstrates how
        to use the mouse interrupt (0x33) with DOS/4GW.

        Compile and link: wcl386 /l=dos4g mouse
*/
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <i86.h>
#include <graph.h>

#define XMAX 600
#define YMAX 400

void m9ain (void)
{
    int f;
    int b1x=0,b1y=0;
    int b2x=100,b2y=50;

    int b1xinc=1, b1yinc=1;
    int b2xinc=1, b2yinc=1;


    int oldx1,oldy1;
    int oldx2,oldy2;

    _setvideomode(16);

    _setcolor(2);


    for (f=0;f<10000;f++)
    {
        oldx1 = b1x;
        oldy1 = b1y;
        oldx2 = b2x;
        oldy2 = b2y;

       // b1xinc = b1xinc * 0.1;

       // b2yinc = b2yinc * 0.1;

        b1x += b1xinc;
        b1y += b1yinc;
        b2x += b2xinc;
        b2y += b2yinc;

        if(b1x>XMAX || b1x<b1xinc)
            b1xinc = -b1xinc;
        if(b1y>YMAX || b1y<b1yinc)
            b1yinc = -b1yinc;
        if(b2x>XMAX || b2x<b2xinc)
            b2xinc = -b2xinc;
        if(b2y>YMAX || b2y<b2yinc)
            b2yinc = -b2yinc;


        _setcolor(f/10);

        _moveto(b1x,b1y);
        _lineto(b2x,b2y);
       _moveto(b1x+1,b1y);
        _lineto(b2x+1,b2y);

    //    _setcolor(0);
        
    //    _moveto(oldx1,oldy1);
     //   _lineto(oldx2,oldy2);

    }

    printf( "OKay press enter mode\n" );
    getc( stdin );

    _setvideomode(-1);

}
