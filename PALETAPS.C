#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PALESDL.H"
#include "PALETAPS.H"
#include "PALE_KEYS.H"
#include "PALESDL_IO.H"
#include "PALEDOS_GUI.H"

#include "PALEDOS_VID.H"

unsigned char lbuffer[LYNX_MAXMEM];//leave room for 'big' prog
 int file_id; //type offile loaded
unsigned int tape_mode=TAP_TAPES;
unsigned char taprom[30];


int load_lynx_tap(char fn[],char fn2[],int tape_type)
{
        FILE *lhandle;
        FILE *lhandle2;
        unsigned int  buf_p=0,quot=0,size_read,load_address;
        char lbl[100];
        unsigned char csum;
        unsigned int cdd,f,ret;
        unsigned int tap_leng,exec_address, buf_pale;
        char csum_ok[10];


        pump_string("mload\"\"\x0d");

        
        // open a file for input                  
        lhandle = fopen((const char *) fn, "rb" );
        if(lhandle!=NULL)
        {
                if( !(size_read = fread( lbuffer,1,0x10000,lhandle)) )
                {
                        return(0);
                }
                else
                {
                  
                  fclose( lhandle );
                  if( size_read == -1 )
                                        return(0);
                }
        }
        else
        {
                sprintf(lbl,"Couldn't Open TAP file %s",fn);
                gui_error(lbl);
                return(0);
        }

        if(tape_type==0) //standard tape with name
        {
                //Get filename - skip to second "
                while (quot<2)
                {
                        if(lbuffer[buf_p++]=='"')
                                quot++;
                }
                lbuffer[buf_p-1]='\0';
                lbuffer[0]=' ';         //wipe out first " ready for printing
        }

        //If next char is A5 we forgot to remove it from the TAP file !
        //we could have either 4d 'M' mc or 42 'B' basic, *** or 41 - 'A' level 9 data ***
        //If next char is 42 only then we have a basic proggy
        if(lbuffer[buf_p]==0xa5)
        {
                buf_p++;
                file_id=lbuffer[buf_p];
                buf_p++;
        }
        else
        {
                file_id=lbuffer[buf_p];
                buf_p++; //skip over the 42 ( or 4D) B or M (when no a5 there - everyone but me !)
        }

        //Get Length
        if(file_id==TAP_BASIC)
        {
                load_address=bank1[0x61fa]+256*bank1[0x61fb];//should be 694D
                tap_leng=lbuffer[buf_p]+256*lbuffer[buf_p+1];
                buf_p+=2;
        }
        else if(file_id==TAP_BINARY)
        {
                tap_leng=lbuffer[buf_p]+256*lbuffer[buf_p+1];
                load_address=lbuffer[buf_p+2]+256*lbuffer[buf_p+3];
                buf_p+=4;
        }else //DATA - swap dest and length
        {
                load_address=lbuffer[buf_p]+256*lbuffer[buf_p+1];
                tap_leng=lbuffer[buf_p+2]+256*lbuffer[buf_p+3];
                buf_p+=4;
        }

        buf_pale=load_address;
        //Get Prog
        csum=0;
        for(f=0;f<tap_leng;f++)
        {
                csum+=lbuffer[buf_p];//only used for binary MLOADed progs & Level 9 Data
                bank1[buf_pale++]=lbuffer[buf_p++];
        }

        //dec ptr to point to last byte of prog in memory
        buf_pale--;

        if(file_id==TAP_BASIC)
        {
                //MessageBox(NULL,"Updating 61fc","PALE Debug",MB_ICONINFORMATION);

                //Update end of program pointer in the BASIC os 
                bank1[0x61fc]=buf_pale % 256;
                bank1[0x61fd]=buf_pale / 256;
        }

        sprintf(csum_ok,"No Checksum");
        //skip over next two bytes if binary file - csum related
        if(file_id==TAP_BINARY || file_id==TAP_DATA)
        {
                sprintf(csum_ok,"Checksum OK");
                if(csum!=lbuffer[buf_p++])
                {
                        //MessageBox(NULL,"Bad Checksum Possibly for this File?","PALE Debug",MB_ICONINFORMATION);
                        sprintf(csum_ok,"Checksum BAD");
                }
                if(lbuffer[buf_p++]==0x4e)
                {  }
                        //                      MessageBox(NULL,"Got a funny N byte thing after the checksum!","PALE Debug",MB_ICONINFORMATION);
        }
        
        //Get Execution Addr
        if(file_id==TAP_BASIC || file_id==TAP_BINARY)
                exec_address=lbuffer[buf_p]+256*lbuffer[buf_p+1];
        else
                exec_address=0;

        if(file_id==TAP_BASIC || file_id==TAP_BINARY)
                sprintf((char*)fn2,"Name:%s \n ID:%02x \n Start %04x \n End %04x \n Length %04x \n Run %04x\n %s",lbuffer,file_id,load_address,buf_pale,tap_leng-1,exec_address,csum_ok);
        else  //Data
                sprintf((char*)fn2,"Name: n/a \n ID: %04x \n Start %04x \n End %04x \n Length %04x \n Run %04x",file_id,load_address,buf_pale,tap_leng-1);


/*
        if(breakin==1)
        {
                set_hl(buf_pale);
                set_de(0);
                set_pc(0xcfb);  //jump back into the ROM load routine - this one to the prompt (but MEM not set correctly)
        }

*/

        if(exec_address!=0 && file_id!=TAP_DATA)
        {
                if(file_id==TAP_BINARY)
                {
                        z80_set_reg(Z80_REG_HL,exec_address);//as the ROM does it
                        z80_set_reg(Z80_REG_PC,exec_address);  //jump to the invaders routine :))
                }
                else
                {
                        z80_set_reg(Z80_REG_HL,buf_pale);//end byte of program
                        z80_set_reg(Z80_REG_DE,exec_address);
                        z80_set_reg(Z80_REG_PC,0xcc1);  //jump back into the ROM load routine
                }
        }
        else

        {
                z80_set_reg(Z80_REG_HL,buf_pale);
                z80_set_reg(Z80_REG_DE,exec_address);
                z80_set_reg(Z80_REG_PC,0xcfb);  //jump back into the ROM load routine - this one to the prompt (but MEM not set correctly)
        }
        return(1);
}



int save_lynx_tap()
{
        unsigned int  buf_p=0,quot=0,size_read,load_address;
        unsigned char buffer[65536];//leave room for 'big' proggy ;)
        char lbl[100],plab[100],pnam[100];
        unsigned char csum;
        unsigned int cdd,f,ret,e1,e2,e3;
        unsigned int tap_leng,exec_address, buf_pale,end_address,start_address,prog_size;
        FILE*  handle2;
    int size_written,end_nam;
        //jump here from ROM save routine
        //get filename and any parameters
        //save header
        //save program between bounds
        //for(f=0;f<10;f++)
        //{
        //      plab[f]=pale_mem[e1+f];         
        //}
        //plab[10]='/0';




        e1=z80_get_reg(Z80_REG_DE);
        quot=0;buf_p=0;
        //Get filename - skip to second "
        while (quot<2)
        {
                plab[buf_p]=bank1[e1+buf_p];
                if(bank1[e1+buf_p++]=='"')
                        quot++;
        }
        plab[buf_p-1]='\0';
        plab[0]=' ';            //wipe out first " ready for save

        e2=strlen(plab);
        for(f=0;f<e2;f++)
                pnam[f]=plab[f+1];

        strcat(pnam,".TAP");    
        //sprintf(lbl,"DE points to %s, pnam is %s",plab,pnam);
        //MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);

    /* open a file for output             */
    handle2 = fopen((const char *) pnam,"wb");
    if( handle2 != NULL )
        {
                //write header - the filename
                buffer[0]='"';
                for(f=1;f<e2;f++)
                {
                                buffer[f]=plab[f];              
                }
                end_nam=f;
                buffer[end_nam]='"';
                buffer[end_nam+1]=0x42; //BASIC proggy designator
                start_address=bank1[0x61fa]+256*bank1[0x61fb];//should be 694D
                end_address=bank1[0x61fc]+256*bank1[0x61fd];
                //write tape length (end of program pointer-load address)
                tap_leng=end_address-start_address+1;
                buffer[end_nam+2]=tap_leng % 256;
                buffer[end_nam+3]=tap_leng / 256;
                //write Prog
                csum=0;
                for(f=0;f<tap_leng;f++)
                {
                        buffer[end_nam+4+f]=bank1[start_address+f];
                }
                //write Execution Addr
                //              exec_address=buffer[buf_p]+256*buffer[buf_p+1];
                e2=z80_get_reg(Z80_REG_IX);

        //      sprintf(lbl,"Hello Pete,IX is %4X",e2);
        //      MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
                                e1=bank1[0x61fc]+256*bank1[0x61fd];

                                
//WHY OH WHY DONT ASK ME WHY BUT THE LYNX
//LIKES TO HAVE 1600 bytes added to this address !
                                
//e3=1600+((0x10000+e2)-e1)%0xffff;             // rom does IX-(61fc/d)
//                              buffer[end_nam+4+f]=e3 % 256;
//                              buffer[end_nam+4+f+1]=e3 / 256;//these seem to retrurn control okay - should be IX ? based though
//                              buffer[end_nam+4+f+2]=e3 / 256;

                buffer[end_nam+4+f]=0;          //exec addr
                buffer[end_nam+4+f+1]=0;
                buffer[end_nam+4+f+2]=0;        //copy byte

                
                
                prog_size=tap_leng+end_nam+4+3+1;       
                // OKAY, actually write the file to disk
                if( !fwrite( buffer,prog_size,1,handle2))
                {
                        gui_error(":( Couldnt write the TAP file");
                }
                
                //sprintf(lbl,"Hello Pete,I Wrote %d bytes",size_written);
                //MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
                fclose( handle2 );
        }
        //return to ROM
        z80_set_reg(Z80_REG_PC,0xc59);  //  do a ret jump back into the ROM 
        return(1);
}


void set_t_mode(int ff)
{
        unsigned int sav_ad;

        tape_mode=ff;



        if (tape_mode==TAP_TAPES)       //TAP
        {
                if (hw_type==LYNX_HARDWARE_48 || hw_type==LYNX_HARDWARE_96)
                {       
                        //These are for the TAPE load/save routines
                        taprom[0]=bank0[0xd67];
                        bank0[0xd67]=0xed;      //change Tape Byte output, just return 0 in A ?
                        taprom[1]=bank0[0xd68];
                        bank0[0xd68]=0x00;
                        taprom[2]=bank0[0xd69];
                        bank0[0xd69]=0xc9;      

                        taprom[3]=bank0[0xb65];
                        bank0[0xb65]=0xc9;      //disabled completely - Read Tape Sync
                        
                //      bank0[0xcd4]=0xed;      //change Read Bit, just return 1 in A
                //      bank0[0xcd5]=0x01;
                //      bank0[0xcd6]=0xc9;
                        taprom[4]=bank0[0xcd4];
                        bank0[0xcd4]=0xc3;      //change Read Bit, just return 1 in A
                        taprom[5]=bank0[0xcd5];
                        bank0[0xcd5]=0xd4;
                        taprom[6]=bank0[0xcd6];
                        bank0[0xcd6]=0x0c;

                        taprom[7]=bank0[0xc95];
                        bank0[0xc95]=0xc3;      //set up an infint loop to wait here whilst VB is loading the RAM
                        taprom[8]=bank0[0xc96];
                        bank0[0xc96]=0x95;
                        taprom[9]=bank0[0xc97];
                        bank0[0xc97]=0x0c;

                        taprom[10]=bank0[0x3f62];
                        bank0[0x3f62]=0xc3;     //and again for MLOAD
                        taprom[11]=bank0[0x3f63];
                        bank0[0x3f63]=0x62;
                        taprom[12]=bank0[0x3f64];
                        bank0[0x3f64]=0x3f;

                        
                        //Patch Save routine to output OUT 93,x trapped here as SAVE
                        //jump back in at 0cfb

                        sav_ad=0xbcb;
                        taprom[13]=bank0[sav_ad+0];
                        bank0[sav_ad+0]=0x20;
                        taprom[14]=bank0[sav_ad+1];
                        bank0[sav_ad+1]=0xf4;
                        taprom[15]=bank0[sav_ad+2];
                        bank0[sav_ad+2]=0x01;//ld BC,0093
                        taprom[16]=bank0[sav_ad+3];
                        bank0[sav_ad+3]=0x93;
                        taprom[17]=bank0[sav_ad+4];
                        bank0[sav_ad+4]=0x00;
                        taprom[18]=bank0[sav_ad+5];
                        bank0[sav_ad+5]=0xed;//out a (c)
                        taprom[19]=bank0[sav_ad+6];
                        bank0[sav_ad+6]=0x79;
                        taprom[20]=bank0[sav_ad+7];
                        bank0[sav_ad+7]=0x00;//never gets to these :)
                        taprom[21]=bank0[sav_ad+8];
                        bank0[sav_ad+8]=0x00;
                }
else if (hw_type==LYNX_HARDWARE_128 || hw_type==LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
                {
                        //LYNX 128 STUFF

                        //These are for the TAPE load/save routines
                        taprom[0]=bank0[0xd3d];
                        bank0[0xd3d]=0xed;      //change Tape Byte output, just return 0 in A ?
                        taprom[1]=bank0[0xd3e];
                        bank0[0xd3e]=0x00;
                        taprom[2]=bank0[0xd3f];
                        bank0[0xd3f]=0xc9;      
                        taprom[3]=bank0[0xb65];
                        bank0[0xb65]=0xc9;      //disabled completely - Read Tape Sync

                        taprom[4]=bank0[0xd76];
                        bank0[0xd76]=0xed;      //change Read Bit, just return 1 in A
                        taprom[5]=bank0[0xd77];
                        bank0[0xd77]=0x01;
                        taprom[6]=bank0[0xd78];
                        bank0[0xd78]=0xc9;

                        //NOW disable LOAD & MLOAD completely
                //      bank0[0xc95]=0xc3;
                //      bank0[0xc96]=0x95;
                        //      bank0[0xc97]=0x0c;

                        taprom[7]=bank0[0xc92];
                        bank0[0xc92]=0xc3;
                        taprom[8]=bank0[0xc93];
                        bank0[0xc93]=0x92;
                        taprom[9]=bank0[0xc94];
                        bank0[0xc94]=0x0c;

                //      bank0[0x5f2a]=0xc3;
                //      bank0[0x5f2b]=0x62;
                //      bank0[0x5f2c]=0x3f;
                        taprom[10]=bank0[0x5f2a];
                        bank0[0x5f2a]=0xc3;
                        taprom[11]=bank0[0x5f2b];
                        bank0[0x5f2b]=0x2a;
                        taprom[12]=bank0[0x5f2c];
                        bank0[0x5f2c]=0x5f;


                        
                        //Patch Save routine to output OUT 93,x trapped here as SAVE
                        //jump back in at 0cfb
                        sav_ad=0xbcb;
                        taprom[13]=bank0[sav_ad+0];
                        bank0[sav_ad+0]=0x20;
                        taprom[14]=bank0[sav_ad+1];
                        bank0[sav_ad+1]=0xf4;
                        taprom[15]=bank0[sav_ad+2];
                        bank0[sav_ad+2]=0x01;//ld BC,0093
                        taprom[16]=bank0[sav_ad+3];
                        bank0[sav_ad+3]=0x93;
                        taprom[17]=bank0[sav_ad+4];
                        bank0[sav_ad+4]=0x00;
                        taprom[18]=bank0[sav_ad+5];
                        bank0[sav_ad+5]=0xed;//out a (c)
                        taprom[19]=bank0[sav_ad+6];
                        bank0[sav_ad+6]=0x79;
                        taprom[20]=bank0[sav_ad+7];
                        bank0[sav_ad+7]=0x00;//never gets to these :)
                        taprom[21]=bank0[sav_ad+8];
                        bank0[sav_ad+8]=0x00;

                        
                        //LYNX 128 Patches
                        //YEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEESS
                //      bank0[0x1f8e]=0xc9;     //disables setup of variables area (at moment VTBL is too low and overwrites jumpblocks)

                //      bank0[0x1f8f]=0x07;//usual Lynx 48K values
                //      bank0[0x1f90]=0x67;

                        //why these two bytes were wrong in the rom I dont know ?)
                //      bank0[0x1746]=0x07;  // becomes VTBL at 6210            NOT NEEDED for Russells ROM but here anyway for ROM1281 from Martyn
                //      bank0[0x1747]=0x67;             //6211
                }
        }
        else
        {
                //Dont do anything if its in RAW mode - should be perfect emulation
                //so restore the ROM to its original state
                if (hw_type==LYNX_HARDWARE_48 || hw_type==LYNX_HARDWARE_96)
                {       
                        //These are for the TAPE load/save routines
                        bank0[0xd67]=0xf5;//taprom[0];  //change Tape Byte output, just return 0 in A ?
                        bank0[0xd68]=0xe5;//taprom[1];
                        bank0[0xd69]=0xd5;//taprom[2];  

                        bank0[0xb65]=0xc5;      //disabled completely - Read Tape Sync
                        
                //      bank0[0xcd4]=0xed;      //change Read Bit, just return 1 in A
                //      bank0[0xcd5]=0x01;
                //      bank0[0xcd6]=0xc9;
                        bank0[0xcd4]=0x01;      //change Read Bit, just return 1 in A
                        bank0[0xcd5]=0x80;
                        bank0[0xcd6]=0x00;

                        bank0[0xc95]=0xcd;      //set up an infint loop to wait here whilst VB is loading the RAM
                        bank0[0xc96]=0x65;
                        bank0[0xc97]=0x0b;

                        bank0[0x3f62]=0x2a;     //and again for MLOAD
                        bank0[0x3f63]=0xee;
                        bank0[0x3f64]=0x61;

                        
                        //Patch Save routine to output OUT 93,x trapped here as SAVE
                        //jump back in at 0cfb

                        sav_ad=0xbcb;
                        bank0[sav_ad+0]=0xcd;
                        bank0[sav_ad+1]=0xb4;
                        bank0[sav_ad+2]=0x0b;//ld BC,0093
                        bank0[sav_ad+3]=0x13;
                        bank0[sav_ad+4]=0x1a;
                        bank0[sav_ad+5]=0x21;//out a (c)
                        bank0[sav_ad+6]=0x00;
                        bank0[sav_ad+7]=0x00;//never gets to these :)
                        bank0[sav_ad+8]=0xe5;
                }
else if (hw_type==LYNX_HARDWARE_128 || hw_type==LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
                {
                        //LYNX 128 STUFF

                        //These are for the TAPE load/save routines
                        bank0[0xd3d]=0xf5;      //change Tape Byte output, just return 0 in A ?
                        bank0[0xd3e]=0xe5;
                        bank0[0xd3f]=0xd5;      
                        
                        bank0[0xb65]=0xc5;      //disabled completely - Read Tape Sync

                        bank0[0xd76]=0x01;      //change Read Bit, just return 1 in A
                        bank0[0xd77]=0x80;
                        bank0[0xd78]=0x00;

                        //NOW disable LOAD & MLOAD completely
                //      bank0[0xc95]=0xc3;
                //      bank0[0xc96]=0x95;
                        //      bank0[0xc97]=0x0c;

                        bank0[0xc92]=0x2a;
                        bank0[0xc93]=0xfa;
                        bank0[0xc94]=0x61;

                //      bank0[0x5f2a]=0xc3;
                //      bank0[0x5f2b]=0x62;
                //      bank0[0x5f2c]=0x3f;
                        bank0[0x5f2a]=0x2a;
                        bank0[0x5f2b]=0xee;
                        bank0[0x5f2c]=0x61;
                        
                        //Patch Save routine to output OUT 93,x trapped here as SAVE
                        //jump back in at 0cfb
                        sav_ad=0xbcb;
                        bank0[sav_ad+0]=0xcd;
                        bank0[sav_ad+1]=0xb4;
                        bank0[sav_ad+2]=0x0b;//ld BC,0093
                        bank0[sav_ad+3]=0x13;
                        bank0[sav_ad+4]=0x1a;
                        bank0[sav_ad+5]=0xcd;//out a (c)
                        bank0[sav_ad+6]=0xa6;
                        bank0[sav_ad+7]=0x62;//never gets to these :)
                        bank0[sav_ad+8]=0xc3;
                }
        }
}


int load_level9_tap(char fn[], char fn2[])
{
        FILE *handle;
        FILE *handle2;
        int file_id;
        unsigned int  buf_p=0,quot=0,size_read,load_address;
        unsigned char buffer[65536];//leave room for 'big' proggy ;)
        char lbl[100];
        unsigned char csum;
        unsigned int cdd,f,ret;
        unsigned int tap_leng,exec_address, buf_pale;

        
        //Remove .TAP
        f=0;
        while(fn[f]!='.')
                f++;
        fn[f]='\0';

        strcpy(lbl,(char*)fn);
        strcat(lbl,"3.tap");
        
        //LOAD RED BANK
        handle = fopen( (const char *)lbl, "rb" );
        if( handle != NULL )
        {
            if( !fread( &buffer,0x3000,1,handle)  )
                {
//                              gui_error("Bad Length for 3rd Level9 File, remember to load the first two normally!");
//                              return(0);
                }
                fclose( handle );
        }
        else
        {
                gui_error("Couldn't open 3rd Level9 File, remember to load the first two normally!");
                return(0);
        }

        buf_p=0;
        //If next char is A5 we forgot to remove it from the TAP file !
        //we could have either 4d 'M' mc or 42 'B' basic, *** or 41 - 'A' level 9 data ***
        //If next char is 42 only then we have a basic proggy
        if(buffer[buf_p]==0xa5)
        {
                buf_p++;
                file_id=buffer[buf_p];
                buf_p++;
        }
        else
        {
                file_id=buffer[buf_p];
                buf_p++; //skip over the 42 ( or 4D) B or M (when no a5 there - everyone but me !)
        }
        load_address=buffer[buf_p]+256*buffer[buf_p+1];
        tap_leng=buffer[buf_p+2]+256*buffer[buf_p+3];
        buf_p+=4;
        //Get DATA
        for(f=0;f<tap_leng;f++)
        {
                bank2[0xc000+f]=buffer[buf_p++];
        }
        strcpy(lbl,(char*)fn);
        strcat(lbl,"4.tap");
        //LOAD BLUE BANK
        handle = fopen((const char *) lbl, "rb" );
        if( handle != NULL )
        {
          if( !fread( &buffer,0x3000,1,handle) )
                {
                //                              return(0);
                }
                fclose( handle );
        }
        else
        {
                gui_error("Couldn't open 4th Level9 File");
                return(0);
        }
        buf_p=0;
        if(buffer[buf_p]==0xa5)
        {
                buf_p++;
                file_id=buffer[buf_p];
                buf_p++;
        }
        else
        {
                file_id=buffer[buf_p];
                buf_p++; //skip over the 42 ( or 4D) B or M (when no a5 there - everyone but me !)
        }
        load_address=buffer[buf_p]+256*buffer[buf_p+1];
        tap_leng=buffer[buf_p+2]+256*buffer[buf_p+3];
        buf_p+=4;
        //Get DATA
        for(f=0;f<tap_leng;f++)
        {
                bank2[0xa000+f]=buffer[buf_p++];
        }
        strcpy(lbl,(char*)fn);
        strcat(lbl,"5.tap");
        //LOAD normal memory 7800 after advloader
        handle = fopen((const char *) lbl, "rb" );
        if( handle != NULL )
        {
          if( !fread( &buffer,0x3000,1,handle)  )
                {
//                      return(0);
                }
                 fclose( handle );
        }
        else
        {
                gui_error("Couldn't open 5th Level9 File");
                return(0);
        }
        buf_p=0;
        if(buffer[buf_p]==0xa5)
        {
                buf_p++;
                file_id=buffer[buf_p];
                buf_p++;
        }
        else
        {
                file_id=buffer[buf_p];
                buf_p++; //skip over the 42 ( or 4D) B or M (when no a5 there - everyone but me !)
        }
        load_address=buffer[buf_p]+256*buffer[buf_p+1];
        tap_leng=buffer[buf_p+2]+256*buffer[buf_p+3];
        buf_p+=4;
        //Get DATA
        for(f=0;f<tap_leng;f++)
        {
                bank1[load_address+f]=buffer[buf_p++];
        }
        z80_set_reg(Z80_REG_PC,0x7056); //re-enter level 9 loader
//      level9=1;//kludge
        return(1);
}

int load_LSF(char *fn)
{
        FILE *handle2;
        unsigned int Addr,Count;
        unsigned char Chr,c;
    int len, eof,f;
        char lbl[100];

   handle2 = fopen((const char *)fn,"rb");
    if( handle2 != NULL )
        {
                //Read in the registers
                z80_set_reg(Z80_REG_AF,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_BC,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_DE,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_HL,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IX,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IY,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_PC,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_SP,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_AF2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_BC2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_DE2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_HL2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IFF1,fgetc(handle2));
                z80_set_reg(Z80_REG_IFF2,fgetc(handle2));
                z80_set_reg(Z80_REG_IR,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IM,fgetc(handle2));
                z80_set_reg(Z80_REG_IRQVector,fgetc(handle2));
                z80_set_reg(Z80_REG_IRQLine,fgetc(handle2));

                video_latch=fgetc(handle2);
                bank_latch=fgetc(handle2);

                update_vid_bank_latches();


                for(f=0;f<18;f++)
                        CRTC_reg[f]=fgetc(handle2);

        eof=0;
 

                Addr=0;
        while(!eof && Addr<0x10000)
        {
                c=fgetc(handle2);
                if (c!=0xED) bank1[Addr++]=c;
                else
                {
                        len=fgetc(handle2);
                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(handle2);
                                while(len--) bank1[Addr++]=c;
                        }
                }
                if (feof(handle2))      eof=1;
        }
//sprintf(lbl,"Bank1 Addr=%d",Addr);
//gui_error(lbl);

                Addr=0;
        while(!eof && Addr<0x10000)
        {
                c=fgetc(handle2);
                if (c!=0xED) bank2[Addr++]=c;
                else
                {
                        len=fgetc(handle2);
                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(handle2);
                                while(len--) bank2[Addr++]=c;
                        }
                }
                if (feof(handle2)) eof=1;
        }

                Addr=0;
        while(!eof && Addr<0x10000)
        {
                c=fgetc(handle2);
                if (c!=0xED) bank3[Addr++]=c;
                else
                {
                        len=fgetc(handle2);
                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(handle2);
                                while(len--) bank3[Addr++]=c;
                        }
                }
                if (feof(handle2)) eof=1;
        }

                fclose(handle2);
        }
        return(1);
}


//LYNX SNAP FORMAT FILES
int save_LSF(char *fn)
{
        FILE *handle2;
        unsigned int Addr,Count,f;
        unsigned char Chr;
        char lbl[100];
    handle2 = fopen((const char *)fn,"wb");
    if( handle2 != NULL )
        {
                fputc(z80_get_reg(Z80_REG_AF)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_AF)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_BC)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_BC)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_DE)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_DE)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_HL)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_HL)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IX)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_IX)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IY)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_IY)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_PC)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_PC)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_SP)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_SP)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_AF2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_AF2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_BC2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_BC2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_DE2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_DE2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_HL2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_HL2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IFF1),handle2);
                fputc(z80_get_reg(Z80_REG_IFF2),handle2);
                fputc(z80_get_reg(Z80_REG_IR)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_IR)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IM),handle2);
                fputc(z80_get_reg(Z80_REG_IRQVector),handle2);
                fputc(z80_get_reg(Z80_REG_IRQLine),handle2);
                fputc(video_latch,handle2);
                fputc(bank_latch,handle2);

                for(f=0;f<18;f++)
                        fputc(CRTC_reg[f],handle2);

                 

            Addr=0x00;
            while(Addr<0x10000)
            {
                    Chr=bank1[Addr];
                    Count=1;
        
                    while((bank1[Addr+Count]==Chr) && ((Addr+Count)<=0x10000))
                            Count++;
        
                    if (Count>240) Count=240;
        
                    if ( Count>3 || Chr==0xed)
                    {
                            fputc(0xed,handle2);
                            fputc(Count,handle2);
                    }
                    else    Count=1;
        
                    fputc(Chr,handle2);
                    Addr+=Count;
            }
        //    fputc(0xed,handle2);
         //   fputc(0x00,handle2);

            Addr=0x00;
            while(Addr<0x10000)
            {
                    Chr=bank2[Addr];
                    Count=1;
        
                    while((bank2[Addr+Count]==Chr) && ((Addr+Count)<=0x10000))
                            Count++;
        
                    if (Count>240) Count=240;
        
                    if ( Count>3 || Chr==0xed)
                    {
                            fputc(0xed,handle2);
                            fputc(Count,handle2);
                    }
                    else    Count=1;
        
                    fputc(Chr,handle2);
                    Addr+=Count;
            }
          //  fputc(0xed,handle2);
          //  fputc(0x00,handle2);

            Addr=0x00;
            while(Addr<0x10000)
            {
                    Chr=bank3[Addr];
                    Count=1;
        
                    while((bank3[Addr+Count]==Chr) && ((Addr+Count)<=0x10000))
                            Count++;
        
                    if (Count>240) Count=240;
        
                    if ( Count>3 || Chr==0xed)
                    {
                            fputc(0xed,handle2);
                            fputc(Count,handle2);
                    }
                    else    Count=1;
        
                    fputc(Chr,handle2);
                    Addr+=Count;
            }
            fputc(0xed,handle2);
            fputc(0x00,handle2);
                fclose(handle2);
        }
        return(1);
}





