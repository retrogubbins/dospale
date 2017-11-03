#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "EMUSWITCH.h"

#include "PALESDL.H"

#include "PALE48K.H"
#include "PALE96K.H"
#include "PALE128K.H"

void zapmem()
{
        int Addr;
                Addr=0x8000;
            while(Addr<0x10000)
                {
           bank2[Addr]=0x0;
           Addr++;
            }

}

void initmem()
{
        switch(hw_type)
        {
                case LYNX_HARDWARE_48:
                        initmem48k();
                        break;
                case LYNX_HARDWARE_96:
                        initmem96k();
                        break;
                case LYNX_HARDWARE_128:
                        initmem128k();
                        break;
        }
}

void get_memscan(int b_no,int m_start,char mscan[])
{
        unsigned int f;
        m_start=m_start%65536;

        for(f=0;f<256;f++)
        {
                if(b_no==0)
                        mscan[f]=bank0[(m_start+f)%0xffff];
                else if(b_no==1)
                        mscan[f]=bank1[(m_start+f)%0xffff];
                else if(b_no==2)
                        mscan[f]=bank2[(m_start+f)%0xffff];
                else if(b_no==3)
                        mscan[f]=bank3[(m_start+f)%0xffff];
                else if(b_no==4)
                        mscan[f]=bank4[(m_start+f)%0xffff];
        }
}

void save_memdump()
{
        FILE *handle2;
        unsigned int Addr,Count,f;
        unsigned char Chr;

    handle2 = fopen("mem0.dmp","wb");
    if( handle2 != NULL )
        {
            Addr=0x00;
            while(Addr<0x10000)
            {
            Chr=bank0[Addr];
            fputc(Chr,handle2);
            Addr++;
            }
                fclose(handle2);
        }

    handle2 = fopen("mem1.dmp","wb");
    if( handle2 != NULL )
        {
            Addr=0x00;
            while(Addr<0x10000)
            {
            Chr=bank1[Addr];
            fputc(Chr,handle2);
            Addr++;
            }
                fclose(handle2);
        }
    handle2 = fopen("mem2.dmp","wb");
    if( handle2 != NULL )
        {
            Addr=0x00;
            while(Addr<0x10000)
            {
            Chr=bank2[Addr];
            fputc(Chr,handle2);
            Addr++;
            }
                fclose(handle2);
        }
    handle2 = fopen("mem3.dmp","wb");
    if( handle2 != NULL )
        {
            Addr=0x00;
            while(Addr<0x10000)
            {
            Chr=bank3[Addr];
            fputc(Chr,handle2);
            Addr++;
            }
                fclose(handle2);
        }
}
