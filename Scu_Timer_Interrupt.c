#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xparameters_ps.h"


// #include "xparameters.h"

// #include "xscutimer.h"
// #ifndef SDT
// #include "xscugic.h"
// #else
// #include "xinterrupt_wrap.h"
// #endif
// #include "xil_exception.h"
// #include "xil_printf.h"



// Device IDs for Timer and Interrupt controller
#define TIMER_DEVICE_ID 0xf8f00600
#define INTC_DEVICE_ID 0xf8f01000
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR

// Timer load value
#define TIMER_LOAD_VALUE 0xFFFF

XScuTimer TimerInstance;   // Timer Instance
XScuGic IntcInstance;      // Interrupt Controller Instance
static volatile int TimerExpired = 0;  // Flag to track the number of timer expirations

void TimerIntrHandler(void *CallBackRef);
int SetupInterruptSystem(XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId);

int main(void) {
    int Status;
    XScuTimer_Config *ConfigPtr;

    xil_printf("Timer Example: Setting up Timer with Interrupts...\r\n");

    // Lookup timer configuration
    ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
    if (ConfigPtr == NULL) {
        return XST_FAILURE;
    }

    // Initialize the timer
    Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // Perform a self-test to verify that the timer is functioning correctly
    Status = XScuTimer_SelfTest(&TimerInstance);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // Setup interrupt system for timer
    Status = SetupInterruptSystem(&IntcInstance, &TimerInstance, TIMER_IRPT_INTR);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // Enable auto-reload for continuous operation
    XScuTimer_EnableAutoReload(&TimerInstance);

    // Load the timer with an initial value
    XScuTimer_LoadTimer(&TimerInstance, TIMER_LOAD_VALUE);

    // Start the timer
    XScuTimer_Start(&TimerInstance);
    xil_printf("Timer started.\r\n");

    // Wait for the timer to expire 3 times before stopping
    while (TimerExpired < 3) {
        // Busy wait
    }

    // Stop the timer after 3 expirations
    XScuTimer_Stop(&TimerInstance);
    xil_printf("Timer stopped after 3 expirations.\r\n");

    return XST_SUCCESS;
}

// Interrupt handler for the timer
void TimerIntrHandler(void *CallBackRef) {
    XScuTimer *TimerInstancePtr = (XScuTimer *)CallBackRef;

    // Check if the timer has expired
    if (XScuTimer_IsExpired(TimerInstancePtr)) {
        XScuTimer_ClearInterruptStatus(TimerInstancePtr);
        TimerExpired++;
        xil_printf("Timer expired %d times\r\n", TimerExpired);

        if (TimerExpired == 3) {
            // Disable auto-reload after 3 expirations
            XScuTimer_DisableAutoReload(TimerInstancePtr);
        }
    }
}

// Setup interrupt system for the timer
int SetupInterruptSystem(XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId) {
    int Status;
    XScuGic_Config *IntcConfig;

    // Initialize the interrupt controller
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (IntcConfig == NULL) {
        return XST_FAILURE;
    }

    Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // Register the interrupt handler
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, IntcInstancePtr);

    // Connect the timer interrupt handler
    Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId, (Xil_ExceptionHandler)TimerIntrHandler, (void *)TimerInstancePtr);
    if (Status != XST_SUCCESS) {
        return Status;
    }

    // Enable the timer interrupt
    XScuGic_Enable(IntcInstancePtr, TimerIntrId);

    // Enable interrupts in the processor
    Xil_ExceptionEnable();

    // Enable timer interrupt
    XScuTimer_EnableInterrupt(TimerInstancePtr);

    return XST_SUCCESS;
}
