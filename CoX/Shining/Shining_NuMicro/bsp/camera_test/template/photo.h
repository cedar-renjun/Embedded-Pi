#ifndef _PHOTO_H_
#define _PHOTO_H_


extern unsigned char ucPhotoBuf[522];    //声明图像缓冲区,已经足够大了


void PhotoLenSet(unsigned char length1, unsigned char length2);
unsigned char PhotoZipRateSet(void);
unsigned char PhotoStartSend(void);
unsigned char PhotoLenRead(void);
unsigned long PhotoDataGet(unsigned char *ucBuf);
unsigned char PhotoResetSend(void);
unsigned char PhotoStopSend(void);
unsigned char PhotoSizeSend(void);
unsigned char PhotoInit(void);
unsigned long PhotoTake(unsigned char *buf);
unsigned char PhotoReset(void);
void conver_byte_asc(unsigned char *source, char *dest, unsigned long n);
unsigned char PhotoSavePower(void);



#endif
