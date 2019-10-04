# Infineon XMC 4700 configuration

This document is a step by step documentation on how to configure a [XMC 4700 relax kit](https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc47_relax_v1/) to drive a 16 channel multiplexer. The XMC 4700 will communicate with a Raspberry Pi with EPICS installed through serial communication. We will replace the Arduino microcontroller that we are using in the prototype with the XMC 4700 for added reliability and fine control. This document provides a guide for
- DAVE Installation
- UART Communication
- LCD Display control 

### Dave Installation and first steps

In order to program the XMC 4700 you need to use the DEVE IDE. You can download it from the Infineon [website](https://infineoncommunity.com/dave-download_ID645); follow this link and submit your information to receive an email with the download link. Once downloaded, extract the ``DAVE-IDE-4.4.2-64Bit`` folder and create a desktop shortcut to the ``DAVE`` application located in the Eclipse folder. The program doesn't need an installation, just double click on the shortcut to launch it. 

In order to test and upload our programs on the microcontroller we need to install the SEGGER J-link debugger. Go to this [link](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack) and download the latest version of the software. During the installation make sure to also install the USB Driver for the debugger. Connect the XMC 4700 from the debugger micro USB port to the PC. If the driver are installed correctly, the green LED will blink from a second and then stop.

Now we can create our first project in DAVE. Go to File -> New -> DAVE project... and enter the new of the project. Select DAVE CE Project and click Next. In the next window select your microcontroller (in our case XMC 4700 Relax kit) and tick "Add floating point support printf and scanf" at the bottom. Click Finish and you are done.

DAVE projects are based on APPs. You can add them clicking on the "Add new APP" button located on the top toolbar. APPs are used to add functionalities to your project. We will see some examples in the next sections of the documents. 

On the toolbar you can find other usefull command:


<p align="center">
  <img style="float: right;" src="https://i.imgur.com/GfktHUA.png" width="400">
</p>

- **Rebuild Active Project** is used to compile your current active project. Use this command everytime you modify the ``main.c`` file.
- **Manual Resources Assignment** is used to assign resources, for examples to link an interrupt to other APP
- **Manual Pin Allocator** is used to assign a pin from the XMC 4700 header to one APP
- **Generate Code** is used to generate the header files from the APP of the project. Everytime you modify one of the APP from the GUI (accessed by double clicking it form the APP dependency tab at the bottom) you need to generate a new code before building it.
- **Debugger** is used to start the debugger. After clicking the button, the program will switch into the debug view; in this view you can test your code executing one command line at the time, by pressing the "Step Over" button (F6). In the top right there is the "Variables" tab, where you can check the real time value of every variables of the code. To exit the debugger view use the right part of the toolbar (DAVE CE button).

### UART Communication
With this project we will set up a serial communication between the microcontroller and the computer using the UART protocol. The first thing we need is a way to test the code. Download [Hterm](https://www.heise.de/download/product/hterm-53283), a virtual interface to simulate serial input and output. The program doesn't need installation, just launch the ``.exe`` file. Connect the XMC 4700 from the debugger micro USB to the PC and select the right COM port on the Hterm terminal. Then set the other parameter according to the following image:

<p align="center">
  <img style="float: right;" src="https://i.imgur.com/vSITmo7.png" width="700">
</p>

Create a new DAVE project and add a UART[4.1.12] APP. Double click on it to open the setting panel. Under "Advanced Settings" you can change change the protocol handling for receiving and transmit. The possible choices are (from the UART APP Help page):

- **Interrupt**: Data provided by the user is transmitted using interrupts. FIFO can be configured to optimize the CPU utilization. Data is loaded to the transmit buffer in interrupt service handler. A callback function can be registered in "Interrupt Settings" tab to be executed after the transmission is complete. 

- **DMA**: Data provided by the user is transmitted by configuring DMA memory to peripheral block transfer. FIFO cannot be used in DMA mode. A callback function can be registered in "Interrupt Settings" tab to be executed after the transmission is complete. Note: DMA option uses DMA capable service requests for establishing DMA handshake. Only 2 such service requests are available for each USIC module. So the user may not be able to select this option if the DMA capable service requests of the USIC module are already used. 

- **Direct**: On selecting this option, interrupt signals will be available for external connection. User can choose a way for implementing data transfer. In this mode, the APP APIs implemented using 'Interrupt' or 'DMA' mode cannot be used.

Now we need to wire the APP to the pin. Open the Manual Pin Allocator and set pin 1.4 as "Receive Pin" and pin 1.5 as "Transmit Pin" and then Save and Close. Remember to generate the code everytime you modify the APP.

After that in the prokect explorer on the left, double click on ``main.c`` to open the code. Delete everything and copy this code in:

```C
#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include <stdio.h.>
int main(void)
{
  DAVE_STATUS_t status;
  uint8_t Send_Data[50] = "test";
  uint8_t Rec_Data[20]={'\0'};
  uint8_t index = 0;

  /*Initialize Dave (including UART) */
  status = DAVE_Init();

  if(status == DAVE_STATUS_SUCCESS)
  {
    /*Transmit the string */
    while(Send_Data[index] != 0)
      {
        UART_TransmitWord(&UART_0,Send_Data[index]);
        index++;

        /*Wait for transmit buffer interrupt to fill it again with remaining data */
        while((UART_GetTXFIFOStatus(&UART_0) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) == 0);
        UART_ClearTXFIFOStatus(&UART_0, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
      }

    /* Receive input */
    index = 0;
    while (1)
      {
        if (!UART_IsRXFIFOEmpty (&UART_0))
          {
            Rec_Data[index] = UART_GetReceivedWord(&UART_0);
            index++;
          }
        if(Rec_Data[index-1]=='\n') break;
      }

    /* Format the string */
    int int_value=0;
    char char_value='\0';
    sscanf(Rec_Data,"%s %d",&char_value, &int_value);


    /*Transmit the received data */
    index = 0;
    while(1)
      {
        UART_TransmitWord(&UART_0,Rec_Data[index]);
        index++;

        /*Wait for transmit buffer interrupt to fill it again with remaining data */
        while ((UART_GetTXFIFOStatus (&UART_0) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) == 0);
        UART_ClearTXFIFOStatus(&UART_0, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);

        if(Rec_Data[index-1]=='\n') break;
      }
  }
  else
  {
    XMC_DEBUG("main: Application initialization failed");
    while(1U)
    {
    }
  }
  return 1U;
}




```

This code sends through UART communication the string ``Send_Data`` and then receive data untill a ``\n`` comes through. At that point the program sends back the message received. The code can also formats the string sent if the string is in the format "<character> <spece> <integer>", saving the character in the variable ``char_value`` and the integer in ``int_value``.

To test the program build the project and start the debugger. Then click "Resume" in the debug view toolbar. Use the Hterm terminal to see the receive and send the string to the microcontroller. 

### LCD Display

For this example we will use a Newhaven Dislay [NHD-0420D3Z-FL-GBW-V3](https://www.digikey.com/product-detail/en/newhaven-display-intl/NHD-0420D3Z-FL-GBW-V3/NHD-0420D3Z-FL-GBW-V3-ND/1701256) in I2C mode. To enter I2C mode you need to solder a jumper across the R1 pad on the back on the display, as shown in the following image:

<p align="center">
  <img style="float: right;" src="https://i.imgur.com/KLBopPq.png" width="400">
</p>

To control the LCD we will use the I2C_MASTER APP. Add the APP in the project and double click on it to open the configuration tab. In "General Setting" change the "Desired bus speed" to 40 KHz. Then open the manual pin allocator and link the SCL and SDA pin (for example SCL to pin 0.13 and SDA to pin 3.15), save, close and generate the code. Now connect with a wire the SCL pin to pin 6 of the display and the SDA pin to pin 5. Pin 4 must be wired to ground, while pin 3 to 5V. 

Now if you power the XMC 4700, display should light on. Now we can control the display sending command with the I2C_MASTER_Transmit function:

```c
I2C_MASTER_Transmit  ( I2C_MASTER_t *  handle,  
  bool  send_start,  
  const uint32_t  address,  
  uint8_t *  data,  
  const uint32_t  size,  
  bool  send_stop  
 ) 
 
```
- **handle** I2C device handle of type I2C_MASTER_t*  
- **send_start** The flag to indicate that the start condition need to be send.  
- **address** I2C slave device address.  
- **data** buffer containing the data to transmit.  
- **size** The number of bytes to be send to slave.  
- **send_stop** The flag to indicate that the stop condition need to be send. 
