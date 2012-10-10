#include "coshining.h"
#include "coshining_cfg.h"

#include "xhw_pwm.h"
#include "xpwm.h"
#include "xhw_types.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xhw_memmap.h"
#include "xdebug.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xhw_sysctl.h"
#include "xhw_gpio.h"
#include "xgpio.h"
#include "xhw_uart.h"
#include "xhw_i2c.h"

unsigned long ulPWMChannel[4] = {xPWM_CHANNEL7, xPWM_CHANNEL1, xPWM_CHANNEL3, xPWM_CHANNEL4};

void x74HC595Init(void);
void x74HC595SendData(unsigned char ucData);
void MotorShieldInit(unsigned long ulChannel);
void MotorShieldConfig(unsigned long ulChannel, unsigned long ulFreq,
                       unsigned long ulDuty);
void MotorForwardMove(unsigned long ulChannel);
void MotorBackwardMove(unsigned long ulChannel);
void MotorStopMove(void);

//*****************************************************************************
//
//! \brief Using PWM to Driver Motor.
//!
//! \param None
//!
//! \note None.
//!
//! \return None.
//
//*****************************************************************************
void
MotorControl(void)
{
    //
    // Set System clock is 48M
    //
    xSysCtlClockSet(48000000, (xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_12MHZ));

	x74HC595Init();
	MotorStopMove();

	MotorShieldInit(0);

	MotorShieldConfig(0, 100, 50);

	MotorForwardMove(0);

	MotorBackwardMove(0);

	MotorStopMove();
}

//*****************************************************************************
//
//! \brief Init 74HC595
//!
//! \param none.
//!
//! \return none
//
//*****************************************************************************
void
x74HC595Init(void)
{
    //
    // Enable GPIO
    //
    xSysCtlPeripheralEnable2(xGPIOSPinToPort(sD4));
    xSysCtlPeripheralEnable2(xGPIOSPinToPort(sD8));
	xSysCtlPeripheralEnable2(xGPIOSPinToPort(sD12));
	xSysCtlPeripheralEnable2(xGPIOSPinToPort(sD7));

    //
    // Set PE2(DIR_CLK), PD3(DIR_SER), and PA6(DIR_LATCH) PD2(DIR_EN) as output
    //
    xGPIOSPinTypeGPIOOutput(sD4);
    xGPIOSPinTypeGPIOOutput(sD8);
    xGPIOSPinTypeGPIOOutput(sD12);
    xGPIOSPinTypeGPIOOutput(sD7);
}


//*****************************************************************************
//
//! \brief Send data to 74HC595
//!
//! \param ucData the data will send to 74HC595.
//!
//! \return none
//
//*****************************************************************************
void
x74HC595SendData(unsigned char ucData)
{
    unsigned long i = 0;
    unsigned long ulBit = 0;
    xGPIOSPinTypeGPIOOutput(sD4);
    xGPIOSPinTypeGPIOOutput(sD8);
    xGPIOSPinTypeGPIOOutput(sD7);
	
    // 
    // 74HC595 operation
    // Output Enable
    //
    xGPIOSPinWrite(sD7, 1);
    xGPIOSPinWrite(sD7, 0);

    //
    // Send Data 0xF7 to 74HC595
    //
    xGPIOSPinWrite(sD4, 0);
    for(i=0; i<8; i++)
    {
        if((ucData & (1<<i)))
        {
            xGPIOSPinWrite(sD8, 1);
        }
        else
        {
            xGPIOSPinWrite(sD8, 0);	
        }
        xGPIOSPinWrite(sD4, 1);
        xSysCtlDelay(1);
        xGPIOSPinWrite(sD4, 0);
    }
    //
    // Latch Data to 74HC595
    //
    xGPIOSPinWrite(sD12, 0);
    xSysCtlDelay(1);
    xGPIOSPinWrite(sD12, 1);
    xSysCtlDelay(1);
    xGPIOSPinWrite(sD12, 0);
}

//*****************************************************************************
//
//! \brief System and PWM Initialize.
//!
//! \param ulChannel the motor channel.
//!
//! ulChannel can be 0 1 2 3.
//! 
//! \return None.
//
//*****************************************************************************
void
MotorShieldInit(unsigned long ulChannel)
{
    //
    // Enable GPIO
    //
    xSysCtlPeripheralReset(xSYSCTL_PERIPH_PWMA);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_PWMA);

	xSysCtlPeripheralReset(xSYSCTL_PERIPH_PWMB);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_PWMB);

    ///
    // Set PWM Clock Source
    //
    xSysCtlPeripheralClockSourceSet(xSYSCTL_PWMA_MAIN, 4);
    xSysCtlPeripheralClockSourceSet(xSYSCTL_PWMB_MAIN, 4);

	if(ulChannel == 0)
	{
	    //
        // Congigure the PWM pin(PWM7, sD11))
        //
        sD11PinTypePWM();
	}
	else if(ulChannel == 1)
	{
	    //
        // Congigure the PWM pin(PWM1, sD3))
        //
        sD3PinTypePWM();
	}
	else if(ulChannel == 2)
	{
	    //
        // Congigure the PWM pin(PWM3, sD5))
        //
        sD5PinTypePWM();
	}
	else if(ulChannel == 3)
	{
	    //
        // Congigure the PWM pin(PWM4, sD6))
        //
        sD6PinTypePWM();
	}
}

//*****************************************************************************
//
//! \brief motor shield config.
//!
//! \param ulChannel the motor channel.
//! \param ulFreq the PWM Frequency.
//! \param ulDuty the PWM Duty.
//!
//! ulChannel can be 0 1 2 3.
//!
//! \return None.
//
//*****************************************************************************
void
MotorShieldConfig(unsigned long ulChannel, unsigned long ulFreq,
                  unsigned long ulDuty)
{
	//
	// Set invert, dead zone and mode
	//
	xPWMInitConfigure(xPWMB_BASE, ulPWMChannel[ulChannel], xPWM_TOGGLE_MODE |
												           xPWM_OUTPUT_INVERTER_DIS |
												           xPWM_DEAD_ZONE_DIS);

	//
	// Set Frequency
	//
	PWMFrequencySet(xPWMB_BASE, ulPWMChannel[ulChannel], ulFreq);

	//
	// Set Duty
	//
	xPWMDutySet(xPWMB_BASE, ulPWMChannel[ulChannel], 50);

	//
	// Set output enable
	//
	xPWMOutputEnable(xPWMB_BASE, ulPWMChannel[ulChannel]);

	//
	// start PWM
	//
	xPWMStart(xPWMB_BASE, ulPWMChannel[ulChannel]);

}


//*****************************************************************************
//
//! \brief Motor Move forward.
//!
//! \param ulChannel the motor channel.
//!
//! ulChannel can be 0 1 2 3.
//!
//! \return None.
//
//*****************************************************************************
void
MotorForwardMove(unsigned long ulChannel)
{
    if(ulChannel == 0)
	{
	    //
		//  x x 1 0 x x x x
		//
	    x74HC595SendData(0x20);
	}
	else if(ulChannel == 1)
	{
	    //
		//  x 1 x x 0 x x x
		//
	    x74HC595SendData(0x40);
	}
	else if(ulChannel == 2)
	{
	    //
		//  1 x x x x x 0 x
		//
	    x74HC595SendData(0x80);
	}
	else if(ulChannel == 3)
	{
	    //
		//  x x x x x 1 x 0
		//
	    x74HC595SendData(0x04);
	}
}

//*****************************************************************************
//
//! \brief Motor Move backward .
//!
//! \param ulChannel the motor channel.
//!
//! ulChannel can be 0 1 2 3.
//!
//! \return None.
//
//*****************************************************************************
void
MotorBackwardMove(unsigned long ulChannel)
{
    if(ulChannel == 0)
	{
	    //
		//  x x 0 1 x x x x
		//
	    x74HC595SendData(0x10);
	}
	else if(ulChannel == 1)
	{
	    //
		//  x 0 x x 1 x x x
		//
	    x74HC595SendData(0x08);
	}
	else if(ulChannel == 2)
	{
	    //
		//  0 x x x x x 1 x
		//
	    x74HC595SendData(0x02);
	}
	else if(ulChannel == 3)
	{
	    //
		//  x x x x x 0 x 1
		//
	    x74HC595SendData(0x01);
	}
}

//*****************************************************************************
//
//! \brief Motor Stop Move.
//!
//! \param None.
//!
//! \return None.
//
//*****************************************************************************
void
MotorStopMove(void)
{
    x74HC595SendData(0x00);
}