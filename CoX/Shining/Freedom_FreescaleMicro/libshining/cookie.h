//*****************************************************************************
//
//! \file cookie.h
//! \brief Defines and Macros for Freedom Board.
//! \version V2.1.1.0
//! \date 12/29/2011
//! \author CooCox
//! \copy
//!
//! Copyright (c)  2011, CooCox 
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __COOKIE_H__
#define __COOKIE_H__

#include "cookie_cfg.h"
#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xgpio.h"
#include "xdebug.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
//extern "C"
//{
#endif

//*****************************************************************************
//
//! \addtogroup Cookie Cookie
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_Board_Res CookieBoard Resource
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_B_Arduino_Compatible Cookie Arduino Compatible Part
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_B_Arduino_Digital Cookie Arduino Compatible Digital Part
//! @{
//
//*****************************************************************************

#define sD0                     PA1
#define sD1                     PA2
#define sD2                     PD4
#define sD3                     PA12
#define sD4                     PA4
#define sD5                     PA5
#define sD6                     PC8
#define sD7                     PC9
#define sD8                     PA13
#define sD9                     PD5
#define sD10                    PD0
#define sD11                    PD2
#define sD12                    PD3
#define sD13                    PD1
#define sD14                    PE0
#define sD15                    PE1

#define sSDA                    PE0
#define sSCL                    PE1

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_B_Arduino_ANALOG Co-Shining Arduino Compatible ANALOG Part
//! @{
//
//*****************************************************************************

#define sA0                     PB0
#define sA1                     PB1
#define sA2                     PB2
#define sA3                     PB3
#define sA4                     PC2
#define sA5                     PC1


//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_B_Arduino_ICSP Co-Shining Arduino Compatible ICSP Part
//! @{
//
//*****************************************************************************

//#define sICSP_1_MISO            PB6
//#define sICSP_3_SCK             PB7
//#define sICSP_4_MOSI            PB5

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_B_Arduino_Communication Co-Shining Arduino Compatible Communication Part
//! @{
//
//*****************************************************************************

#define sSPI_BASE               xSPI0_BASE
//#define sICSP_SPI_BASE          xSPI0_BASE
#define sI2C_BASE               xI2C1_BASE
#define sADC_BASE               xADC0_BASE
#define sUART_BASE              xUART0_BASE
#define sUART_DBG_BASE          xUART1_BASE 

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup CS_Exported_APIs Co-Shining API
//! \brief Co-Shining Pin Configuration API
//!
//! @{
//
//*****************************************************************************
#define sPinTypeUART(ulBase)                                                  \
        do                                                                    \
        {                                                                     \
         xASSERT(ulBase == sUART_BASE || ulBase == sUART_DBG_BASE);           \
                                                                              \
         if(ulBase == sUART_BASE)                                             \
         {                                                                    \
           xSPinTypeUART(UART0RX, sD0);                                       \
           xSPinTypeUART(UART0TX, sD1);                                       \
         }                                                                    \
         else if(ulBase == sUART_DBG_BASE)                                    \
         {                                                                    \
           xSPinTypeUART(UART1TX, PC4);                                       \
           xSPinTypeUART(UART1RX, PC3);                                       \
         }                                                                    \
        }while(0)
        
#define sPinTypeSPI(ulBase)                                                   \
        do                                                                    \
        {                                                                     \
           xSPinTypeSPI(SPI0CS,  sD10);                                       \
           xSPinTypeSPI(SPI0MISO,sD12);                                       \
           xSPinTypeSPI(SPI0CLK, sD13);                                       \
        }while(0) 
            

#define sPinTypeI2C(ulBase)                                                   \
        do                                                                    \
        {                                                                     \
         xASSERT(ulBase == sI2C_BASE);                                        \
                                                                              \
         xSPinTypeI2C(I2C1SDA, sSDA);                                         \
         xSPinTypeI2C(I2C1SCK, sSCL);                                         \
        }while(0)


#define sA0PinTypeADC()         xSPinTypeADC(ADC0, sA0)
#define sA1PinTypeADC()         xSPinTypeADC(ADC1, sA1)
#define sA2PinTypeADC()         xSPinTypeADC(ADC2, sA2)
#define sA3PinTypeADC()         xSPinTypeADC(ADC3, sA3)
#define sA4PinTypeADC()         xSPinTypeADC(ADC4, sA4)
#define sA5PinTypeADC()         xSPinTypeADC(ADC5, sA5)

#define sD3PinTypePWM()         xSPinTypePWM(PWM4, sD3)
#define sD5PinTypePWM()         xSPinTypePWM(PWM2, sD5)
#define sD6PinTypePWM()         xSPinTypePWM(PWM4, sD6)
#define sD9PinTypePWM()         xSPinTypePWM(PWM5, sD9)
#define sD10PinTypePWM()        xSPinTypePWM(PWM6, sD10)
#define sD11PinTypePWM()        xSPinTypePWM(PWM7, sD11)

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
//}
#endif

#endif // __COOKIE_H__  
