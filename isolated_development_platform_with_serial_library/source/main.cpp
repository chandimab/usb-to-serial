// USB2Serial Library Headers
#include "USB2SerialLib/testlib_usb_to_serial.h"
#include "USB2SerialLib/testlib_usb_to_serial.cpp"

#include <stdio.h>
#include <stdlib.h>
//#include <string>
#include <unistd.h>

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
            //     19200, // baud rate
            //     SERIAL_7E2//serial config 8E1
            // );

            printf("[serial port 0 was configured 9600 Baud, 8N1 ]\n");

            

            char msg[] = {'A','B','C'}; //6 chars

            printf("[serial transmission start.]\n");
            //send characters to serial 0

                 for(int i=0; i<3; i++){
                    usb_2_serial.serial_write(
                        0, //serial port
                        msg[i] //data
                    );
                 }

                  
/**
                    usb_2_serial.device_write_data_byte(
                        M_DATA_WRITE | (0<<5)
                    ); //token


                    usleep(100000);

                    usb_2_serial.device_write_data_byte(
                        msg[i]
                    ); //token

                    usleep(100000);

                    usb_2_serial.device_read_serial_byte(); //read ack

                    printf("%c", msg[i]);
                    
                    usleep(100000);
**/

//new

                    // usb_2_serial.device_write_data_byte(
                    //     M_DATA_WRITE | (0<<5)
                    // ); //token
                    
                    // usleep(100000);
                    
                    // usb_2_serial.device_write_data_byte(
                    //     msg[i]
                    // ); //token

                    // usleep(10000);

                    // usb_2_serial.device_read_serial_byte(); //read ack

                    // usleep(10000);

                 //}


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
