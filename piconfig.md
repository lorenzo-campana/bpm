# Raspberry Pi configuration for EPICS control
This document contains an in depth documentation on how to setup a Raspberry pi 3b with the control software EPICS.

####Raspberry pi setup
The first thing to do is download the operating system for the pi. The raspberry pi uses a modified version of Debian called *Raspbian*. For our application we're going to download the previos version of the OS called *Raspbian stretch*.

You can download the OS image [here](http://downloads.raspberrypi.org/raspbian/images/raspbian-2019-04-09/) (Download the ``2019-04-08-raspbian-stretch.zip`` file and extract the image file)

After downloading the OS image we need to burn it on a micro SD card (suggested size 16 GB). In order to do so download and install [Balena Etcher](https://www.balena.io/etcher/). The program is self explanatory: just run the program, select the ``2019-04-08-raspbian-stretch.iso`` image, select the micro SD card (**WARNING**: the SD card will be formatted) and press flash. 

Once the SD card is ready, we can start configuring the pi. First thing we need to do is insert the micro SD card in the its slot. 
After that we can plug in the pi a mouse and keybord, as well as the monitor through the HDMI port. 

The raspberry pi doesn't have a power button; to turn it on, jus plug the a micro USB cable using a proper power brick.

Once the pi is booted up, follow the instruction to complete the configuration. Once that finished the pi will restart, and you will be good to go!

####``OPTIONAL``: set up a new user

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

#### Set up a soft link

In order to acces our ``base`` folder more easily we can set up a soft link with the following command:

```shell
ln -s /home/user/base-3.14.12.7/ /home/user/epics
```
In this way we can acces our ``base-3.14.12.7`` folder just by writing ``cd epics``

#### Create a EPICS application

It's now time to create our application, that we will call ``my_app``. We will use a Pearl script that automatically create every file that we need. The script can generate varius type of EPICS application, we will use the ``example`` type. First we need to create a new folder to put the application file in:


```shell
cd
mkdir my_app
cd my_app
```

After that we can run the script with the following command:

```shell
epics/bin/linux-arm/makeBaseApp.pl -t example my_app
epics/bin/linux-arm/makeBaseApp.pl -i -t example my_app
```

Now in the ``my_app`` folder we should have all the file that we need to set up a database.

The last thing we need to do is add some ``PATH`` in the ``.bashrc`` file. This file contains the commands that are executed when you open a new terminal. **WARNING:** the ``.bashrc`` file contains critical information for the terminal. If you mess up this file, the terminal won't work anymore. For this reason before editing it create a backup with this commands:

```shell
cd 
cp .bashrc bashrc_backup
```

We will edit the ``.bashrc`` file using the ``nano`` command:

```shell
nano .bashrc
 
```

Finally scroll to the bottom of the file and add the following code as the last line:

```shell
PATH=/home/user/base-3.14.12.7/bin/linux-arm:$PATH

export EPICS_BASE=/home/user/base-3.14.12.7/lib/linux-arm/libca.so
export EPICS+HOST_ARCH=linux-arm

```

Save and close the file pressing ``Ctrl + X`` then ``Y`` and ``Enter``

#### Create a new database


