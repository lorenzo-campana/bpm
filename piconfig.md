# Raspberry Pi configuration for EPICS control
This document contains an in depth documentation on how to setup a Raspberry pi 3b with the control software EPICS.

#### Raspberry pi setup
The first thing to do is download the operating system for the pi. The raspberry pi uses a modified version of Debian called *Raspbian*. For our application we're going to download the previos version of the OS called *Raspbian stretch*.

You can download the OS image [here](http://downloads.raspberrypi.org/raspbian/images/raspbian-2019-04-09/) (Download the ``2019-04-08-raspbian-stretch.zip`` file and extract the image file)

After downloading the OS image we need to burn it on a micro SD card (suggested size 16 GB). In order to do so download and install [Balena Etcher](https://www.balena.io/etcher/). The program is self explanatory: just run the program, select the ``2019-04-08-raspbian-stretch.iso`` image, select the micro SD card (**WARNING**: the SD card will be formatted) and press flash. 

Once the SD card is ready, we can start configuring the pi. First thing we need to do is insert the micro SD card in the its slot. 
After that we can plug in the pi a mouse and keybord, as well as the monitor through the HDMI port. 

The raspberry pi doesn't have a power button; to turn it on, jus plug the a micro USB cable using a proper power brick.

Once the pi is booted up, follow the instruction to complete the configuration. Once that finished the pi will restart, and you will be good to go!

#### *OPTIONAL*: set up a new user

If you want to install EPICS on a different user than the default one, you can create a new user ``custom_user`` with the following command:

```shell
sudo adduser custom_user
```

We then need to promote ``custom_user`` to superuser by editing the ``visudo`` file:

```shell
sudo visudo
```

After the line 

```shell
# User privilege specification
root    ALL=(ALL:ALL) ALL
```

add the following line:

```shell
custom_user    ALL=(ALL:ALL) ALL
```

After that exit pressing ``Ctrl + X`` and save the file pressing ``Y`` and then ``Enter``.


#### EPICS installation

To intsall EPICS the first thing to do is download [EPICS Base](https://epics.anl.gov/download/base/baseR3.14.12.7.tar.gz). We will use version ``3.14.12.7`` because that's the version of the software used in SPEAR3. Extract the archive in your home folder.

To compile ``EPICS Base`` we will use the ``make`` command:

```shell
cd
cd base-3.14.12.7
make
```

Now we need to install a java based graphic interface called [VisualDCT](https://github.com/epics-extensions/VisualDCT/releases). Download the [latest version](VisualDCT-2.8.1-distribution.tar.gz) and extrct it your home folder like ``EPICS Base``.

To open the graphic interface and visualize database, just type the following command:

```shell
cd 
cd VisualDCT-2.8.1
java -cp VisualDCT.jar com.cosylab.vdct.VisualDCT
```


