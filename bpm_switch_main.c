#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int mode_select=1; // manual mode -> 1 Automatic mode -> 0
int dwell_time=100*100; // time to wait before switching to another channel in automatic mode
int cycles=1; // number of time to go through the selected channels while switching
int channel_select=1; // channel selection for manual mode. Accept a number between 1 and 16
long int channel_mask=65535; // channel selection for automatic mode. Accept a number between 1 and 65535. To select the channels you want, convert a 16 bit binary number to decimal. eg 65535 = 1111111111111111 all channel selected
char error[40]; // string used for communicating back from the micro-controller to the pi
int global_delay=10*100; // time to wait after receiving the 10 Hz trigger before starting switching

int mask=0b1; // binary number used to scan the cahnnel_mask in auto mode

int i; // for loop indexes
int j;
int k;

int new_round=0; // flag for the first cycle of switching 


int8_t rx_buffer[64] = {0}; // array with the command read through the usb communication
char selection; // parameter 
int input; // parameter's value
uint8_t Bytes; // number of byte to be received through usb

// C union struct used to convert the decimal channel selection number in binary.
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

union data converter1; // converter for manual mode
union data converter2; // converter for auto mode

uint8_t mask_buffer[17] = {0}; // array with the number of each channel selected for auto mode

uint8_t cont_mask = 0; // counter to track how many channels have been cycled through in auto mode
uint8_t cont_cycles = 0; // counter to track how many cycles have been done in auto mode
int cont; // counter used for scanning channel_mask

int main(void)
{
    DAVE_STATUS_t init_status;
    init_status = DAVE_Init(); // DAVE APP initialization

    for(i=0; i<16;i++) mask_buffer[i]=i+1; // mask_buffer initialization. Default value -> all channel selected

    if (init_status == DAVE_STATUS_SUCCESS) // check if DAVE is initialized correctly
    {
        if(USBD_VCOM_Connect() != USBD_VCOM_STATUS_SUCCESS) // check if the usb connection is initialized correctly
        {
            return -1;
        }

        while(!USBD_VCOM_IsEnumDone()); // wait for the usb enumeration

        while(1U) // main loop
        {
            Bytes = USBD_VCOM_BytesReceived(); // count how many bytes are coming through the serial bus
            if (Bytes) // check if at least 1 byte has been sent
            {
                for (k=0; k<sizeof(rx_buffer);k++) rx_buffer[k]='\0'; // reset rx_buffer

                USBD_VCOM_ReceiveData(rx_buffer, Bytes); // receive the byte from the serial bus and write on rx_buffer

                selection = rx_buffer[0]; // save the parameter from the string received

                rx_buffer[0] = ' '; // delete the command to enable the conversion of the string to an integer 
                input = atoi((char*)rx_buffer); // save the parameter's value as an integer number


                switch (selection) { 

                    case 'D': // dwell time has come in

                        dwell_time = input*100; //DAVE timer parameter is scaled by a factor 100
                        sprintf(error, "dwell changed to %d us\n", dwell_time/100); // write into the error string what changed
                        break;

                    case 'T': // mode select has come in

                        mode_select = input;
                        if(mode_select==0){
                            PIN_INTERRUPT_Enable(&PIN_INTERRUPT_0); // enable the 10 hz interrupt
                            sprintf(error, "mode changed to AUTO\n");
                        }
                        else{
                            PIN_INTERRUPT_Disable(&PIN_INTERRUPT_0); // disable 10 hz interrupt
                            sprintf(error, "mode changed to MANUAL\n");
                        }
                        break;

                    case 'H': // channel select has come in

                        channel_select = input;
                        sprintf(error, "channel selected %d\n", channel_select);
                        if (mode_select==1) {

                            converter1.value=channel_select; // convert the channel_select to a binary variable

                            if(converter1.outputs.a==1) {DIGITAL_IO_SetOutputHigh(&D0);} // set the pin 
                            else {DIGITAL_IO_SetOutputLow(&D0);}

                            if(converter1.outputs.b==1){DIGITAL_IO_SetOutputHigh(&D1);}
                            else {DIGITAL_IO_SetOutputLow(&D1);}

                            if(converter1.outputs.c==1){DIGITAL_IO_SetOutputHigh(&D2);}
                            else {DIGITAL_IO_SetOutputLow(&D2);}

                            if(converter1.outputs.d==1){DIGITAL_IO_SetOutputHigh(&D3);}
                            else {DIGITAL_IO_SetOutputLow(&D3);}

                            if(converter1.outputs.e==1){DIGITAL_IO_SetOutputHigh(&D4);}
                            else {DIGITAL_IO_SetOutputLow(&D4);}
                        }
                        break;

                    case 'C': // cycles has come in

                        cycles = input;
                        sprintf(error, "cycles changed to %d\n", cycles);
                        break;

                    case 'M': // channel mask has come in
                        channel_mask = input;
                        sprintf(error, "mask changed to %ld\n",channel_mask);

                        cont=0;
                        for(i=0; i<16;i++) mask_buffer[i]=0; // reset mask_buffer

                        for(i=0;i<16;i++) // scan the channel_mask
                        { 
                            if((channel_mask&(mask<<i))>0) //read the channel_mask
                            {
                                mask_buffer[cont] = i+1; // save channel number in mask_buffer
                                cont ++;
                            }
                        }
                        break;

                    case 'Y':// delay has come in
                        global_delay = input*100;
                        sprintf(error, "delay changed to %d us\n",global_delay/100);
                        break;

                    case 'R': // A read command has come in
                        USBD_VCOM_SendByte('R');
                        USBD_VCOM_SendData((const int8_t *)&error[0], sizeof(error));
                        for (k=0; k<sizeof(error);k++) error[k]='\0';
                        break;
                    default:
                        break;
                }// end switch
                input=0; // reset parameter value
                CDC_Device_USBTask(&USBD_VCOM_cdc_interface); // function required for the correct functioning of the usb APP
            }// end if
        } // end loop
    }
} // end main 


void trigger_interrupt(void) //10 hz pin interrupt function 
{
    if (mode_select == 0) // check if you are in auto mode
    {
        if (TIMER_GetTimerStatus(&timer_global_delay)) TIMER_Stop(&timer_global_delay); // if the timers are running, it stops them  
        if (TIMER_GetTimerStatus(&timer_dwell_time)) TIMER_Stop(&timer_dwell_time); 

        cont_cycles = 0; // reset both counter
        cont_mask = 0;

        TIMER_SetTimeInterval(&timer_global_delay,global_delay); // update the time interval of the timers
        TIMER_SetTimeInterval(&timer_dwell_time,dwell_time);

        TIMER_Start(&timer_global_delay); // start the timer for the global delay
    }
}


void int_delay(void) // global delay timer interrupt function
{
    converter2.value = mask_buffer[cont_mask]; // set the first channel of mask_buffer

    if(converter2.outputs.a==1){DIGITAL_IO_SetOutputHigh(&D0);}
    else {DIGITAL_IO_SetOutputLow(&D0);}

    if(converter2.outputs.b==1){DIGITAL_IO_SetOutputHigh(&D1);}
    else {DIGITAL_IO_SetOutputLow(&D1);}

    if(converter2.outputs.c==1){DIGITAL_IO_SetOutputHigh(&D2);}
    else {DIGITAL_IO_SetOutputLow(&D2);}

    if(converter2.outputs.d==1){DIGITAL_IO_SetOutputHigh(&D3);}
    else {DIGITAL_IO_SetOutputLow(&D3);}

    if(converter2.outputs.e==1){DIGITAL_IO_SetOutputHigh(&D4);}
    else {DIGITAL_IO_SetOutputLow(&D4);}

    new_round = 1; // flag for the first cycle of switching

    TIMER_Start(&timer_dwell_time);  // start the dwell time timer
    TIMER_Stop(&timer_global_delay); // stop the global delay timer
}

void int_dwell(void)
{

    if (new_round == 1) // check if this is the first dwell time timer event after the global delay
    {
        cont_mask = 1; // skip the first channel of buffer_mask because it was already done in the global delay function 
        new_round = 0; // drop the flag 
    }

    if (mask_buffer[cont_mask] != 0) // check if there is a channel in mask_buffer
    {
        converter2.value=mask_buffer[cont_mask]; // output that channel

        if(converter2.outputs.a==1){DIGITAL_IO_SetOutputHigh(&D0);}
        else {DIGITAL_IO_SetOutputLow(&D0);}

        if(converter2.outputs.b==1){DIGITAL_IO_SetOutputHigh(&D1);}
        else {DIGITAL_IO_SetOutputLow(&D1);}

        if(converter2.outputs.c==1){DIGITAL_IO_SetOutputHigh(&D2);}
        else {DIGITAL_IO_SetOutputLow(&D2);}

        if(converter2.outputs.d==1){DIGITAL_IO_SetOutputHigh(&D3);}
        else {DIGITAL_IO_SetOutputLow(&D3);}

        if(converter2.outputs.e==1){DIGITAL_IO_SetOutputHigh(&D4);}
        else {DIGITAL_IO_SetOutputLow(&D4);}

        cont_mask++; // change channel for the next dwell timer event 
    }

    else // all channels in the mask_buffer have been done
    {

        cont_cycles++; // update the cycle counter

        if (cont_cycles != cycles) // check if this is the last cycle 
        {
            cont_mask = 0; // reset the mask counter
            converter2.value=mask_buffer[cont_mask]; // output the first channel of the next cycle

            if(converter2.outputs.a==1){DIGITAL_IO_SetOutputHigh(&D0);}
            else {DIGITAL_IO_SetOutputLow(&D0);}

            if(converter2.outputs.b==1){DIGITAL_IO_SetOutputHigh(&D1);}
            else {DIGITAL_IO_SetOutputLow(&D1);}

            if(converter2.outputs.c==1){DIGITAL_IO_SetOutputHigh(&D2);}
            else {DIGITAL_IO_SetOutputLow(&D2);}

            if(converter2.outputs.d==1){DIGITAL_IO_SetOutputHigh(&D3);}
            else {DIGITAL_IO_SetOutputLow(&D3);}

            if(converter2.outputs.e==1){DIGITAL_IO_SetOutputHigh(&D4);}
            else {DIGITAL_IO_SetOutputLow(&D4);}
        }

        else // this is the last cycle
        {
            DIGITAL_IO_SetOutputLow(&D0); // disconnect all pin 
            DIGITAL_IO_SetOutputLow(&D1);
            DIGITAL_IO_SetOutputLow(&D2);
            DIGITAL_IO_SetOutputLow(&D3);
            DIGITAL_IO_SetOutputLow(&D4);
            TIMER_Stop(&timer_dwell_time); // stop the dwell time timer
        }
        cont_mask++; // update the mask_buffer counter

    }
}