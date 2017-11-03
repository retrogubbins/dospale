#ifndef __RAZE_HELPER_H__
#define __RAZE_HELPER_H__

void get_statusregs(char*lbl);
void get_statusstack(char *lbl);
int disassemble (char *destin,unsigned int jogval,int bank);
void dpmi_status(char *lbl1);
void keycodes_status(char *lbl1);

#endif
