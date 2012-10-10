#include "cookie.h"
#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xuart.h"

// Only for Fire Board
//#define LED_PIN PC3

#define LED_PIN PB13
#define LED_ON  1
#define LED_OFF 0

#define MAX_TICK 0x4FFFFF

static void PrintStr(const char * pStr)
{
	while(*pStr != '\0')
    {
        xUARTCharPut(USART2_BASE, *pStr++);
    }
}

void ProductionTest(void)
{
    //
    // Configure system clock to 72MHz
    //
    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlDelay(1000);

    //
    // Configure Debug Port (USART2)
    // 115200 8-N-1
    // TX-->PA2
    // RX-->PA3
    //
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART2);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);
    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);
    sPinTypeUART(sUART_DBG_BASE);

    xUARTConfigSet(sUART_DBG_BASE, 115200, (UART_CONFIG_WLEN_8   |
                                            UART_CONFIG_STOP_ONE |
                                            UART_CONFIG_PAR_NONE));
    xUARTEnable(sUART_DBG_BASE, (UART_BLOCK_UART | UART_BLOCK_TX | UART_BLOCK_RX ));

    PrintStr("Welcome, Cookie STM32 board\r\n");

    //
    // Configure LED
    //

    // Only for Fire Board
    //xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOC);
    //xGPIOSPinTypeGPIOOutput(LED_PIN);


    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOB);
    xGPIOSPinTypeGPIOOutput(LED_PIN);

    PrintStr("Now LED is Blink\r\n");

    while(1)
    {

        PrintStr(".");
        xGPIOSPinWrite(LED_PIN , LED_ON);
        xSysCtlDelay(MAX_TICK);

        PrintStr(".");
        xGPIOSPinWrite(LED_PIN , LED_OFF);
        xSysCtlDelay(MAX_TICK);


        PrintStr(".");
        xGPIOSPinWrite(LED_PIN , LED_ON);
        xSysCtlDelay(MAX_TICK);

        PrintStr(".");
        xGPIOSPinWrite(LED_PIN , LED_OFF);
        xSysCtlDelay(MAX_TICK);

        PrintStr("\b\b\b\b\r");
        PrintStr("    \r");
        xSysCtlDelay(MAX_TICK);

    }
}

