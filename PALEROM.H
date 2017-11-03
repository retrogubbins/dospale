#ifndef __PALEROM_H__
#define __PALEROM_H__





int build_rom(char r0[],char r1[],char r2[],
								 char r3[],char r4[], char r5[],
								 char r6[], char r7[], char ropth[]);
int load_lynx_rom();
void patch_lynx_rom();

void create_dummy_rom();
void saveROMconfigfile();
int loadROMconfigfile();
void init_ROMsets();


void load_romset(int f);

extern char romset_files[8*8][40];
extern unsigned char romset_hwtypes[8];



extern unsigned char rompath[200];

#endif
