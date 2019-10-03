// This Arduino sketch is used to control a minicircuit USB / TTL RF SP16T Switch, a 16 channel multiplexer. The parameter are set through serial comunication between the arduino and a Raspberry Pi with installed EPICS, the ASYNC and StreamRecord libraries. 
//The program features two different operation modes: 
//-- MANUAL: this mode lets you decide which channel of the 16 of the multiplexer you want as output. 
// -- AUTO: this mode lets ypu select one or more channel of the multiplexer. The arduino will continuously switch between them, after it detects a trigger signal. you can set how long a channel is selected as output (dwell time), how many times you want to switch between the selected channels (cycles), and the delay from the trigger signal.

#include <avr/interrupt.h>  
#include <stdio.h>
#include <string.h>

const int D0 = 13; // define the pin for the output signal
const int D1 = 12;
const int D2 = 11;
const int D3 = 10;
const int D4 = 9;

const int INT_PIN = 2 ; // define the pin for the interrupt

String input = ""; // variable for the serial input commands

// declaration of the global variable that will be used as parameter

int mode_select=1; // manual mode -> 1 Automatic mode -> 0
long int dwell_time=100; // time that the arduino waits before switching to anoter channel in automatic mode
long int cycles=10; // number of time the arduino will go through the selected channels while switching
int channel_select=1; // channel selection for manual mode. Accept a number between 1 and 16
long int channel_mask=65535; // channel selection for automatic mode. Accept a number between 1 and 65535. To select the channels you want, convert a 16 bit binary number to decimal. eg 65535 = 1111111111111111 all channel selected
char error[40]; // string used for comunicating back from the uno to the pi
int global_delay=10; // time that the arduino waits after receiving the 10 trigger before starting switching

int mask=0b1; // binary number used to scan the cahnnel_mask in auto mode
int i; // for loop indexes
int j;

int volatile trigger=0; // variable that activate automatic mode when it is selected and a 10 hz trigger comes

// C union struct used to convert the decimal channel selction number in binary.
union data {
    struct{
        unsigned int a:1;
        unsigned int b:1;
        unsigned int c:1;
        unsigned int d:1;
        unsigned int e:1;
    }outputs;
    unsigned char value;
};

union data converter1;
union data converter2;


void setup() {
    pinMode(D0, OUTPUT); //setup for te outup pins
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);

    pinMode(INT_PIN, INPUT_PULLUP); //setup for the trigger input pin
    
    attachInterrupt(digitalPinToInterrupt(INT_PIN), interrupt10hz, FALLING); //define the trigger pin, the trigger edge and the function to be executed when the trigger comes
    Serial.begin(9600); // Make sure the Serial commands match this baud rate
}

void loop () {
    while (Serial.available()>0){ //detect if something comes through the serial
        char lastRecvd = Serial.read(); //save the command in a variable
        
        if (lastRecvd == '\n') { // The end of the command has arrived
            switch (input[0]) {

                case 'D':// Dwell time has come in
                    input = input.substring(1,input.length()); 
                    dwell_time = input.toInt(); // convert the dwell time into a int
                    sprintf(error, "dwell_changed_to_%d_us\n", dwell_time); // write into the error string what changed
                    input = ""; // reset the input
                    break;

                case 'T':// Mode select has come in
                    input = input.substring(1,input.length());
                    mode_select = input.toInt();
                    if(mode_select==0) 
                        sprintf(error, "mode_changed_to_AUTO\n");
                    else 
                        sprintf(error, "mode_changed_to_MANUAL\n");
                    input = "";
                    break;

                case 'H':// channel select has come in

                    input = input.substring(1,input.length());
                    channel_select = input.toInt();
                    sprintf(error, "chennel_selected_%d\n", channel_select);
                    if (mode_select==1) { 

                        converter1.value=channel_select; // convert the channel_select to a binary variable
                        digitalWrite(D0, converter1.outputs.a); // set the pins according to the channel selected
                        digitalWrite(D1, converter1.outputs.b);
                        digitalWrite(D2, converter1.outputs.c);
                        digitalWrite(D3, converter1.outputs.d);
                        digitalWrite(D4, converter1.outputs.e);
                    }

                    input = "";
                    break;

                case 'C':// cycles has come in 
                    input = input.substring(1,input.length());
                    cycles = input.toInt();
                    sprintf(error, "cycles_changed_to_%d\n", cycles);
                    input = "";
                    break;

                case 'M':// channel mask has come in
                    input = input.substring(1,input.length());
                    channel_mask = input.toInt();
                    sprintf(error, "mask_changed_to_%ld\n",channel_mask);
                    input = "";
                    break;

                case 'Y':// delay has come in
                    input = input.substring(1,input.length());
                    global_delay = input.toInt();
                    sprintf(error, "delay_changed_to_%d_us\n",global_delay);
                    input = "";
                    break;

                case 'R': // A read command has come in
                    Serial.print("R "); 
                    Serial.print(error); //send the error string through the serial
                    Serial.print("\n");
                    input = "";
                    break;
                default:
                    input= "";
                    break;
            }// end switch
        }// end if

        else { // Input is still coming in
            input += lastRecvd;
        }
    }


    if(mode_select==0 & trigger==1){
        delayMicroseconds(global_delay); // delay from the 10 hz
        trigger=0; // reset the trigger variable
        for(j=0;j<cycles; j++){ // for loop for the cycles

            for(i=0;i<16;i++){ // for loop for the channel
                if((channel_mask&(mask<<i))>0){ //read the channel_mask

                    converter2.value=i+1;

                    digitalWrite(D0,converter2.outputs.a); // set the pins
                    digitalWrite(D1,converter2.outputs.b);
                    digitalWrite(D2,converter2.outputs.c);
                    digitalWrite(D3,converter2.outputs.d);
                    digitalWrite(D4,converter2.outputs.e);
                    delayMicroseconds(dwell_time); // dwell time
                }
            }
            digitalWrite(D0,LOW); // at the end of the cycle all pin are set to LOW
            digitalWrite(D1,LOW);
            digitalWrite(D2,LOW);
            digitalWrite(D3,LOW);
            digitalWrite(D4,LOW);
        }
    }
}


void interrupt10hz() {
    trigger=1; 
}
