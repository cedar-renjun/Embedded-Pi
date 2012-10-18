#include "cookie.h"
#include "xgpio.h"
#include "xadc.h"
#include "xuart.h"
#include "xsysctl.h"
#include "LCDShield.h"

#define SHIELD_UART             xSYSCTL_PERIPH_UART1
#define SHIELD_UART_CLK         xSYSCTL_UART0_MAIN

#define SHIELD_ADC              xSYSCTL_PERIPH_ADC1
#define SHIELD_ADC_BASE         xADC1_BASE

const char cKey[5][6] = {"right","up","dowm","left","select"};

int main()
{
    int key;

    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlDelay(1000);

    xSysCtlPeripheralEnable(SHIELD_UART);
    xSysCtlPeripheralEnable(xGPIOSPinToPeripheralId(sD13));
    xSysCtlPeripheralClockSourceSet(SHIELD_UART_CLK, 1);

    LCDShieldInit();

    //
    // Enable Peripheral SPI0
    //
    xSysCtlPeripheralEnable(SHIELD_ADC);

    xSPinTypeADC(ADC10, sA0);


    //
    // ADC Channel0 convert once, Software tirgger.
    //
    xADCConfigure(SHIELD_ADC_BASE, xADC_MODE_SCAN_CONTINUOUS, ADC_TRIGGER_PROCESSOR);

    //
    // Enable the channel0
    //
    xADCStepConfigure(SHIELD_ADC_BASE, 0, xADC_CTL_CH0);

    //
    // Enable the adc
    //
    xADCEnable(SHIELD_ADC_BASE);

    //
    // start ADC convert
    //
    xADCProcessorTrigger(SHIELD_ADC_BASE);

    LCDShieldLocationSet(0, 0);
    LCDShieldDisplayString("Hello Cookie!");
    LCDShieldLocationSet(0, 1);
    LCDShieldDisplayString("Hello CooCox! ");

    xSysCtlDelay(1000000);

    while(1)
    {
        key = LCDShieldButtonGet();
        if(key != -1)
        {
            LCDShieldDisplayClear();
            LCDShieldLocationSet(0, 0);
            LCDShieldDisplayString("The key is: ");
            LCDShieldLocationSet(0, 1);
            LCDShieldDisplayString(&cKey[key][0]);
        }
    }
}
