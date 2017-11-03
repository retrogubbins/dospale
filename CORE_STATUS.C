#include <stdio.h>
#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

#include "EMUSWITCH.h"


#include "PALESDL.H"
#include "Z80DASM.H"
#include "PALESDL_IO.H"
#include "PALE_KEYS.H"


struct meminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} MemInfo;

#define DPMI_INT        0x31

void dpmi_status(char *lbl1)
{
        char lbl2[200];
    union REGS regs;
    struct SREGS sregs;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof(sregs) );
    sregs.es = FP_SEG( &MemInfo );
    regs.x.edi = FP_OFF( &MemInfo );
        
        sprintf(lbl1,"DPMI Status:");

    int386x( DPMI_INT, &regs, &regs, &sregs );
    sprintf(lbl2, "Lrgst Avail blk:\n\t \f2%lu\f5\n",
            MemInfo.LargestBlockAvail );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Max ulkd page alloc:\n\t \f2%lu\f5\n",
            MemInfo.MaxUnlockedPage );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Lgst Lockable Page:\n\t \f2%lu\f5\n",
            MemInfo.LargestLockablePage );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Lin addr space inc alloc pgs:\n\t \f2%lu\f5\n", MemInfo.LinAddrSpace );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Free pages available:\n\t \f2%lu\f5\n",
             MemInfo.NumFreePagesAvail );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Pages not in use:\n\t \f2%lu\f5\n",
             MemInfo.NumPhysicalPagesFree );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Pages managed by host:\n\t \f2%lu\f5\n",
             MemInfo.TotalPhysicalPages );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Free lin addr space(pgs):\n\t \f2%lu\f5\n",
             MemInfo.FreeLinAddrSpace );
        strcat(lbl1,lbl2);
    sprintf(lbl2, "Paging/file partition (pgs):\n\t \f2%lu\f5\n",
             MemInfo.SizeOfPageFile );
        strcat(lbl1,lbl2);

}



void keycodes_status(char *lbl1)
{
        unsigned int k;
        char lbl2[200];
        
        sprintf(lbl1,"Keycodes Status:\n");
        for(k = 0 ; k <120; k++)
        {
                if(key[k])
                {
                    sprintf(lbl2, "\n\t \f2%lu\f5\n",k);
                        strcat(lbl1,lbl2);
                }
        }
}

void get_statusregs(char* lbl)
{
        sprintf(lbl,
      //          "PC:%04X IR: %02X AF:%04X HL:%04X\nDE:%04X BC:%04X SP:%04X IX:%04X\nIY:%04X",
                "PC:%04X AF:%04X HL:%04X SP:%04X\nDE:%04X BC:%04X\nIX:%04X IY:%04X  IR: %02X\n\f6Bank:%02X Video:%02X\f7",
            z80_get_reg(Z80_REG_PC),
                z80_get_reg(Z80_REG_IR),
                z80_get_reg(Z80_REG_AF),
                z80_get_reg(Z80_REG_HL),
                z80_get_reg(Z80_REG_DE),
                z80_get_reg(Z80_REG_BC),
                z80_get_reg(Z80_REG_SP),
                z80_get_reg(Z80_REG_IX),
                z80_get_reg(Z80_REG_IY),
                                bank_latch,
                                video_latch);
}

void get_statusstack(char *lbl)
{
        unsigned int i,sp;
        char lbl1[300],lbl2[300],lbl3[300],lbl4[300];
        //STACK
        sp=z80_get_reg(Z80_REG_SP);
        
        sprintf(lbl2," ");
        for (i=0;i<5;++i)
        {
                sprintf(lbl3,"%02X%02X ",bank1[sp+i*2],bank1[sp+i*2]);
                strcat(lbl2,lbl3);
        }
        strcpy(lbl,lbl2);
}



int disassemble (char *destin,unsigned int jogval,int bank)
{
        int g,retr,f,start,mid,end,linecount;
        unsigned int curp,peecee;
        char lbl[200];
        char dis_buf[4000];
        char return_char[4];


        //Poss might have to increase this
        // to avoid routine below scanning backwards into other vars space
        peecee=z80_get_reg(Z80_REG_PC);
        curp=peecee-8+jogval;
        
        //skip forward to the relevant byte
        sprintf(return_char,"\n");
        sprintf(dis_buf,"\0");
        mid=0;
        for (f=0;f<12;f++)
        {
                if(curp==peecee)
                {
                        mid=strlen(dis_buf);
                        sprintf(lbl,"->");
                        strcat(dis_buf,lbl);
                }
                sprintf(lbl,"%4X - ",curp);
                strcat(dis_buf,lbl);
//              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                if(bank==0)
                                retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                else if(bank==1)
                                retr=Z80_Dasm((unsigned char *)bank1+curp,lbl,curp);
                else if(bank==2)
                                retr=Z80_Dasm((unsigned char *)bank2+curp,lbl,curp);
                else if(bank==3)
                                retr=Z80_Dasm((unsigned char *)bank3+curp,lbl,curp);
                else if(bank==4)
                                retr=Z80_Dasm((unsigned char *)bank4+curp,lbl,curp);
                //              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
//              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                if(retr !=0)
                {
                    strcat(dis_buf,lbl);
                    strcat(dis_buf,return_char);
                    curp+=retr;//advance to the next opcode
                }
                else
                    curp++; //advance to the next opcode
        }
        end=strlen(dis_buf);

        if (mid!=0)
        {
                //okay, we've got a listing, now search thru
                //to fnd the PC line, and then take the 14 lines either side of it
                //and copy them to the output buffer
                start=0;
                f=0;    
                //Skip to PC line
//              while(dis_buf[f++]!='_')
//              {
//                      if(f>end)return(0);
//              };
                //Now go back 6 lines
                f=mid;
                for(g=0;g<4;g++)
                {
                        while(dis_buf[f--]!='\n')
                        {
                                if(f<=0)return(0);
                        }
                }
                start=f+2;
                for(f=start;f<end;f++)
                        destin[f-start]=dis_buf[f];
        }
                else return(0);

//if(end > 150)
  //  end = 150;
                for(f=0;f<end;f++)
                        destin[f]=dis_buf[f];
                destin[f]='\0';
        return(1);
}

