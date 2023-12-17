/*
 C Bonus Assignment | Que 2 - Microwave Oven Embedded System using C
 Submitted By: Gurkirat Bindra
 Student ID: 218862722
 Course: EECS2032
 Section: Z
 */

// Libraries relevant to the "FRDM KL-43Z" board & MCUXpresso SDK
#include "board.h"    // contains functions and definitions specific to the board and to initialize & configure its hardware components
#include "fsl_gpio.h" // for GPIO pins on the board (can config as digital inputs/outputs) used to interact with components such as relays
#include "fsl_port.h" // for Port module of the board (additional functionality to GPIO pins)
#include "fsl_uart.h" // functions and definitions for working with UART (Universal Async Transmitter/Reciever)

// Macro checks to see if the button connected to PTA1 pin is pressed
#define KEY_PRESSED (GPIO_ReadPinInput(GPIOA, 1) == 0)

// Declaration of implemented functions
void setup();
void display(const char *message); // to show messages on the LCD screen
void start_microwave(uint32_t duration); // to control the microwave's operation

int main(void)
{
    setup();    // Initializing the hardware

    while (1)   // Starting an inifinite loop (continously checks if button is pressed)
    {
        // Conditional executes once the button recieves user input
        if (KEY_PRESSED)
        {
        // Operational status displayed on LCD screen
            display("Microwave ON");
            start_microwave(30); // 30-second duration
            display("Microwave OFF");
        }
    }
}

// Defining the setup function
void setup()
{
    // Initialize hardware using the MCUXpresso SDK
    BOARD_InitPins(); // Initializes the board's pins
    BOARD_BootClockRUN(); // Sets up the board's clock source and speed
    BOARD_InitDebugConsole(); // Sets up a debug console for development

    // Set up the button input for the keypad (PTA1 Pin)
    gpio_pin_config_t keyConfig = {kGPIO_DigitalInput, 0}; // Configures the pin to accept user input (digital input)
    GPIO_PinInit(GPIOA, 1, &keyConfig); // Initializes the PTA1 pin for the keypad

    // Set up the relay output for relay control (PTA2 pin)
    gpio_pin_config_t relayConfig = {kGPIO_DigitalOutput, 0}; // Configures the pin as a digital output
    GPIO_PinInit(GPIOA, 2, &relayConfig); // Initializes the PTA2 pin for the relay

    // Initialize UART for LCD display (Universal Asynchronous Reciever/Transmitter)
    uart_config_t uartConfig;
    UART_GetDefaultConfig(&uartConfig); // Gets the default UART configuration
    uartConfig.baudRate_Bps = 9600; // Baud rate of 9600 bps to communicate with the LCD display
    
    // Board only needs to send data to the LCD screen so...
    uartConfig.enableTx = true; // Tx (transmit) is enabled
    uartConfig.enableRx = false; // Rx (recieve) is disabled
    UART_Init(UART0, &uartConfig, CLOCK_GetFreq(kCLOCK_CoreSysClk)); // Initialization of the UART module with modified configuration (lines 62-63)
    // Enables the board to send data to the LCD display using UART communication

    // Display a welcome message on the LCD screen
    display("Press button to start");
}

// Defining the display function
void display(const char *message)
{
    // Clear the LCD screen
    uint8_t clearScreenCmd[] = {0xFE, 0x01}; // Array containing the command to clear the LCD screen
    UART_WriteBlocking(UART0, clearScreenCmd, sizeof(clearScreenCmd)); // Send the clear screen command to the LCD display
    SDK_DelayAtLeastUs(50000, CLOCK_GetFreq(kCLOCK_CoreSysClk)); // Wait 50ms for the LCD to process the command

    // Display the provided message on the LCD screen
    UART_WriteBlocking(UART0, (const uint8_t *)message, strlen(message)); // Send the message to the LCD display
}

// Defining the start_microwave function
void start_microwave(uint32_t duration)
{
    // Turn on the relay to start the microwave (PTA2 pin)
    GPIO_WritePinOutput(GPIOA, 2, 1); // Set the PTA2 pin to high (1) to activate the relay

    // Wait for the specified duration (in seconds)
    SDK_DelayAtLeastUs(duration * 1000000, CLOCK_GetFreq(kCLOCK_CoreSysClk)); // Delay for the specified duration in microseconds

    // Turn off the relay to stop the microwave (PTA2 pin)
    GPIO_WritePinOutput(GPIOA, 2, 0); // Set the PTA2 pin to low (0) to deactivate the relay
}
