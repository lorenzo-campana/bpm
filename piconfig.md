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

#### ``OPTIONAL``: set up a new user

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
after this your visudo file will look like this:

![visudo](https://i.imgur.com/QEgffBb.png=250x250).


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

It's now time to create our application, that we will call ``my_app``. We will use a Pearl script that automatically create every file that we need. The script can generate varius type of EPICS application, we will use the ``ioc`` type. First we need to create a new folder to put the application file in:


```shell
cd
mkdir my_app
cd my_app
```

After that we can run the script with the following command:

```shell
../base-3.14.12.7/bin/linux-arm/makeBaseApp.pl -t ioc my_app
../base-3.14.12.7/bin/linux-arm/makeBaseApp.pl -i -t ioc my_app
```
When executing the second link the prompt will ask you what application the ioc should boot. Juest press ``Enter`` to set the default value and continue. 

After this we need to compile the application. As we did for the ``base`` folder, we will use the ``make command``. Navigate in the ``my_app`` folder and type in ``make``.

Now in the ``my_app`` folder we should have all the file that we need to set up a database.

#### Adding PATH to the .bashrc file
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

We now need to create the database that will store all the record that we need for our application. We can do this in two ways:
1. Using the VisualDCT tool 
2. Writing the ``.db`` file manually
a To initialize the database we will write the file ourself. In the future we will use VisualDCT to modify and add record to the database. 

The database files will be store in ``/my_app/my_appApp/Db``. Navigate in this folder and create a database file ``my_database.db`` using the ``nano`` command:

```shell
cd
cd my_app/my_appApp/Db
nano my_database.bd
```

Inside this file we will create a record (for this example we will create a ``sub`` record, a subroutine that can exectue ``C`` code. We will add the code in the next section).

```c
record(sub, my_sub) { 
    field(SNAM, my_subProcess)
    field(INAM, my_subInit)
}
```
Save and exit with ``Ctrl = X`` then ``Y`` and ``Enter``.

After creating it we need to add the new database in the ``Makefile``. In the same directory of the ``my_database.db`` file open the ``Makefile`` with ``nano``. Search for this line:

```
#DB += xxx.db
```

and add the following line of code just below that. 

```
DB += my_database.db
```
Save and exit with ``Ctrl = X`` then ``Y`` and ``Enter``. In this way the ``make`` command will know what database it need to use when compiling the application.

#### Subroutine records

In this section i will explain how a subroutine record works. A subroutine is a special epics record that can execute ``C`` code. This code will be stored in the ``/home/user/my_app/my_appApp/src`` folder. To create the source code for the subroutine go in the ``src`` folder and create a ``my_subroutine.c`` file using ``nano``. Inside the file write the following code:

```c
#include <stdio.h>
#include <subRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

static int my_subInit(subRecord *precord) {
     return 0;
}

static int my_subProcess(subRecord *precord) {
     return 0;
}

/* Note the function must be registered at the end!*/
epicsRegisterFunction(my_subInit);
epicsRegisterFunction(my_subProcess);
```
A subroutine record as two special attribute: 
1. INAM: is the name of the initialization function (``my_subInit`` in our case). This function is called only once when the database is loaded.
2. SNAM: is the name of the process function (``my_subProcess`` in our case). This function is called everytime the record gets processed. 
Inside this two function we can write any c code we want. If the subroutine record has some input links (we will se later how to create them) we can use them in our code using the pointer ``precord->a`` for the input A (INPA), and so on for the other input (INPB, INPC, etc).

to make the subroutine work we need to register it in EPICS. To do so create a new file called ``my_database.dbd`` in the same folder as the source code ``my_subroutine.c``. Open it with ``nano`` and write the following code:

```c
function(my_subInit)
function(my_subProcess)
```
 
Then exit and save the file.

After creating the record remember to compile it using ``make`` in the top folder of your application (``/home/user/my_app``).

#### Adding record to the database

Right now we only have a subroutine record in our database. In order to add other PV (the equivalent of variables for the database) we will use VisualDCT. Open the Java program (use the lines of code at the end of the **EPICS installation** chapter) and from the file browser open the database ``my_database.db`` located in ``/home/user/my_app/my_appApp/Db``. 

Inside the database we should have the subroutine records that we created in the previous chapter. In VisualDCT records appear as boxes with the type of record on the top, followed by the name of the record and its attribute in the lower part. 

