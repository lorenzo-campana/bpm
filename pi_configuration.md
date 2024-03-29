
# Raspberry Pi configuration for EPICS control
This document contains an in depth documentation on how to setup a Raspberry pi 3b with the control software EPICS.

### Raspberry pi setup
The first thing to do is download the operating system for the pi. The raspberry pi uses a modified version of Debian called *Raspbian*. For our application we're going to download the previos version of the OS called *Raspbian stretch*.

You can download the OS image [here](http://downloads.raspberrypi.org/raspbian/images/raspbian-2019-04-09/) (Download the ``2019-04-08-raspbian-stretch.zip`` file and extract the image file)

After downloading the OS image we need to burn it on a micro SD card (suggested size 16 GB). In order to do so download and install [Balena Etcher](https://www.balena.io/etcher/). The program is self explanatory: just run the program, select the ``2019-04-08-raspbian-stretch.iso`` image, select the micro SD card (**WARNING**: the SD card will be formatted) and press flash. 

Once the SD card is ready, we can start configuring the pi. First thing we need to do is insert the micro SD card in the its slot. 
After that we can plug in the pi a mouse and keybord, as well as the monitor through the HDMI port. 

The raspberry pi doesn't have a power button; to turn it on, jus plug the a micro USB cable using a proper power brick.

Once the pi is booted up, follow the instruction to complete the configuration. Once that finished the pi will restart, and you will be good to go!

### ``OPTIONAL``: set up a new user

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

<p align="center">
  <img style="float: right;" src="https://i.imgur.com/QEgffBb.png" width="400">
</p>

After that exit pressing ``Ctrl + X`` and save the file pressing ``Y`` and then ``Enter``.


### EPICS installation

To intsall EPICS the first thing to do is download [EPICS Base](https://epics.anl.gov/download/base/baseR3.14.12.7.tar.gz). We will use version ``3.14.12.7`` because that's the version of the software used in SPEAR3. Extract the archive in your home folder.

To compile ``EPICS Base`` we will use the ``make`` command:

```shell
cd ~/base-3.14.12.7
make
```

Now we need to install a java based graphic interface called [VisualDCT](https://github.com/epics-extensions/VisualDCT/releases). Download the [latest version](VisualDCT-2.8.1-distribution.tar.gz) and extrct it your home folder like ``EPICS Base``.

To open the graphic interface and visualize database, just type the following command:

```shell
cd ~/VisualDCT-2.8.1
java -cp VisualDCT.jar com.cosylab.vdct.VisualDCT
```

### Set up a soft link

In order to acces our ``base`` folder more easily we can set up a soft link with the following command:

```shell
ln -s /home/user/base-3.14.12.7/ /home/user/epics
```
In this way we can acces our ``base-3.14.12.7`` folder just by writing ``cd epics``

### Create a EPICS application

It's now time to create our application, that we will call ``my_app``. We will use a Pearl script that automatically create every file that we need. The script can generate varius type of EPICS application, we will use the ``ioc`` type. First we need to create a new folder to put the application file in:


```shell
mkdir ~/my_app
cd my_app
```

After that we can run the script with the following command:

```shell
~/base-3.14.12.7/bin/linux-arm/makeBaseApp.pl -t ioc my_app
~/base-3.14.12.7/bin/linux-arm/makeBaseApp.pl -i -t ioc my_app
```
When executing the second link the prompt will ask you what application the ioc should boot. Juest press ``Enter`` to set the default value and continue. 

After this we need to compile the application. As we did for the ``base`` folder, we will use the ``make command``. Navigate in the ``my_app`` folder and type in ``make``.

Now in the ``my_app`` folder we should have all the file that we need to set up a database.

### Adding PATH to the .bashrc file
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

### Create a new database

We now need to create the database that will store all the record that we need for our application. We can do this in two ways:
1. Using the VisualDCT tool 
2. Writing the ``.db`` file manually
a To initialize the database we will write the file ourself. In the future we will use VisualDCT to modify and add record to the database. 

The database files will be store in ``/my_app/my_appApp/Db``. Navigate in this folder and create a database file ``my_database.db`` using the ``nano`` command:


```shell

cd ~/my_app/my_appApp/Db
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

### Subroutine records


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

Now we need to edit the ``Makefile`` inside thr ``src`` folder with the new file we just created. Open it with ``nano`` and under the comment 
```c
#my_app.dbd will be made up from these files:
my_app_DBD += base.dbd
```

add the following two line of code :


```c
my_app_DBD += my_database.dbd
my_app_SRCS += my_subroutine.c
```
Then exit and save the file.

The last thing we need to edit is the ``st.cmd`` file located in ``/home/pi/my_app/iocBoot/iocmy_app``. Open it with ``nano`` and add the following line of code under ``## Load record instances``:
 
```
dbLoadRecords("db/my_database.db")
```

After creating the record remember to compile it using ``make`` in the top folder of your application (``/home/user/my_app``).

### Adding record to the database

:Right now we only have a subroutine record in our database. In order to add other PV (the equivalent of variables for the database) we will use VisualDCT. Open the Java program (use the lines of code at the end of the **EPICS installation** chapter) and from the file browser open the ``.dbd`` file ``my_app.db`` located in ``/home/user/my_app/dbd``. After this you should see a black windows with white dots. Now open the database file with file -> Open... and in the file browser open the ``my_database.db`` file located in ``/home/user/my_app/my_appApp/Db``. 

Inside the database we should have the subroutine records that we created in the previous chapter. In VisualDCT records appear as boxes with the type of record on the top, followed by the name of the record and its attribute in the lower part. 

After opening the database you shold have a window like this:

<p align="center">
  <img style="float: right;" src="https://i.imgur.com/IPP9u30.png" width="400">
</p>

From this window you can create new record just by right clicking anywhere in the black dotted window and then clicking "New record...". On the window that pops up you can select the tipe of record and the name. You can edit the record's attributes by double clicking on it. 

We can now link a record to our subroutine. Create an analog output (ao) record called ``my_record`` and then right click on the subroutine record. Under INLINK select INPA and then click on the analog output that we just created. This will link the two record, and the value of ``my_record`` will be avaible to use in the subroutine calling ``precord->a``.

We want to process the subroutine each time we update the value stored in the ``my_record`` record, so we need a FLNK. To create this type of link just right click on the analog output record, select FLNK and click on the subroutine. In order to setup the right processing scheme, double click on the output record and then change the fiel SCAN to "Passive". Then right click on the INPA of the subroutine and under "Process" set "PP - Process passive".
 

### IOC boot
 
Now we have EPICS installed with a functioning database we can start the ioc. Go to the folder ``/home/pi/my_app/iocBoot/iocmy_app`` and run the following command to start the ioc:

```shell
../../bin/linux-arm/my_app st.cmd
```

While the ioc is running open another terminal. From there you can access the varius record inside the database we loaded. In order to do so use the following command:
- ``caget my_record`` to read the value inside the record
- ``caput my_record value`` to write a value inside the record
- ``camonitor my_record`` to start a realtime monitor of the value inside the record that is updated each time the record is processed


### Adding libraries to your epics application: wiringPi

To control the GPIO pin of the pi through epics we need to install an externa library called WiringPi. To install the library you just need to edit the ``Makefile`` located in ``/home/pi/my_app/my_appApp/src`` and add the following line at the end of the file:

 ```
 my_app_LIBS += wiringPi
 ```
 
 Remember to always ``make``  the application after you commit ny change.
 
 With this library we can use the following command while programming our subroutine in C:
 
 - ``#include <wiringPi.h>`` to include the library in the code
 - ``wiringPiSetup()`` to initialise the library. this function must be called at the start of the programm
 - ``pinMode(int pin, int mode)`` to set up one of the pin. The available mode are ``INPUT`` or output ``OUTPUT``. You must specify the wiringPi pin number and not the one written on the header. [Here](https://projects.drogon.net/raspberry-pi/wiringpi/pins/) you can find the right pin number.
 
- ``digitalWrite(int pin, int value)`` to write ``HIGH`` (1) or ``LOW`` (0) to an output pin.
- ``wiringPiISR(int pin, int edgeType, &my_interrupt)`` to create an interrupt. You can select the type of signal to trigger the interrupt with the edgeType parameter (option are ``INT_EDGE_FALLING``, ``INT_EDGE_RISING``). When an interrupt signal comes in the pin selected, the program will execute the ``my_interrupt`` function. This function *must* be ``void`` without any arguments:

```C
void my_interrupt()
```


### Arduino integration

If we want the pi to comunicate with the arduino we will need two additional libraries: ASYNC and StreamDevice. With this two piece of software we will be able to setup a serial comunication between the epics ioc on the pi and the arduino. This is usefull if you need to run a real time program on a more stable system; The pi will handle the IOC and will send the arduino the parameter, while the arduino will execute the program.

Let-s install the libraries. Enter in the ``~/Downloads`` folder and the download the files with this two command:

```shell
wget http://aps.anl.gov/epics/download/modules/asyn4-28.tar.gz
wget http://epics.web.psi.ch/software/streamdevice/StreamDevice-2.tgz
```
After that go in your EPICS base (``~/base-3.14.12.7``) folder and create a new folder called  ``modules``:

```
cd ~/base
mkdir modules
```
The first library that we will install is ASYNC, because it-s required for StreamDevice. Extract the library and create a soft link to the folder with this two command:

```shell
tar -zxf ~/Downloads/asyn4-28.tar.gz -C ~/base-3.14.12.7/modules/
ln -s /home/pi/base-3.14.12.7/modules/asyn4-28 /home/pi/base-3.14.12.7/modules/asyn
```

After that enter the async folder with ``cd ~/base-3.14.12.7/modules/async`` and edit the ``RELEASE`` file located in the ``configure`` folder using ``nano`` as always.
-Comment out the IPAC line
-Comment out the SNCSEQ line
-Comment out the EPICS_BASE line and add the following line ``EPICS_BASE=/home/pi/base-3.14.12.7``
After that in ``~/base-3.14.12.7/modules/async`` run ``make``.

When this completes, the system is ready for installation of the Stream Device library. Create a new folder inside the modules and then extract there the libraray that we downloaded. after that run the ``makeBaseApp.pl`` script, with these commands:

```shell
mkdir /home/pi/base-3.14.12.7/modules/stream
cd /home/pi/base-3.14.12.7/modules/stream
tar -zxvf ~/Downloads/StreamDevice-2.tgz -C  /home/pi/base-3.14.12.7/modules/stream
makeBaseApp.pl -t support
```

The last command will ask you the application name; just press ``Enter`` to continue.

In order to let StreamDevice knows where ASYNC is, add the following line of code in the ``RELEASE`` file located in the configure folder:

```shell
ASYN=/home/pi/base-3.14.12.7/modules/asyn
```

To finish the installation run ``make`` in ``~/base-3.14.12.7/modules/stream``. Once this completes, ``cd`` into ``StreamDevice-2-6``, and run ``make`` again.

### Serial communication between Pi and Arduino
The first thing to do is to tell our epics application ehere are the two libraries that we just installed. To do so, edit the ``RELEASE`` file located in ``~/my_app/configure/`` and add the following lines of code:

```
ASYN=/home/pi/base-3.14.12.7/modules/asyn
STREAM=/home/pi/base-3.14.12.7/modules/stream
```

After that we need to create a ``my_protocol.proto`` file that will contain the instruction for epics on what to send to the arduino via serial comunication. Create this file using ``nano`` in the Db folder of your application (``~/my_app/my_appApp/Db/``):

```
Terminator = LF;
set_parameter {
    out "P%d\n";
    ExtraInput = Ignore;
}

get_value {
    out "R";
    in "R %s";
    ExtraInput = Ignore;
}
```
The first line tells EPICS how to terminate strings, and this matches the expectations of the Arduino Serial code. The function ``set_parameter`` tells epics to write something (in our case the letter "P" followed by an integer and a new line) in the serial bus, while the ``get_value`` tells epics to expect something (in our case the letter "R" followed by a string) from the serial bus. We will use this information later, in the arduino code. 

Now we need a new database record that will store the parameter that we want to send to the arduino. Open ``my_database.db`` using Visual DCT and create a new analog output (ao) record called ``my_param``. Double click on it to open the properties and edit the DTYP to "stream" and the OUT to ``@my_protocol.proto set_parameter() $(PORT)``. Everytime this record gets processed, the ``set_parameter()`` function defined in the proto file will be executed, with its output oing in the seial port indicated (in this case ``$(PORT)``, we will set the right port in the next step).

Create a second analog input record called ``my_value``. Double click on it to open the properties and edit the DTYP to "stream" and the INP to ``@my_protocol.proto get_value() $(PORT)``

Now we need to modify the Makefile. Move to ``~/my_app/my_appApp/src`` and edit the Makefile you find in there using ``nano``. After the line that says, ``myapp_DBD += base.dbd``, add the following lines:

```
my_app_DBD += asyn.dbd
my_app_DBD += stream.dbd
my_app_DBD += drvAsynIPPort.dbd
my_app_DBD += drvAsynSerialPort.dbd
```
and near the end of the file, after the line ``#  ADD EXTRA GNUMAKE RULES BELOW HERE`` add:

```
my_app_LIBS += asyn
my_app_LIBS += stream
```

Now move to ``~/my_app/iocBoot/iocmy_app`` and edit the ``st.cmd`` file. After ``> envPaths`` (near the top), add the following

```
epicsEnvSet(STREAM_PROTOCOL_PATH,"../../my_appApp/Db")
```

After the line, ” my_app_registerRecordDeviceDriver pdbbase”, add the following


```
drvAsynSerialPortConfigure("SERIALPORT","/dev/ttyACM0",0,0,0)
asynSetOption("SERIALPORT",-1,"baud","115200")
asynSetOption("SERIALPORT",-1,"bits","8")
asynSetOption("SERIALPORT",-1,"parity","none")
asynSetOption("SERIALPORT",-1,"stop","1")
asynSetOption("SERIALPORT",-1,"clocal","Y")
asynSetOption("SERIALPORT",-1,"crtscts","N")
dbLoadRecords("db/my_database.db","PORT='SERIALPORT'")
```

These lines configure a serial connection called “SERIALPORT”, linked to /dev/ttyACM0 (the arduino should be connected to this port, but check if it's true), and then set all the options so that the serial communications works in the way that the Arduino expects. Now that this connection has been defined, it can be used in place of the $(PORT) variable referred to in the database definition file.

In this same file (``st.cmd``), change any references to $(TOP) to ``/home/pi/my_app``, and any references to $(IOC) to ``iocmy_app``.

Finally go to ``~/my_app`` and ``make``.

Now we need to write the code that will go inside the arduino. The following code is an example:

```c
String input = "";
int parameter;

void setup() {
 Serial.begin(115200); 
 }
 
void loop () {

 while (Serial.available()>0){
   char lastRecvd = Serial.read();
   if (lastRecvd == '\n') { 
     switch (input[0]) {
       case 'P': // A write command has come in
         input = input.substring(1,input.length());
         parameter=input.toInt();
         input = "";
         break;
       case 'R': // A read command has come in
         Serial.print("something"); 
         input = "";
         break;
       default:
         break;
     }
   }
   else { // Input is still coming in
     input += lastRecvd;
   }
 }
}
 
```
In the ``setup()`` we setup the serial comunication. In the ``loop()`` the arduino scans the serial and record when something comes through. If it gets the letter "P" he will set the variable ``parameter`` to what it received from the serial. If it receives a "R" it will write something to the serial bus. 

This code, together with the proto file, connects the two records that we created to the serial, allowing the arduino to read and write them. 

### Arduino + Pi 16 channel multiplexer controller
In this github repository you can find all the file necessary to create an EPICS application that can comunicate to an arduino in order to control a minicircuits [USB / TTL RF SP16T Switch](https://www.minicircuits.com/pdfs/USB-1SP16T-83H.pdf), a 16 channel multiplexer in two different modes:
- Manual mode: you can set the channel that you want to output
- Automatic mode: you can select one or more channels and the arduino will continuously switch between them, after it detects a trigger signal. you can set how long a channel is selected as output (dwell time), how many times you want to switch between the selected channels (cycles), and the delay from the trigger signal.

The file provided are:
- the ``.proto`` [file](serial_communication.proto) for the serial comunication 
- the [sketch](bpm_switch.ino) for the arduino
