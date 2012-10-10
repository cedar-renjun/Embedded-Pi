#include "coshining.h"
#include "xgpio.h"
#include "xuart.h"
#include "xsysctl.h"
#include "photo.h"
#include "xcore.h"
#include "xhw_ints.h"
//#include "sdcard.h"

unsigned long ulPhotoLen = 0;   //图像的长度
unsigned long ulPhotoCount = 0;   //图像的长度
unsigned char ucPhotoBuf[522];    //声明图像缓冲区,已经足够大了

//把下面的命令(除了ucGetPhotoCmd命令)声明为const类型,就可以存储在rom,节省ram

//复位命令
unsigned char const ucResetCmd[] = 
{
	0x56,
	0x00,
	0x26,
	0x00
};

//复位返回命令
unsigned char const ucResetRcv [] =
{ 
	0x76,
	0x00,
	0x26,
	0x00
};

//开始拍照命令
unsigned char const ucStartPhotoCmd [] =
{
	0x56,
	0x00,
	0x36,
	0x01,
	0x00
};

//开始拍照返回命令
unsigned char const ucStartPhotoRcv [] =
{
	0x76,
	0x00,
	0x36,
	0x00,
	0x00
};

//读长度命令
unsigned char const ucReadLenCmd [] = 
{
	0x56,
	0x00,
	0x34,
	0x01,
	0x00
};
//读长度返回命令的前七个字节,返回命令的后两个字节为图像长度
unsigned char const ucReadLenRcv [] =
{
	0x76,
	0x00,
	0x34,
	0x00,
	0x04,
	0x00,
	0x00
};

//取图像命令,也就是读数命令
unsigned char ucGetPhotoCmd [] =
{
	0x56,
	0x00,
	0x32, 
	0x0C, 
	0x00, 
	0x0A,
	 
	0x00, 
	0x00, 
	0x00, 
	0x00,
	
	0x00, 
	0x00,
	0x00, 
	0x00,
	
	0x00,
	0x0A              //这个时间间隔要注意,应该设大一点,而不是设小一点,设小一点的话数据来得太快就容易出现接收错误

};
unsigned char const ucGetPhotoRcv [] =
{
	0x76,
	0x00,
	0x32,
	0x00,
	0x00
};


//设置压缩率命令,这里设为0x80 : 1,尽量使图像小
unsigned char const ucSetZipRateCmd [] =
{
	0x56,
	0x00, 
	0x31, 
	0x05,  
	0x01, 
	0x01, 
	0x12,
	0x04,  
	0x36
};

unsigned char const ucSetZipRateRcv [] = 
{
	0x76,
	0x00,
	0x31,
	0x00,
	0x00
};

//停止拍照命令,它的返回命令与开始拍照的相同
unsigned char const ucStopPhotoCmd [] =
{
	0x56,
	0x00,
	0x36,
	0x01,
	0x03
};
//停止拍照返回命令,跟开始拍照的一样
unsigned char const ucStopPhotoRcv [] =
{
	0x76,
	0x00,
	0x36,
	0x00,
	0x00
};

//设置图像大小命令
unsigned char const ucPhotoSizeCmd [] =
{
	0x56,
	0x00, 
	0x31, 
	0x05,  
	0x04, 
	0x01, 
	0x00, 
	0x19,
	0x11,
	0x00
};
unsigned char const ucPhotoSizeRcv [] =
{
	0x76, 
	0x00, 
	0x31, 
	0x00, 
	0x00
};
unsigned char const ucSavePowerCmd [] =
{
	0x56, 
	0x00, 
	0x3E, 
	0x03, 
	0x00, 
	0x01, 
	0x00
};
unsigned char const ucSavePowerRcv [] =
{
	0x76, 
	0x00, 
	0x3E, 
	0x00, 
	0x00
};
unsigned char const ucSetBaudRateCmd [] = 
{
	0x56,
	0x00, 
	0x24, 
	0x03, 
	0x01, 
	0x0D,
	0xA6	
};
unsigned char const ucSetBaudRateRcv [] = 
{
	0x76, 
	0x00, 
	0x24, 
	0x00, 
	0x00
};

unsigned char const ucFlowerCmd [] = 
{
	0x56,
	0x08, 
	0x29, 
	0x09, 
	0x00	
};


//*****************************************************************************
//
//! \brief Send a buffer to uart.
//!
//! \param ulBase is the base address of the UART port.
//! \param pucStr is the point of data will be sent.
//! \param ulLen is the lenth of data will be sent.
//!
//! Send a buffer to uart
//!
//! \return None
//
//*****************************************************************************
void
UARTBufferPut(unsigned long ulBase, unsigned char *pucStr, unsigned long ulLen)
{
    unsigned long i;
	  for(i=0; i<ulLen; i++)
    {
        xUARTCharPut(ulBase, *pucStr++);
    }	
}


//*****************************************************************************
//
//! \brief get data from UART.
//!
//! \param pucBuf is the point of data will be save.
//! \param ulLen is the lenth of data will be save.
//!
//! get data from UART.
//!
//! \return the lenth of data
//
//*****************************************************************************
unsigned long 
UARTBufferGet(unsigned long ulBase, unsigned char *pucBuf, unsigned long  ulLen)
{
	unsigned long i = 0;
	for (i = 0; i < ulLen; i++)
	{
		pucBuf[i] = xUARTCharGet(ulBase);
	}	
	return ulLen;
}


//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//发送设置图像大小命令,此函数可以不要
unsigned char 
PhotoSizeSend(void)
{
	  unsigned long i;//, count;
	  unsigned char tmp[5];
	
	  UARTBufferPut(sUART_BASE, (unsigned char *)&ucPhotoSizeCmd[0], 10);
	
	  if (!UARTBufferGet(sUART_BASE, tmp, 5)) return 0;
	
	//检验数据
	for (i = 0; i < 5; i++)
	{
		if (tmp[i] != ucPhotoSizeRcv[i]) return 0;
	}

	return 1;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//发送停止拍照函数
unsigned char PhotoStopSend(void)
{
	unsigned long i;//, count;
	unsigned char tmp[5];
	
	
	//发送停止拍照命令,其实就是清空缓冲区,而不是什么停止拍照
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucStopPhotoCmd[0], 5);
	
	if (!UARTBufferGet(sUART_BASE,tmp, 5)) return 0;
	
	//检验数据
	for (i = 0; i < 5; i++)
	{
		if (tmp[i] != ucStopPhotoRcv[i]) return 0;
	}
	
//	xSysCtlDelay(500);  //延时一会,等摄像头擦写完缓冲区
	
	return 1;
}


//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//发送复位命令函数
unsigned char PhotoResetSend(void)
{
	unsigned char tmp[4];
	unsigned long i;//, count;
	
	//while(1)
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucResetCmd[0], 4);

	if (!UARTBufferGet(sUART_BASE,tmp, 4)) 
		 return 0;
	
	for (i = 0; i < 4; i++)
		if (tmp[i] != ucResetRcv[i]) return 0;
	
	xSysCtlDelay(3000000);  //复位后延时,因为只检验了帧头,后面的数据要丢弃
	//清空一下溢出标志
	while(xUARTCharGetNonBlocking(sUART_BASE) != -1);
	while(!UARTFIFORxIsEmpty(sUART_BASE));
	
	
	return 1;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//读图像数据函数，返回图像数据的大小(以字节为单位),若失败则返回0
unsigned long PhotoDataGet(unsigned char *ucBuf) 
{
	unsigned long i;//, count;
	unsigned long ulLeft;
	
	ulPhotoCount = 0;
	
	UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucGetPhotoRcv[0], 5);
	
	while(ulPhotoCount < ulPhotoLen)
	{
		ulLeft = ulPhotoLen - ulPhotoCount;
    ucGetPhotoCmd[8] =ulPhotoCount/256;
		ucGetPhotoCmd[9] =ulPhotoCount%256;
		if(ulLeft <= 512)
		{
			ucGetPhotoCmd[12] =ulLeft/256;
		  ucGetPhotoCmd[13] =ulLeft%256;
			ulPhotoCount = ulPhotoCount + ulLeft;
			//发送读数命令
			do
			{
	    UARTBufferPut(sUART_BASE, (unsigned char *)&ucGetPhotoCmd[0], 16);
		  UARTBufferGet(sUART_BASE, ucBuf, ulLeft+10);
			}
			while(!(ucBuf[ulLeft+3] == 0xFF &&
				      ucBuf[ulLeft+4] == 0xD9)
				   );
			UARTBufferPut(sUART_DBG_BASE, &ucBuf[5], ulLeft);
			UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucGetPhotoRcv[0], 5);
			//UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucStopPhotoRcv[0], 5);
			//PhotoTake(&ucPhotoBuf[0]);
    }
		else
		{

			ucGetPhotoCmd[12] =2;
		  ucGetPhotoCmd[13] =0;
			ulPhotoCount = ulPhotoCount + 512;
			//发送读数命令
	    UARTBufferPut(sUART_BASE, (unsigned char *)&ucGetPhotoCmd[0], 16);
		  UARTBufferGet(sUART_BASE, ucBuf, 522);
			UARTBufferPut(sUART_DBG_BASE, &ucBuf[5], 512);
			
    }

  }
	ulPhotoCount = 0;
	
	/*//发送读数命令
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucGetPhotoCmd[0], 16);
	
	if (!UARTBufferGet(sUART_BASE,ucBuf, 10 + ulPhotoLen)) return 0;
	
	//检验帧头
	for (i = 0; i < 5; i++)
	{
		if (ucBuf[i] != ucGetPhotoRcv[i]) return 0;
	}
	
	//检验帧尾
	for (i = 0; i < 5; i++)
	{
		if (ucBuf[i + 5 + ulPhotoLen] != ucGetPhotoRcv[i]) return 0;
	}
	
	//检验图像的合法性,以0xFF,0xD8开始,0xFF,0xD9结束
	if (ucBuf[5] != 0xFF || ucBuf[6] != 0xD8 ||
		ucBuf[3 + ulPhotoLen] != 0xFF || ucBuf[4 + ulPhotoLen] != 0xD9)
	{
		return 0;
	}
	
	//把图像数据移动到数组0开始处
	for (i = 0; i < ulPhotoLen; i++)
		ucBuf[i] = ucBuf[i + 5];*/
		
	
	return ulPhotoLen;	//返回图像长度
}


//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//读长度函数
unsigned char PhotoLenRead(void)
{
	unsigned long i;//, count;
	unsigned char tmp[9];
	
	while(UARTCharGetNonBlocking(sUART_BASE) != -1);
	
	//发送读长度命令
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucReadLenCmd[0], 5);
	
	if (!UARTBufferGet(sUART_BASE,tmp, 9)) return 0;
	
	//检验数据
	for (i = 0; i < 7; i++)
	{
		if (tmp[i] != ucReadLenRcv[i]) return 0;
	}
	
	//PhotoLenSet(tmp[7], tmp[8]);  //设置读长度命令
	ulPhotoLen = tmp[7] * 0x100 + tmp[8];
	PhotoLenSet(0x2, 0);
	
	return 1;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
void PhotoLenSet(unsigned char length1, unsigned char length2)
{
//	setlocale(LC_ALL, "C");
	ucGetPhotoCmd[12] = length1;
	ucGetPhotoCmd[13] = length2;
	//ulPhotoLen = length1 * 0x100 + length2;
}


//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//发送压缩率函数
unsigned char PhotoZipRateSet(void)
{
	unsigned long i;//, count;
	unsigned char tmp[5];
	
	//设置压缩率为80:1
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucSetZipRateCmd[0], 9);

	if (!UARTBufferGet(sUART_BASE,tmp, 5)) return 0;
	
	//检验数据
	for (i = 0; i < 5; i++)
	{
		if (tmp[i] != ucSetZipRateRcv[i]) return 0;
	}
	
	return 1;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
unsigned char PhotoStartSend(void)
{	
	unsigned long i;//, count;
	unsigned char tmp[5];
	
	//向摄像头发送拍照命令
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucStartPhotoCmd[0], 5);

	if (!UARTBufferGet(sUART_BASE,tmp, 5)) return 0;
	
	//检验数据
	for (i = 0; i < 5; i++)
	{
		if (tmp[i] != ucStartPhotoRcv[i]) return 0;
	}
	
	return 1;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//拍照初始化函数
unsigned char PhotoInit(void)
{
	//camera_power_on();
		
	
	xSysCtlDelay(25000000);    //在上电复位后要等2.5秒摄像头才能接收命令
	if (!PhotoResetSend()) goto error_exit;	
//	if (!PhotoSizeSend()) goto error_exit;   //设置图像大小,这句可不要
	if (!PhotoStopSend()) goto error_exit;  //清空摄像头缓冲区	
	if (!PhotoZipRateSet()) goto error_exit;    //设置压缩率为0x80:1
	xSysCtlDelay(10);   //这里要注意,设置压缩率后要延时
	if (!PhotoSavePower()) goto error_exit;
	
	
	
	return 1;
error_exit:
  
    return 0;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
unsigned char PhotoReset(void)
{
	if (!PhotoResetSend()) goto error_exit;	       //设置图像大小后要重启
	if (!PhotoInit()) goto error_exit;
	return 1;
error_exit:
	return 0;
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
unsigned char PhotoSavePower(void)
{
	unsigned long i;//, count;
	unsigned char tmp[5];
	
	//向摄像头发送退出省电命令
	UARTBufferPut(sUART_BASE, (unsigned char *)&ucSavePowerCmd[0], 7);

	if (!UARTBufferGet(sUART_BASE,tmp, 5)) return 0;
	
	//检验数据
	for (i = 0; i < 5; i++)
	{
		if (tmp[i] != ucSavePowerRcv[i]) return 0;
	}
	
	return 1;	
}

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
//拍照函数,包括四个步骤
unsigned long PhotoTake(unsigned char *ucBuf)
{
	unsigned long len;

   
	if(!PhotoInit()) goto error_exit;
	if(!PhotoStartSend()) goto error_exit;  //送开始拍照命令
	if(!PhotoLenRead()) goto error_exit;   //读长度
		
	//len = PhotoDataGet(ucBuf);		//读数
	//if (len == 0)  goto error_exit;

	//if (!PhotoStopSend()) goto error_exit;  //停止拍照,清空缓冲区
	
	//return len;
	
error_exit:
  	return 0;
}

unsigned char ucUartTep[21]; 
unsigned char ucUartLen = 0;

//*****************************************************************************
//
//! \brief .
//!
//! \param .
//! \param .
//! \param .
//!
//! 
//!
//! \return 
//
//*****************************************************************************
unsigned long 
PhotoSendToComputer(void *pvCBData, unsigned long ulEvent,
                    unsigned long ulMsgParam, void *pvMsgData)
{
	  unsigned long i;
	  ucUartTep[ucUartLen] = xUARTCharGetNonBlocking(sUART_DBG_BASE);
	  ucUartLen++;

}

unsigned char
BufferLenGet(void)
{
    return ucUartLen;
}
void
BufferRemove(unsigned char ucLen)
{
    int i;
	  for(i=0; i< (20 - ucLen); i++)
	  {
        ucUartTep[i] = ucUartTep[i+ucLen];
    }
		ucUartLen = ucUartLen - ucLen;
}

int main()
{
	  int i,Index,j;	
	  unsigned char ucRet = 0;
    xSysCtlClockSet(12000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_12MHZ);
    xSysCtlDelay(1000);
	
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART0);	
	  xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART1);
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD13));   
    xSysCtlPeripheralClockSourceSet(xSYSCTL_UART0_MAIN, 1);
	  xSysCtlPeripheralClockSourceSet(xSYSCTL_UART1_MAIN, 1);
	
    sPinTypeUART(sUART_BASE); 
	  sPinTypeUART(sUART_DBG_BASE);

    xUARTConfigSet(sUART_BASE, 38400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	  xUARTConfigSet(sUART_DBG_BASE, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    xUARTEnable(sUART_BASE, (UART_BLOCK_UART | UART_BLOCK_TX | UART_BLOCK_RX));
	  xUARTEnable(sUART_DBG_BASE, (UART_BLOCK_UART | UART_BLOCK_TX | UART_BLOCK_RX));
	  xUARTIntCallbackInit(sUART_DBG_BASE, PhotoSendToComputer);
	  xUARTIntEnable(sUART_DBG_BASE, xUART_INT_RX);
	  xIntEnable(xINT_UART0);
	  //UARTBufferPut(sUART_DBG_BASE, "adfdsgfh", 8);
	
	  //PhotoInit();
	  PhotoTake(&ucPhotoBuf[0]);

    while(1)
    {
			
			  /*UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucStartPhotoRcv[0], 5);
			  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucReadLenRcv[0], 7);
			  xUARTCharPut(sUART_DBG_BASE, ulPhotoLen/256);
			  xUARTCharPut(sUART_DBG_BASE, ulPhotoLen%256);
			
			  PhotoDataGet(&ucPhotoBuf[0]);
			  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucStopPhotoRcv[0], 5);
			
			  xSysCtlDelay(25000000);*/
			  ucRet = BufferLenGet();
			  if(ucRet <= 0) goto end;
        for(i=0; i<ucRet; i++)
			  {
					  if(ucUartTep[i] == 0x56)
						{
							  Index = i;
							  break;
						}
        }
				if(Index > 0)
				{
            BufferRemove(Index);
					  Index = 0;
        }
				ucRet = BufferLenGet();
				if( ucUartTep[0] == ucResetCmd[0] &&
					  ucUartTep[1] == ucResetCmd[1] &&
				    ucUartTep[2] == ucResetCmd[2] &&
				    ucUartTep[3] == ucResetCmd[3] 
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucResetRcv[0], 4);
					  BufferRemove(4);
        }
				if( ucUartTep[0] == ucStartPhotoCmd[0] &&
					  ucUartTep[1] == ucStartPhotoCmd[1] &&
				    ucUartTep[2] == ucStartPhotoCmd[2] &&
				    ucUartTep[3] == ucStartPhotoCmd[3] &&
				    ucUartTep[4] == ucStartPhotoCmd[4]
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucStartPhotoRcv[0], 5);
					  BufferRemove(5);
        }
				if( ucUartTep[0] == ucReadLenCmd[0] &&
					  ucUartTep[1] == ucReadLenCmd[1] &&
				    ucUartTep[2] == ucReadLenCmd[2] &&
				    ucUartTep[3] == ucReadLenCmd[3] &&
				    ucUartTep[4] == ucReadLenCmd[4]
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucReadLenRcv[0], 7);
					  xUARTCharPut(sUART_DBG_BASE, ulPhotoLen/256);
					  xUARTCharPut(sUART_DBG_BASE, ulPhotoLen%256);
					  BufferRemove(5);
        }
				if( ucUartTep[0] == ucSetZipRateCmd[0] &&
					  ucUartTep[1] == ucSetZipRateCmd[1] &&
				    ucUartTep[2] == ucSetZipRateCmd[2] &&
				    ucUartTep[3] == ucSetZipRateCmd[3] &&
				    ucUartTep[4] == ucSetZipRateCmd[4] &&
				    ucUartTep[5] == ucSetZipRateCmd[5] &&
				    ucUartTep[6] == ucSetZipRateCmd[6] &&
				    ucUartTep[7] == ucSetZipRateCmd[7] 
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucSetZipRateRcv[0], 5);
					  BufferRemove(9);
        }
				if( ucUartTep[0] == ucStopPhotoCmd[0] &&
					  ucUartTep[1] == ucStopPhotoCmd[1] &&
				    ucUartTep[2] == ucStopPhotoCmd[2] &&
				    ucUartTep[3] == ucStopPhotoCmd[3] &&
				    ucUartTep[4] == ucStopPhotoCmd[4]
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucStopPhotoRcv[0], 5);
					  BufferRemove(5);
        }
				if( ucUartTep[0] == ucPhotoSizeCmd[0] &&
					  ucUartTep[1] == ucPhotoSizeCmd[1] &&
				    ucUartTep[2] == ucPhotoSizeCmd[2] &&
				    ucUartTep[3] == ucPhotoSizeCmd[3] &&
				    ucUartTep[4] == ucPhotoSizeCmd[4] &&
				    ucUartTep[5] == ucPhotoSizeCmd[5] &&
				    ucUartTep[6] == ucPhotoSizeCmd[6] &&
			    	ucUartTep[7] == ucPhotoSizeCmd[7] 
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucPhotoSizeRcv[0], 5);
					  BufferRemove(9);
        }
				if( ucUartTep[0] == ucSavePowerCmd[0] &&
					  ucUartTep[1] == ucSavePowerCmd[1] &&
				    ucUartTep[2] == ucSavePowerCmd[2] &&
				    ucUartTep[3] == ucSavePowerCmd[3] &&
				    ucUartTep[4] == ucSavePowerCmd[4] &&
				    ucUartTep[5] == ucSavePowerCmd[5] 
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucSavePowerRcv[0], 5);
					  BufferRemove(7);
        }
				if( ucUartTep[0] == ucSetBaudRateCmd[0] &&
					  ucUartTep[1] == ucSetBaudRateCmd[1] &&
				    ucUartTep[2] == ucSetBaudRateCmd[2] &&
				    ucUartTep[3] == ucSetBaudRateCmd[3] &&
				    ucUartTep[4] == ucSetBaudRateCmd[4] &&
				    ucUartTep[5] == ucSetBaudRateCmd[5] 
					)
				{
					  UARTBufferPut(sUART_DBG_BASE, (unsigned char *)&ucSetBaudRateRcv[0], 5);
					  BufferRemove(7);
        }
				if( ucUartTep[0] == ucGetPhotoCmd[0] &&
					  ucUartTep[1] == ucGetPhotoCmd[1] &&
				    ucUartTep[2] == ucGetPhotoCmd[2] &&
				    ucUartTep[3] == ucGetPhotoCmd[3] &&
				    ucUartTep[4] == ucGetPhotoCmd[4] &&
				    ucUartTep[5] == ucGetPhotoCmd[5] 
					)
				{
					  BufferRemove(16);
					  PhotoDataGet(&ucPhotoBuf[0]);
					  PhotoTake(&ucPhotoBuf[0]);
					  
        }
				if( ucUartTep[0] == ucFlowerCmd[0] &&
					  ucUartTep[1] == ucFlowerCmd[1] &&
				    ucUartTep[2] == ucFlowerCmd[2] &&
				    ucUartTep[3] == ucFlowerCmd[3]  
					)
				{
					  SysCtlDelay(1000);
					  j=0;
					  switch(ucUartTep[4])
					  {
							  case 0x00 :
								{
                    j=j+0;
									  break;
                }
								case 0x10 :
								{
                    j=j+10;
									  break;
                }
								case 0x20 :
								{
                    j=j+20;
									  break;
                }
								case 0x01 :
								{
                    j=j+1;
									  break;
                }
								case 0x11 :
								{
                    j=j+11;
									  break;
                }
								case 0x21 :
								{
                    j=j+21;
									  break;
                }
								default:
								{
									  break;
								}
						}

					  BufferRemove(5);
					  
        }
				
end:
    }
}
