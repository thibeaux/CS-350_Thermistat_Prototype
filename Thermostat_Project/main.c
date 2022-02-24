/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * @author: Brandon Thibeaux
 * @class:  CS-350
 * @Date:   2/20/2022
 * @Brief:  This is a project that is supposed to demonstrate how a thermostat would work using the CC3220S_LAUNCHXL micro-controller
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* ================================================= Program Global Variables ================================================= */
// Machine State Enumerations
enum State {START, HEATER_ON , HEATER_OFF} state;

uint16_t temperature = 0; // =ASCII decimal value of room temperature (00 - 99) degrees Celsius
uint16_t setPoint=30; // ASCII decimal value of set-point temperature (00-99) degrees Celsius
int heaterFlag = 0; //  ‘0’ if heat is off, ‘1’ if heat is on
uint8_t increaseFlag = 0;
uint8_t decreaseFlag = 0;

/* ================================================= UART ================================================= */
/* UART Global Variables */
/* Definitions */
#define DISPLAY(x) UART_write(uart, &output, x);

// UART Global Variables
char output[64];
int bytesToSend;
// Driver Handles - Global variables
UART_Handle uart;
void initUART(void)
{
    UART_Params uartParams;
    // Init the driver
    UART_init();

    // Configure the driver
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;
    // Open the driver
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL)
    {
        /* UART_open() failed */
        while (1);
    }
}
/* ================================================= TIMER ================================================= */
// Driver Handles - Global variables
Timer_Handle timer0;
volatile unsigned char TimerFlag = 0;
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{

    TimerFlag = 1; // set flag

    // Determine state functionality to be executed next in the main while loop
    switch(state)
      {
          case START:
              //Describe START state functionality. In other words, see if our temp is higher or lower than the default set point
              if(temperature >= setPoint)
              {
                  state = HEATER_OFF;
              }
              else if(temperature < setPoint)
              {
                  state = HEATER_ON;
              }
              else
              {
                  state = START;
              }

              break;

          case HEATER_ON:
              // If our temperature is less than the set point remain on the on state. Else go to OFF state
              if(temperature >= setPoint)
              {
                  state = HEATER_OFF;
              }
              else if(temperature < setPoint)
              {
                  state = HEATER_ON;
              }
              else
              {
                  state = START;
              }
              break;

          case HEATER_OFF:
              // If our temperature is greater than or equal to the set point, remain in this state. Else go to ON state
              if(temperature >= setPoint)
              {
                  state = HEATER_OFF;
              }
              else if(temperature < setPoint)
              {
                  state = HEATER_ON;
              }
              else
              {
                  state = START;
              }
              break;

          default:
              state = START;
              break;
      }
}
void initTimer(void)
{
    Timer_Params params;
    // Init the driver
    Timer_init();

    // Configure the driver
    Timer_Params_init(&params);
    params.period = 1000; // 1 second
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    // Open the driver
    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL)
    {
        /* Failed to initialized timer */
        while (1) {}
    }
    if (Timer_start(timer0) == Timer_STATUS_ERROR)
    {
        /* Failed to start timer */
        while (1) {}
    }
}

/*================================================= I2C ================================================= */
// I2C Global Variables
static const struct {
uint8_t address;
uint8_t resultReg;
char *id;
} sensors[3] = {
{ 0x48, 0x0000, "11X" },
{ 0x49, 0x0000, "116" },
{ 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];
uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;
// Driver Handles - Global variables
I2C_Handle i2c;

// Make sure you call initUART() before calling this function.
void initI2C(void)
{
    int8_t i, found;
    I2C_Params i2cParams;
    DISPLAY(snprintf(output, 64, "Initializing I2C Driver - "))

    // Init the driver
    I2C_init();
    // Configure the driver
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    // Open the driver
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL)
    {
        DISPLAY(snprintf(output, 64, "Failed\n\r"))
        while (1);
    }
    DISPLAY(snprintf(output, 32, "Passed\n\r"))
    // Boards were shipped with different sensors.
    // Welcome to the world of embedded systems.
    // Try to determine which sensor we have.
    // Scan through the possible sensor addresses
    /* Common I2C transaction setup */
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;
    found = false;
    for (i=0; i<3; ++i)
    {
        i2cTransaction.slaveAddress = sensors[i].address;
        txBuffer[0] = sensors[i].resultReg;
        DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id))
        if (I2C_transfer(i2c, &i2cTransaction))
        {
            DISPLAY(snprintf(output, 64, "Found\n\r"))
            found = true;
            break;
        }
            DISPLAY(snprintf(output, 64, "No\n\r"))
    }
    if(found)
    {
        DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address: %x\n\r", sensors[i].id, i2cTransaction.slaveAddress))
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Temperature sensor not found, contact professor\n\r"))
    }
}
// READ TEMPERATURE FUNTION
int16_t readTemp(void)
{
    int j;
    int16_t temperature = 0;
    i2cTransaction.readCount = 2;
    if (I2C_transfer(i2c, &i2cTransaction))
    {
        /*
        * Extract degrees C from the received data;
        * see TMP sensor datasheet
        */
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
        temperature *= 0.0078125;
        /*
        * If the MSB is set '1', then we have a 2's complement
        * negative value which needs to be sign extended
        */
        if (rxBuffer[0] & 0x80)
        {
        temperature |= 0xF000;
        }
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Error reading temperature sensor (%d)\n\r",i2cTransaction.status))
        DISPLAY(snprintf(output, 64, "Please power cycle your board by unplugging USB and plugging back in.\n\r"))
    }
    // Make sure that the temperature value can only return a number between 00 and 99
    if(temperature <= 0)
    {
        temperature = 0;
    }
    if(temperature >= 99)
    {
        temperature = 99;
    }
    return temperature;
}
/* ================================================= GPIO Interrupt Callback ================================================= */
/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    // If button is pressed increase the set point to change heater state
    /* Toggle an LED */
    //GPIO_toggle(CONFIG_GPIO_LED_0); // Debug line

    increaseFlag = 1;
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn1(uint_least8_t index)
{
    // If button is pressed decrease the set point to change heater state
    /* Toggle an LED */
    //GPIO_toggle(CONFIG_GPIO_LED_0); // Debug line

    decreaseFlag = 1;
}

/*
 * ========= STATE MACHINE =========
 */
void latch()
{
    //GPIO_toggle(CONFIG_GPIO_LED_1);
    switch(state)
    {
        case HEATER_ON:
            // Describe what the heater does when it is on the ON state
            heaterFlag = 1;
            break;
        case HEATER_OFF:
            // Describe what the heater does in the OFF state
            heaterFlag = 0;
            break;
    }
}

/*
 *  ================================================= Main Thread =================================================
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    initUART();
    GPIO_init();
    initTimer();
    initI2C();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);

    /* Install Button callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);

    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on CONFIG_GPIO_BUTTON1.
     */
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        /* Configure BUTTON1 pin */
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);

        /* Install Button callback */
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }
    // see chart for size of this data type below. We need a large data type to handle the amount of milliseconds the program may live. 8 bits and 16 bit data types are too small
    // With a 64 bit unsigned data type we reduce errors like getting a negative number and we get a variable that can hold about 5 hours of millisecond increments
    uint64_t timer  = 0; //in ms
    while(1)
    {
        // Check temperature and update LED every 500ms use latch function for execution
        // Check Button flags if raised on lowered ever 200ms
        // Send update report to server (UART TERMINAL) every 1 second.
        /*
         *  Server Update should be formatted as <AA,BB,S,CCCC>. This can be broken down as follows:
         *  AA = ASCII decimal value of room temperature (00 - 99) degrees Celsius
         *  BB = ASCII decimal value of set-point temperature (00-99) degrees Celsius
         *  S = ‘0’ if heat is off, ‘1’ if heat is on
         *  CCCC = decimal count of seconds since board has been reset
         *  <%02d,%02d,%d,%04d> = temperature, set-point, heat, seconds
         */
        //DISPLAY(snprintf(output, sizeof(output), "                                       \r",timer));
       // DISPLAY(snprintf(output, sizeof(output), "Count: %d \r",timer));

        if(timer % 1000 == 0) // happens every 1000 ms or 1 second
        {
            //FIXME: Make output exactly how the Rubric says to make the output look. Get rid of this extra text.
            DISPLAY(snprintf(output, sizeof(output), "%02d,%02d,%d,%04d\r",temperature,setPoint,heaterFlag,(timer/1000))); // timer /1000 converts the time from milliseconds to seconds
            //GPIO_toggle(CONFIG_GPIO_LED_0);
        }
        if(timer % 500 == 0) // happens ever 500 ms
        {
            //DISPLAY(snprintf(output, sizeof(output), "Timer val: %d \r",timer)); // Debug line
            //GPIO_toggle(CONFIG_GPIO_LED_0); // Debug line

            temperature = readTemp(); //get temperature
            latch(); // update current state of the machine
            if (heaterFlag)
            {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
            }
            else
            {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
            }
        }
        if(timer % 200 == 0) // happens every 200 ms
        {
            //DISPLAY(snprintf(output, sizeof(output), "Timer val: %d \r",timer)); // Debug line
            //GPIO_toggle(CONFIG_GPIO_LED_0); // Debug line

           if(increaseFlag)
           {
               setPoint++;
               increaseFlag = 0;
           }
           else if(decreaseFlag)
           {
               setPoint--;
               decreaseFlag = 0;
           }


        }
        while(!TimerFlag){}//wait for timer period
        TimerFlag = 0; // lower flag
        timer++;
    }

    return (NULL);
}

/*
 * Data Type Chart For Reference:
 * Type Name    Bytes           Other Names                     Range of Values
 * int              4            signed                          -2,147,483,648 to 2,147,483,647
   unsigned int    4            unsigned                         0 to 4,294,967,295
   __int8           1           char                            -128 to 127
   unsigned int8    1            unsigned char                   0 to 255
__int16             2      short, short int, signed short int  -32,768 to 32,767
unsigned int16    2      unsigned short, unsigned short int        0 to 65,535
__int32          4       signed, signed int, int                 -2,147,483,648 to 2,147,483,647
unsigned int32    4      unsigned, unsigned int                     0 to 4,294,967,295
__int64             8   long long, signed long long              -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
unsigned __int64    8   unsigned long long                        0 to 18,446,744,073,709,551,615
 */
