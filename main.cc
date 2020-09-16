/*
 * Empty C++ Application
 */
#include "xgpio.h"
#include "xil_io.h"
#include "xparameters.h"
#include "sleep.h"
#include "xsdps.h"
#include "ff.h"

#define GPIO_LED_FPGA   XPAR_AXI_GPIO_LED0_DEVICE_ID
#define SDCARD_DATA_ADR 			0x00100000

/************************** Class Prototypes ******************************/
static int readSD();

/************************** Class Objects ******************************/
XGpio GpioLED;

/************************** Global Variables ***************************/
uint8_t *data;
uint8_t sdData[40];
float floatTest;

/************************** File Objects ***************************/
static char FileNameLength[16] = "data.bin";
static FIL fil;
static FATFS fatfs;
static char *SD_File;

struct sdata{
	float data1x = 0;
	float data1y = 0;
	float data1z = 0;
	float data2x = 0;
	float data2y = 0;
	float data2z = 0;
	float data3x = 0;
	float data3y = 0;
	float data3z = 0;
}__attribute__((packed));

int main()
{
	sdata dataObj;
	XGpio_Initialize(&GpioLED, GPIO_LED_FPGA);
	/* Set the direction for all signals as inputs except the LED output */
	XGpio_SetDataDirection(&GpioLED, 1, 0x00);
	/* Set low all gpio */
	XGpio_DiscreteWrite(&GpioLED, 1, 0);

	data = reinterpret_cast<uint8_t*>(SDCARD_DATA_ADR);

	// read sd card
	int status = readSD();
	memcpy(&floatTest,reinterpret_cast<uint8_t*>(SDCARD_DATA_ADR),4);

	uint8_t taskCounter = 0;
	int offset = 0;
	// inf loop
	while(true)
	{
		taskCounter++;
		XGpio_DiscreteWrite(&GpioLED, 1, (taskCounter % 2));
		memcpy(&dataObj,reinterpret_cast<uint8_t*>(SDCARD_DATA_ADR+offset),24);
		offset = offset + 24;
		sleep(1);
	}

	return 0;
}

static int readSD()
{
	FRESULT Res;
	UINT NumBytesRead;
	/*
	 * To test logical drive 0, Path should be "0:/"
	 * For logical drive 1, Path should be "1:/"
	 */
	TCHAR *Path = "0:/";
	/*
	 * Register volume work area, initialize device
	 */
	Res = f_mount(&fatfs, Path, 0);

	if (Res != FR_OK) {
		return XST_FAILURE;
	}
	/*
	 * Open file with required permissions.
	 * Here - Creating new file with read/write permissions. .
	 * To open file with write permissions, file system should not
	 * be in Read Only mode.
	 */
	SD_File = (char *)FileNameLength;

	Res = f_open(&fil, SD_File, FA_READ);
	if (Res) {
		return XST_FAILURE;
	}

	/*
	 * Pointer to beginning of file .
	 */
	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	/*
	 * Read data from file.
	 */
	Res = f_read(&fil, (void*)data, 48000,
			&NumBytesRead);
	if (Res) {
		return XST_FAILURE;
	}

	Res = f_close(&fil);
	if (Res) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
