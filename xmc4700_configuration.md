# Infineon XMC 4700 configuration

This document is a step by step documentation on how to configure a [XMC 4700 relax kit](https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc47_relax_v1/) to drive a 16 channel multiplexer. The XMC 4700 will communicate with a Raspberry Pi with EPICS installed through serial communication. We will replace the Arduino microcontroller that we are using in the prototype with the XMC 4700 for added reliability and fine control. This document provides a guide for
- DAVE Installation
- UART Communication
- LCD Display control 

### Dave Installation and first steps

In order to program the XMC 4700 you need to use the DEVE IDE. You can download it from the Infineon [website](https://infineoncommunity.com/dave-download_ID645); follow this link and submit your information to receive an email with the download link. Once downloaded, extract the ``DAVE-IDE-4.4.2-64Bit`` folder and create a desktop shortcut to the ``DAVE`` application located in the Eclipse folder. The program doesn't need an installation, just double click on the shortcut to launch it. 

In order to test and upload our programs on the microcontroller we need to install the SEGGER J-link debugger. Go to this [link](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack) and download the latest version of the software. During the installation make sure to also install the USB Driver for the debugger. 

Now we can create our first project in DAVE. Go to File -> New -> DAVE project... and enter the new of the project. Select DAVE CE Project and click Next. In the next window select your microcontroller (in our case XMC 4700 Relax kit) and tick "Add floating point support printf and scanf" at the bottom. Click Finish and you are done.

<p align="center">
  <img style="float: right;" src="https://i.imgur.com/xlVlGVJ.png" width="400">
</p>



