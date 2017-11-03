#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#include "PALESDL.H"
#include "PALEDOS_GUI.H"
#include "PALEDOS_VID.H"

#include "PALEDOS_TEMP.H"

void saveconfigfile()
{
        FILE *otpf;
                char lbl[50];
                otpf=fopen("PALECONF.CFG","wt");
        if(otpf==NULL)
        {
                sprintf(lbl,"Cannot open PALE Configuration File for writing");
                                gui_error(lbl);
                return;
        }
        fprintf(otpf,"===================================================================\n");
        fprintf(otpf,"PALESDL configuration file\n");
        fprintf(otpf,"===================================================================\n");
        fprintf(otpf,"emu_speed= %6d\t\t#emulator speed\n",emu_speed);
        fprintf(otpf,"show_memmap= %6d\n",show_memmap);
        fprintf(otpf,"show_status= %6d\n",show_status);
        fprintf(otpf,"show_sysvars= %6d\n",show_sysvars);
        fprintf(otpf,"memmap_bankno= %6d\n",memmap_bankno);
        fprintf(otpf,"emu_display= %6d\n",emu_display);
        fprintf(otpf,"display_w= %6d\n",get_display_w());
        fprintf(otpf,"display_h= %6d\n",get_display_h());
        fprintf(otpf,"SoundEnable= %6d\n",SoundEnable);
        fprintf(otpf,"mc_type= %6d\t\t#machine type\n",mc_type); //This MUST come at the end of the file
        fclose(otpf);
}

int loadconfigfile()
{
        int tempi, tempi2;
                static int ftimes=1;
                static int rgain=1;
                int currline=0,vid_w_bodg;
        unsigned long tempiul;
                float tempf;
        char *lval,label[200];
        char bytes[3];
        FILE *inpf;
                char lbl[60];

        inpf=fopen("PALECONF.CFG","rt");
        if(inpf!=NULL)     //Check to see if we can open requested file
        {
                while(fgets(label,200,inpf)!=NULL)
                {
                        lval=strstr(label,"emu_speed=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+10,"%d",&tempi);
                                set_speed(tempi);
                        }
                        lval=strstr(label,"memmap_bankno=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+14,"%d",&tempi);
                                set_memmap_bankno(tempi);
                        }
                        lval=strstr(label,"show_memmap=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+12,"%d",&tempi);
                                set_show_memmap(tempi);
                        }
                        lval=strstr(label,"show_status=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+12,"%d",&tempi);
                                set_show_status(tempi);
                        }
                        lval=strstr(label,"show_sysvars=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+13,"%d",&tempi);
                                set_show_sysvars(tempi);
                        }
                        lval=strstr(label,"emu_display=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+12,"%d",&tempi);
                                set_display(tempi);
                        }
                        lval=strstr(label,"display_w=");        //THESE two dont work yet
                        if(lval!=NULL)
                        {
                                sscanf(lval+10,"%d",&tempi);
                                vid_w_bodg=tempi;       //Naughtiness
                        }
                        lval=strstr(label,"display_h=");        //these two dont work yet
                        if(lval!=NULL)
                        {
                                sscanf(lval+10,"%d",&tempi);
//                                video_resize(vid_w_bodg,tempi,1);
                        }
                        lval=strstr(label,"SoundEnable=");      
                        if(lval!=NULL)
                        {
                                sscanf(lval+12,"%d",&tempi);
                                set_SoundEnable(tempi);
                        }
                        lval=strstr(label,"mc_type=");
                        if(lval!=NULL)
                        {
                                sscanf(lval+8,"%d",&tempi);
                                set_machine(tempi);
                        }
                }
                        fclose(inpf);
        }
        else
        {
                sprintf(lbl,"could not open COnfig for reading");
                                gui_error(lbl);
                return(0);
        }

                return(1);
}

