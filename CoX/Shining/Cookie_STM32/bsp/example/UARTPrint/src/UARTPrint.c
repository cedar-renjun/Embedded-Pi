#include "cookie.h"
#include "xgpio.h"
#include "xuart.h"
#include "xsysctl.h"

#define MAX_TICK 0x4FFFFF

static void PrintStr(const char * pStr)
{
	while(*pStr != '\0')
    {
        xUARTCharPut(USART2_BASE, *pStr++);
    }
}

void UARTPrint(void)
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

    PrintStr("Welcome ! Cookie STM32 Version Board\r\n");
    PrintStr("Running");

    while(1)
    {

        PrintStr(".");
        xSysCtlDelay(MAX_TICK);

        PrintStr(".");
        xSysCtlDelay(MAX_TICK);


        PrintStr(".");
        xSysCtlDelay(MAX_TICK);

        PrintStr(".");
        xSysCtlDelay(MAX_TICK);

        PrintStr("\b\b\b\b    \b\b\b\b");

    }
}

