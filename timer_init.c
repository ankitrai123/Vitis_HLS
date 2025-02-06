// /***************************** Include Files *********************************/

#include "xparameters.h"
#include "xparameters_ps.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xgpio.h"  // Include for LED control if using GPIO for LEDs
#include "xil_exception.h"
#include "xil_printf.h"
#include <stdio.h>
#include "platform.h"
#include "sleep.h"

/************************** Constant Definitions *****************************/

#define TIMER_DEVICE_ID     XPAR_SCUTIMER_BASEADDR
#define INTC_DEVICE_ID      XPAR_XSCUGIC_0_BASEADDR
#define TIMER_IRPT_INTR     XPAR_SCUTIMER_INTR
#define TIMER_LOAD_VALUE    0x1F40000

XScuTimer TimerInstance;

/************************** Function Prototypes ******************************/
int InitializeTimer(XScuTimer *TimerInstancePtr);
void PrintTimerValues(XScuTimer *TimerInstancePtr);

/*****************************************************************************/
/**
 * Main function to initialize the timer and print the timer value every 0.5 seconds.
 *****************************************************************************/
int main(void)
{
    int Status;

    xil_printf("Simple Timer Example\r\n");

    // Initialize the platform
    init_platform();

    // Initialize the timer
    Status = InitializeTimer(&TimerInstance);
    if (Status != XST_SUCCESS) {
        xil_printf("Timer Initialization Failed\r\n");
        return XST_FAILURE;
    }

    // Start the timer
    XScuTimer_Start(&TimerInstance);

    // Loop to print the timer value every 0.5 seconds
    while (1) {
        PrintTimerValues(&TimerInstance);
    }

    cleanup_platform();
    return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * Function to initialize the timer.
 *****************************************************************************/
int InitializeTimer(XScuTimer *TimerInstancePtr)
{
    int Status;
    XScuTimer_Config *ConfigPtr;

    // Lookup configuration based on the device ID
    ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
    if (!ConfigPtr) {
        xil_printf("Timer Configuration Lookup Failed\r\n");
        return XST_FAILURE;
    }

    // Initialize the timer instance with the config
    Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        xil_printf("Timer Configuration Initialization Failed\r\n");
        return XST_FAILURE;
    }

    // Load the timer with a value for a 0.5-second interval
    XScuTimer_LoadTimer(TimerInstancePtr, TIMER_LOAD_VALUE);

    // Enable auto-reload mode for continuous operation
    XScuTimer_EnableAutoReload(TimerInstancePtr);

    return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * Function to print the timer value every 0.5 seconds.
 *****************************************************************************/
void PrintTimerValues(XScuTimer *TimerInstancePtr)
{
    static int Counter = 0;

    // Wait for the timer to expire (polling method)
    while (!XScuTimer_IsExpired(TimerInstancePtr));

    // Clear the interrupt status to restart the timer
    XScuTimer_ClearInterruptStatus(TimerInstancePtr);

    // Print the current counter value
    xil_printf("Timer Value: %u\r\n", XScuTimer_GetCounterValue(TimerInstancePtr));

    // Increment counter to track how many times the timer has expired
    Counter++;

    // Toggle an LED or perform another action every 1 second (2 timer expiries)
    if (Counter % 2 == 0) {
        xil_printf("1 Second Passed: Toggle LED\r\n");
    }
}


// #include <stdio.h>
// #include "platform.h"
// #include "xil_printf.h"
// #include "xparameters.h"
// #include "xgpio.h"
// #include "xparameters.h"
// #include "xgpio.h"
// #include "sleep.h"

// #define GPIO_DEVICE_ID  0x41200000  // GPIO device ID
// #define LED_DELAY       1000000                // Delay in microseconds for LED blink
// #define LED_CHANNEL     1                      // GPIO channel for LEDs
// #define LED_MAX         3                   // 4-bit for 4 LEDs (binary 1111)

// XGpio Gpio;  // GPIO device instance

// int main() {
//     u32 led_value = 0x01;  // Initial LED state

//     // Initialize the GPIO driver
//     if (XGpio_Initialize(&Gpio, 0x41200000) != XST_SUCCESS) {
//         xil_printf("GPIO Initialization Failed\n");
//         return XST_FAILURE;
//     }

//     // Set GPIO direction to output
//     XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0x00);
//     // XGpio_SetDataDirection(&Gpio, 3, 0x00);

//     // Blink LEDs
//     while (1) {
//         XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_value);  // Write LED value
//         // XGpio_DiscreteWrite(&Gpio, 3, led_value); 
//         usleep(LED_DELAY);                                  // Delay
//         led_value = (led_value << 1) & LED_MAX;             // Shift LED
//         if (led_value == 0) led_value = 0x01;               // Reset if overflow
//     }

//     return 0;
// }



