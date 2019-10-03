# Infineon XMC 4700 configuration

This document is a step by step documentation on how to configure a [XMC 4700 relax kit](https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc47_relax_v1/) to drive a 16 channel multiplexer. The XMC 4700 will communicate with a Raspberry Pi with EPICS installed through serial communication. We will replace the Arduino microcontroller that we are using in the prototype with the XMC 4700 for added reliability and fine control. This document provides a guide for
- DAVE Installation
- UART Communication
- LCD Display control 

### Dave Installation and first steps

In order to program the XMC 4700 you need to use the DEVE IDE. You can download it from the Infineon [website](https://infineoncommunity.com/dave-download_ID645); follow this link and submit your information to receive an email with the download link. Once downloaded, extract the ``DAVE-IDE-4.4.2-64Bit`` folder and create a desktop shortcut to the ``DAVE`` application located in the Eclipse folder. The program doesn't need an installation, just double click on the shortcut to launch it. 

In order to test and upload our programs on the microcontroller we need to install the SEGGER J-link debugger. Go to this [link](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack) and download the latest version of the software. During the installation make sure to also install the USB Driver for the debugger. 

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

