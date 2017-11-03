#include <stdio.h>

#include "EMUSWITCH.h"

#include "PALESDL.H"
#include "PALEDOS_VID.H"
#include "PALEDOS_GUI.H"
#include "PALEDISK.H"
#include "PALESDL_IO.H"
#include "PALERAWTAPE.H"

UBYTE Read_Mem96(UWORD A);


void  Write_Mem96(UWORD A, UBYTE V)
{
                if((bank_latch & 0x01)==0)
                {
                        bank1[A]=V;
                }
                if((bank_latch & 0x02)==0x02)
                {
                        if ((video_latch & 0x04)==0) 
                        {
                                if(A<0x2000)
                                        bank2[A+0xa000]=V;                              // mirror
                                else if(A>=0x2000 && A<0x4000)
                                        bank2[A+0x8000]=V;                              // mirror
                                else if(A>=0x4000 && A<0x6000)
                                        bank2[A+0x8000]=V;                              // mirror
                                else if(A>=0x6000 && A<0x8000)
                                        bank2[A+0x6000]=V;                              // mirror
                                else if(A>=0x8000 && A<0xa000)
                                        bank2[A+0x2000]=V;                              // mirror
                                else if(A>=0xa000 && A<0xe000)
                                        bank2[A]=V;                             // BLUE RED
                                else if(A>=0xe000)
                                        bank2[A-0x2000]=V;                              // mirror
                        }
                }
                if((bank_latch & 0x04)==0x04)
                {
                        if ((video_latch & 0x08)==0) 
                        {
                                if(A<0x2000)
                                        bank3[A+0xa000]=V;                              // mirror
                                else if(A>=0x2000 && A<0x4000)
                                        bank3[A+0x8000]=V;                              // mirror
                                else if(A>=0x4000 && A<0x6000)
                                        bank3[A+0x8000]=V;                              // mirror
                                else if(A>=0x6000 && A<0x8000)
                                        bank3[A+0x6000]=V;                              // mirror
                                else if(A>=0x8000 && A<0xa000)
                                        bank3[A+0x2000]=V;                              // mirror
                                else if(A>=0xa000 && A<0xe000)
                                        bank3[A]=V;                             //AGREEN  GREEN
                                else if(A>=0xe000)
                                        bank3[A-0x2000]=V;                              // mirror

                        }
                }
}



void initmem96k()
{
        z80_init_memmap();
        z80_map_fetch(0x0000, 0x1FFF,&bank0[0x0000]);
        z80_map_fetch(0x2000, 0x3FFF,&bank0[0x2000]);
        z80_map_fetch(0x4000, 0x5FFF,&bank0[0x4000]);
        z80_map_fetch(0x6000, 0x7FFF,&bank0[0x6000]);
        z80_map_fetch(0x8000, 0x9FFF,&bank0[0x8000]);
        z80_map_fetch(0xA000, 0xBFFF,&bank0[0xA000]);
        z80_map_fetch(0xC000, 0xDFFF,&bank0[0xC000]);
        z80_map_fetch(0xE000, 0xFFFF,&bank0[0xE000]);

        z80_map_read(0x0000, 0x1FFF,&bank0[0x0000]);
        z80_map_read(0x2000, 0x3FFF,&bank0[0x2000]);
        z80_map_read(0x4000, 0x5FFF,&bank0[0x4000]);
        z80_map_read(0x6000, 0x7FFF,&bank0[0x6000]);
        z80_map_read(0x8000, 0x9FFF,&bank0[0x8000]);
        z80_map_read(0xA000, 0xBFFF,&bank0[0xA000]);
        z80_map_read(0xC000, 0xDFFF,&bank0[0xC000]);
        z80_map_read(0xE000, 0xFFFF,&bank0[0xE000]);


        z80_add_write(0x0000, 0xFFFF, Z80_MAP_HANDLED,(void *) &Write_Mem96);

//      z80_add_read(0x0000, 0xFFFF, Z80_MAP_HANDLED,(void *) &Read_Mem96);

        z80_set_in(&lynx_inp);
        z80_set_out(&lynx_outp);
        z80_end_memmap();
}



void update_vid_maps96k()
{
        if ((bank_latch & 0x40)==0x40)
        {
                if ((video_latch & 0x04)==0)
                {
                        z80_map_read(0x0000, 0x1FFF, &bank2[0xa000]); //Mirror o
 //             z80_map_fetch(0x0000, 0x1FFF, &bank2[0xa000]); //Mirror o
                        z80_map_read(0x2000, 0x3FFF, &bank2[0xa000]); //Mirror o
 //                     z80_map_fetch(0x2000, 0x3FFF, &bank2[0xa000]); //Mirror o
                        z80_map_read(0x4000, 0x5FFF, &bank2[0xc000]); //Mirror
 //                     z80_map_fetch(0x4000, 0x5FFF, &bank2[0xc000]); //Mirror
                        z80_map_read(0x6000, 0x7FFF, &bank2[0xc000]); //Mirror o
//                      z80_map_fetch(0x6000, 0x7FFF, &bank2[0xc000]); //Mirror o
                                        // IMPORTANT ONES
                        z80_map_read(0x8000, 0x9FFF, &bank2[0xa000]); // o
//                      z80_map_fetch(0x8000, 0x9FFF, &bank2[0xa000]); // o
                                z80_map_read(0xa000, 0xBFFF, &bank2[0xa000]); // BLUE bank
//                      z80_map_fetch(0xa000, 0xBFFF, &bank2[0xa000]); // BLUE bank
                                        z80_map_read(0xc000, 0xDFFF, &bank2[0xc000]); // RED bank  unc 2giv halfred     
//                      z80_map_fetch(0xc000, 0xDFFF, &bank2[0xc000]); // RED bank  unc 2giv halfred            
                                z80_map_read(0xa000, 0xDFFF, &bank2[0xa000]); // BLUE bank
//                      z80_map_fetch(0xa000, 0xDFFF, &bank2[0xa000]); // BLUE bank
                                z80_map_read(0xE000, 0xFFFF, &bank2[0xc000]); //Mirror o
//                      z80_map_fetch(0xE000, 0xFFFF, &bank2[0xc000]); //Mirror o
                                        // IMPORTANT ONES
                }


                if ((video_latch & 0x08)==0) 
                {               
                                z80_map_read(0x0000, 0x1FFF, &bank3[0xa000]); //Mirror o
//                      z80_map_fetch(0x0000, 0x1FFF, &bank3[0xa000]); //Mirror o
                        z80_map_read(0x2000, 0x3FFF, &bank3[0xa000]); //Mirror o
//                      z80_map_fetch(0x2000, 0x3FFF, &bank3[0xa000]); //Mirror o
                                z80_map_read(0x4000, 0x5FFF, &bank3[0xc000]); //Mirror
//                      z80_map_fetch(0x4000, 0x5FFF, &bank3[0xc000]); //Mirror
                        z80_map_read(0x6000, 0x7FFF, &bank3[0xc000]); //Mirror o
//                      z80_map_fetch(0x6000, 0x7FFF, &bank3[0xc000]); //Mirror o
                                        // IMPORTANT ONES
                        z80_map_read(0x8000, 0x9FFF, &bank3[0xa000]); //Mirror o
//                      z80_map_fetch(0x8000, 0x9FFF, &bank3[0xa000]); //Mirror o
                                        z80_map_read(0xa000, 0xBFFF, &bank3[0xa000]); // ALT GREEN bank
//                      z80_map_fetch(0xa000, 0xBFFF, &bank3[0xa000]); // ALT GREEN bank
                                        z80_map_read(0xc000, 0xDFFF, &bank3[0xc000]); // GREEN bank
//                      z80_map_fetch(0xc000, 0xDFFF, &bank3[0xc000]); // GREEN bank
                                z80_map_read(0xE000, 0xFFFF, &bank3[0xc000]); //Mirror o
//                      z80_map_fetch(0xE000, 0xFFFF, &bank3[0xc000]); //Mirror o
                                        // IMPORTANT ONES
                }
        }
}



void bank_latch96k(unsigned char value)
{
    bank_latch=value;


//return;

        //if(debug==1)printf("Bank Switch %2x",value);
        if ((value & 0x40)==0x40)                       //READ 2 & 3 Enable 
                update_vid_maps96k();

        if ((value & 0x20)==0) 
        {               //RAM bank1 READ ENABLE
                z80_map_read(0x0000, 0xFFFF, &bank1[0x0000]); 
                z80_map_fetch(0x0000, 0xFFFF, &bank1[0x0000]); 
        }


    //ROM COMES LAST SO IT TAKES PRIORITY OVER THE OTHERS
        if ((value & 0x10)==0)
    {
                z80_map_read(0x0000, 0x1FFF, &bank0[0x0000]); // rom 
                z80_map_read(0x2000, 0x3FFF, &bank0[0x2000]); // rom 
                z80_map_read(0x4000, 0x5FFF, &bank0[0x4000]); // rom 

                z80_map_fetch(0x0000, 0x1FFF, &bank0[0x0000]);
        z80_map_fetch(0x2000, 0x3FFF, &bank0[0x2000]);
        z80_map_fetch(0x4000, 0x5FFF, &bank0[0x4000]);

                if(bank0[0xe000]!=0xff) // && (disk_rom_enabled==1))    // Disk rom is special - can be turned off
                {
                        if((disk_options & 0x10)==0)
                        {
                                z80_map_read(0xe000, 0xFFFF, &bank0[0xe000]); // disk rom 
                        z80_map_fetch(0xe000, 0xFFFF, &bank0[0xe000]);
                        }
                }
        }
}



void vid_latch96k(UBYTE value)
{
        video_latch=value;
        if((video_latch & 0x10)==0x10)  //0= show green bank, 1=alt green
                show_alt_green=1;
        else
                show_alt_green=0;

        if(video_latch & 0x04)// && (video_latch & 0x20)==0x00) //0= show red/blue bank, 1=inhibit
                show_bank2=0;
        else
                show_bank2=1;

        if(video_latch & 0x08)// && (video_latch & 0x20)==0x00) //0= show green/altgreen bank, 1=inhibit
                show_bank3=0;
        else
                show_bank3=1;

        if((video_latch & 0x02)==0x02)  //0= motor off, 1=on
        {
                raw_motor=1;
        }
        else
        {
                raw_motor=0;
                stop_tape();
        }

        update_vid_maps96k();
        if (value & 0x40 && Line_Blank==0)
        {       
		        //Line Blanking monostable - freezes z80 till next scanline end
                Line_Blank=1;
                z80_stop_emulating();
        }       

}




void outp_96(UWORD port, UBYTE value)
{

/*      if((port & 0x207f)==0x207f)
                        bank_latch96k(value);
        else if((port & 0xff)==0x80)
                        vid_latch96k(value);*/


//FIXME - rev2x pcbs have this modded to xx7f thus releasing b register (also one of the SPLYNX mods)

//fixme so this should prob be 0x00ff

        if((port & 0x207f)==0x207f)port=0x7f;
//      if((port & 0xff7f)==0xff7f)port=0x7f;
     port = port & 0xFF;
        switch (port) {
                case 0x7F:
                        bank_latch96k(value);
                        break;
                case 0x80:
                        vid_latch96k(value);
                        break;
                default:
                        if(debug==1)
                                printf("Trying to out to port %02X (%02X)\n",port,value);
        }


}

Uint8 getbyte_96(Uint8 bank, Uint16 byte_offset, Uint8 byte)
{
        Uint8 red=0, blue=0, green=0,b1,b2,b3;
        Uint8 retbyte = 0;

		switch(bank)
		{
			case 0:
		        if(show_bank2==1)
				{
					if (byte_offset>0x1fff)
    					return  bank2[0x8000+byte_offset+byte];  //blue mirror
                    else
                        return  bank2[0xa000+byte_offset+byte];  //blue
 			    }
				else
            	    return 0;
				break;    	
			case 1:
		        if(show_bank2==1)
        		{
                	if (byte_offset>0x1fff)
                        return bank2[0xa000+byte_offset+byte];  //red mirror
                	else
                        return bank2[0xc000+byte_offset+byte]; //red
                }
			    else
            	    return 0;
				break;
			case 2:
		        if(show_bank3==1 )
        		{
                	if (byte_offset>0x1fff)
                	{
                        if(show_alt_green==1)
                                return bank3[0x8000+byte_offset+byte];   //alt green mirror
                        else
                                return bank3[0xa000+byte_offset+byte];  //green mirror
                	}
                	else
                	{
                        if(show_alt_green==1)
                                return bank3[0xa000+byte_offset+byte]; //alt green
                        else
                                return bank3[0xc000+byte_offset+byte];   //green
                	}
				}
		        else
        	        return 0;
				break;
		}
 //       blue= ((b1>> bit) & 0x01) ;
  //      red= ((b2>> bit) & 0x01)<<1 ;
   //     green= ((b3>> bit) & 0x01)<<2 ;
    //    colour = green + red + blue;
/*
        if(bank == 0)
            return b1;
        else if(bank == 1)
            return b2;
        else if(bank == 2)
            return b3;
*/                    

        return(0);
}

unsigned int getword_96(Uint8 bank, Uint16 byte_offset, Uint8 byte)
{
        unsigned int *bnk2;
 
		switch(bank)
		{
			case 0:
		        if(show_bank2==1)
				{
					if (byte_offset>0x1fff)
					{
						bnk2 = (unsigned int *)&bank2[0x8000+byte_offset+byte];//blue mirror
						return bnk2[0];
                    }
					else
					{
						bnk2 = (unsigned int *)&bank2[0xa000+byte_offset+byte];//blue
						return bnk2[0];
 			        }
				}
				else
            	    return 0;
				break;    	
			case 1:
		        if(show_bank2==1)
        		{
                	if (byte_offset>0x1fff)
					{
						bnk2 = (unsigned int *)&bank2[0xa000+byte_offset+byte];//red mirror
						return bnk2[0];
                	}
					else
					{
						bnk2 = (unsigned int *)&bank2[0xc000+byte_offset+byte];//red
						return bnk2[0];
					}
                }
			    else
            	    return 0;
				break;
			case 2:
		        if(show_bank3==1 )
        		{
                	if (byte_offset>0x1fff)
                	{
                        if(show_alt_green==1)
						{
							bnk2 = (unsigned int *)&bank3[0x8000+byte_offset+byte];//alt green mirror
							return bnk2[0];
                        }
						else
						{
							bnk2 = (unsigned int *)&bank3[0xa000+byte_offset+byte];//green mirror
							return bnk2[0];
						}
                	}
                	else
                	{
                        if(show_alt_green==1)
						{
							bnk2 = (unsigned int *)&bank3[0xa000+byte_offset+byte];//alt green 
							return bnk2[0];
                        }
						else
						{
							bnk2 = (unsigned int *)&bank3[0xc000+byte_offset+byte];//green 
							return bnk2[0];
						}
                	}
				}
		        else
        	        return 0;
				break;
		}
        return(0);
}


void update_keyrate96K()
{
                static  unsigned int new_keyval,looper=0;


//      return;
                if(emu_speed >2)        //stop keyrepeat for faster emus
                {
                                        new_keyval=0x0800*(emu_speed-1);
                                        bank1[0x6233]=new_keyval % 256;
                                        bank1[0x6234]=new_keyval / 256;
                }
                else
                {
                                        new_keyval=0x0800;
                                        bank1[0x6233]=new_keyval % 256;
                                        bank1[0x6234]=new_keyval / 256;
                }
}



UBYTE  Read_Mem96(UWORD A)
{
        //see if ROM is paged in or not
        if((bank_latch & 0x10)==0 )             //ROMS BANK 0
        {
                if (A<0x6000)
                        return(bank0[A]);
                if(bank0[0xe000]!=0xff) //&& (disk_rom_enabled==1))     // Disk rom is special - can be turned off
                {
                        if((A>=0xe000)   && ((disk_options&0x10)==0))
                        return(bank0[A]);
                }
        }       
        
        if ((bank_latch & 0x20)==0)     //USER RAM
        {
                        return(bank1[A]);
        }

        if ((bank_latch & 0x40)==0x40)  //vidram
        {
                if ((video_latch & 0x04)==0) 
                {
                        if(A>=0xa000 && A<0xe000)
                                return(bank2[A]);               //BLUE  RED
                        if(A>=0x8000 && A<0xa000)
                                return(bank2[A+0x2000]);        //mirror
                        if(A>=0x6000 && A<0x8000)
                                return(bank2[A+0x6000]);        //mirror
                        if(A>=0xe000)
                                return(bank2[A-0x2000]);        //mirror
                        if(A<0x2000)
                                return(bank2[A+0xa000]);        //mirror
                        if(A>=0x2000 && A<0x4000)
                                return(bank2[A+0x8000]);        //mirror
                        if(A>=0x4000 && A<0x6000)
                                return(bank2[A+0x8000]);        //mirror
                }
                if ((video_latch & 0x08)==0)
                {
                        if(A>=0xa000 && A<0xe000)
                                return(bank3[A]);//             if((video_latch & 0x20)==0x20 && (bank_latch & 0x02)==0x02)
                                                                                //AGREEN  GREEN
                        if(A>=0x8000 && A<0xa000)
                                return(bank3[A+0x2000]);        //mirror
                        if(A>=0x6000 && A<0x8000)
                                return(bank3[A+0x6000]);        //mirror
                        if(A>=0xe000)
                                return(bank3[A-0x2000]);        //mirror

                        if(A<0x2000)
                                return(bank3[A+0xa000]);        //mirror
                        if(A>=0x2000 && A<0x4000)
                                return(bank3[A+0x8000]);        //mirror
                        if(A>=0x4000 && A<0x6000)
                                return(bank3[A+0x8000]);        //mirror
                }
        }
        return(0xff);
}
