#include "xgpio.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xil_printf.h"
#include "sleep.h"

int main(void)
{
    XGpio Gpio;
    int Status;

    xil_printf("AXI GPIO LED Test Start\r\n");

    Status = XGpio_Initialize(&Gpio, XPAR_XGPIO_0_BASEADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("GPIO initialization failed\r\n");
        return XST_FAILURE;
    }

    XGpio_SetDataDirection(&Gpio, 1, 0x0);

    while (1) {
        XGpio_DiscreteWrite(&Gpio, 1, 0x1);
        usleep(5000);   // 5 ms

        XGpio_DiscreteWrite(&Gpio, 1, 0x0);
        usleep(5000);   // 5 ms
    }

    return 0;
}