#include "xparameters.h"   // SDK generated parameters
#include "xsdps.h"         // SD device driver
#include "ff.h"            // FATFS header for file operations
#include "xil_printf.h"    // For xil_printf

// FATFS Variables
static FATFS fatfs;            // File system object
static FIL fil;                // File object
static char FileName[] = "Test.txt";  // File name
static char Path[] = "0:/";     // SD card logical drive

// Buffers for reading/writing
char WriteBuffer[] = "Hello SD Card!";  // Data to write
char ReadBuffer[50];                    // Buffer to store read data

int main(void)
{
    FRESULT res;              // Result code
    UINT bytesRead, bytesWritten;

    xil_printf("SD Card Read/Write Example\r\n");

    // Mount the SD card
    res = f_mount(&fatfs, Path, 0);
    if (res != FR_OK) {
        xil_printf("Failed to mount SD card\r\n");
        return -1;
    }

    // Open or create a file with write access
    res = f_open(&fil, FileName, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        xil_printf("Failed to open file for writing\r\n");
        return -1;
    }

    // Write data to the file
    res = f_write(&fil, WriteBuffer, sizeof(WriteBuffer), &bytesWritten);
    if (res != FR_OK || bytesWritten != sizeof(WriteBuffer)) {
        xil_printf("Failed to write to file\r\n");
        f_close(&fil);
        return -1;
    }

    xil_printf("Data written to file: %s\r\n", WriteBuffer);

    // Close the file
    f_close(&fil);

    // Open the file for reading
    res = f_open(&fil, FileName, FA_READ);
    if (res != FR_OK) {
        xil_printf("Failed to open file for reading\r\n");
        return -1;
    }

    // Read data from the file
    res = f_read(&fil, ReadBuffer, sizeof(WriteBuffer), &bytesRead);
    if (res != FR_OK || bytesRead == 0) {
        xil_printf("Failed to read from file\r\n");
        f_close(&fil);
        return -1;
    }

    xil_printf("Data read from file: %s\r\n", ReadBuffer);

    // Close the file and unmount the file system
    f_close(&fil);
    f_mount(NULL, Path, 0);

    return 0;
}
