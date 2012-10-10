#include "cookie.h"
#include "xsysctl.h"
#include "xgpio.h"

#define LED_PIN PB13
#define LED_ON  1
#define LED_OFF 0

#define MAX_TICK 0x4FFFFF

void LEDBlink(void)
{
    //
    // Configure Cookie STM32 Version Board
    //
    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlDelay(1000);

    //
    // Enable LED PORT
    //
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOB);

    //
    // Configure LED Pin
    //
    xGPIOSPinTypeGPIOOutput(LED_PIN);

    while(1)
    {
        //
        // Turn LED on and delay
        //
        xGPIOSPinWrite(LED_PIN, LED_ON);
        xSysCtlDelay(MAX_TICK);

        //
        // Turn LED off and delay
        //
        xGPIOSPinWrite(LED_PIN, LED_OFF);
        xSysCtlDelay(MAX_TICK);
    }
}
