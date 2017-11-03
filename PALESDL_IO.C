#include "EMUSWITCH.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PALESDL.H"
#include "PALE_VID.H"
#include "PALEDOS_GUI.H"

#include "PALEDISK.H"
#include "PALETAPS.H"
#include "PALESND.H"
#include "PALESDL_IO.H"
#include "PALE48K.H"
#include "PALE96K.H"
#include "PALE128K.H"
#include "PALERAWTAPE.H"



unsigned char sound_port=0;
UBYTE video_latch,bank_latch;


//show_bankX are used by 48/96 modes
//show_allbanks used by 128 mode
//Line_Blank, show_alt_green and speaker_enable are common ports
Uint8 Line_Blank=0;
Uint8 show_bank1,show_bank2,show_bank3,show_allbanks,show_alt_green,speaker_enable;

void update_vid_bank_latches()
{
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                                bank_latch48k(bank_latch);
                                vid_latch48k(video_latch);
                        break;
                case LYNX_HARDWARE_96:
                                bank_latch96k(bank_latch);
                                vid_latch96k(video_latch);
                        break;
                case LYNX_HARDWARE_128:
                                bank_latch128k(bank_latch);
                                vid_latch128k(video_latch);
                        break;
        }
}

void clearports()
{
        unsigned int f;
        //SET ALL PORTS TO Default values
        for(f=0;f<LYNX_MAXMEM;f++)
        {
                z80ports_in[f]=0xff;
                //IMPORTANT IN PORTS
                //FOR 128
                //0x84  single step port ?
                z80ports_out[f]=0x00;
        }
        //set up the main bank ports
        video_latch=0x00;
        bank_latch=0x00;
        disk_options=1;
        disk_rom_enabled=1;
}


void lynx_outp(UWORD port, UBYTE value)
{
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        outp_48(port, value);
                        break;
                case LYNX_HARDWARE_96:
                        outp_96( port, value);
                        break;
                case LYNX_HARDWARE_128:
                        outp_128( port, value);
                        break;
        }
        switch(port&0xFF)
        {
                //COMMON OUTPUT PORTS
                case 0x80:                              //common Vid latch property - speaker enable
                        speaker_enable=value&0x01;
        //              update_sound();
                        break;
                case 0x84:
                        sound_port=value;
                        if(SoundEnable)update_sound();
                        if((raw_motor==1) && (tape_operation==TAPE_STOPPED))start_tape_record();
                        if(tape_operation==TAPE_RECORD)update_tape();
                        break;
                case 0x54:
                case 0x55:
                case 0x56:
                case 0x57:
                case 0x58:
                        disk_outp(port,value);//Disk ports
                        break;
                case 0x86:
                        crtc_reg=value;
                        break;
                case 0x87:
                        CRTC_reg[crtc_reg]=value;
                        break;
                case 0x93:
                        save_lynx_tap();                //TRAP for SAVE
                        break;
        }
//      disk_outp(port,value);//Disk options

}

UBYTE lynx_inp(UWORD port)
{
        unsigned char tmp;
        unsigned int tmp2;
        char lbl[200],lbl2[200];


        if((port&0x2000)!=0x2000)
                port=port&0xFFF;

//      if(  (((port & 0x0ff)==0x080) && (raw_motor==1)) || tape_override==FORCE_PLAY ||  tape_override==FORCE_RECORD)
        if(  (((port & 0x0ff)==0x080) && (raw_motor==1) && (speaker_enable==0)) )
        {
                        if(tape_operation==TAPE_STOPPED)
                                start_tape_play();
                        update_tape();
                        update_sound();
                        if(tape_level>raw_threshold)  //80)For Zen (Tape 3)  //70)for Colossal adventure
                                bit(&z80ports_in[0x80],0,0);
                        else
                                bit(&z80ports_in[0x80],0,1);
        }

        //This gets done a lot so freeload on here in any Input request
        if(tape_override==FORCE_PLAY ||  tape_override==FORCE_RECORD)
        {
                        update_tape();
                        update_sound();
        }


        // DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT   
        // DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT   
        // DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT   
        // DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT   
        // DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT   
        // DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT    DISK INPUT   
        if((port & 0xff)==0x50)
        {
                //              if(disk_trace_on==1)
                //              {
                //                      sprintf(lbl2," STATUS READ Value = %2X\n",disk_statusreg);
                //                      fprintf(stream_disk_trace,lbl2);
                //              }
                return(disk_statusreg);//noticce that dual purpose here
        }
        if((port & 0xff)==0x51)
                return(disk_trackreg[disk_drive]);
        if((port & 0xff)==0x52)
                return(disk_sectreg);
        if((port & 0xff)==0x53)
        {
                if((disk_comreg & 0xe0)==0x80)  //if were in a read sector command
                {
                        tmp=disk_sect_buf[disk_sect_buf_ptr++];
                        if(disk_trace_on==1)
                        {
                                sprintf(lbl2," *** DATA Read (Sector) - Drive %d Head %d Track %d ,Sector %d, SECT BUFPTR %d  Value = %2X\n",disk_drive,disk_head,disk_trackreg[disk_drive],disk_sectreg,disk_sect_buf_ptr-1,tmp);
                               // fprintf(stream_disk_trace,lbl2);
                        }

                        if(disk_sect_buf_ptr>=BYTES_PER_SECT)
                        {
                                disk_statusreg=0x00;
                                disk_sect_buf_ptr=0;
                        }
                        return(tmp);
                }
                else if((disk_comreg & 0xf8)==0xc0)//Read Address
                {
                        tmp=disk_sect_buf[disk_sect_buf_ptr++];

                        if(disk_sect_buf_ptr>=8)
                        {
                                disk_statusreg=0x00;
                                disk_sect_buf_ptr=0;
                        }                       
                        return(tmp);
                }
                sprintf(lbl2,"some other read of 53 port is %2X",port);
                //MessageBox(NULL,lbl2,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
                return(disk_datareg);
        }





        return(z80ports_in[port]);
}

