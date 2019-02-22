// USB2Serial Library Headers
#include "USB2SerialLib/testlib_usb_to_serial.h"
#include "USB2SerialLib/testlib_usb_to_serial.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>

int error(){
    printf("exitting the program.\n");
    return 1;
}

int main(){

    // creating an object
    TestLib_usb_to_serial usb_2_serial;

    printf("[Program started. identifying the device]\n");
    //cout <<"program started."<<endl;
    if(usb_2_serial.device_identify()){
        printf("[device is availabe at PORT %s]\n", usb_2_serial.get_device_port());

        //connect to the device
        if(usb_2_serial.device_open()){
            printf("[device is opened and configured.]\n");

            //configure serial 0
            // usb_2_serial.set_config(
            //     0, //serial port 0
            //     9600, // baud rate
            //     SERIAL_8N1//serial config 8N1
            // );

            printf("[serial port 0 was configured 9600 Baud, 8N1 ]\n");

            char msg[] = {'h','e','l','l','o','\n'}; //6 chars

            printf("[serial transmission start.]\n");
            //send characters to serial 0
            for(int k=0; k<5; k++){
                for(int i=0; i<6; i++){
                    usb_2_serial.serial_write(
                        0, //serial port
                        msg[i] //data
                    );
                    printf("%c", msg[i]);
                }
                 //delay 1000ms

            }

            printf("[close device connection.]\n");
            //closing device
            usb_2_serial.device_close();
            
            printf("[exitting program.]\n");
            return 1;

        }else{
            printf("[can't opened and configure the device.]\n");
            error();
        }

    }else{
        printf("[device is not availabe]\n");
        error();
    }

    return 0;
}
