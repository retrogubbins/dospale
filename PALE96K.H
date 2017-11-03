#ifndef __PALE96K_H__
#define __PALE96K_H__

void initmem96k();
void update_vid_maps96k();
void outp_96(UWORD port, UBYTE value);

Uint8 getpixel_96(Uint16 offset, Uint8 bit);
Uint8 getbyte_96(Uint8 bank, Uint16 byte_offset, Uint8 byte);
unsigned int getword_96(Uint8 bank, Uint16 byte_offset, Uint8 byte);

void update_keyrate96K();
void bank_latch96k(unsigned char value);
void vid_latch96k(unsigned char value);



#endif
