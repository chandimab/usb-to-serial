#include "testlib_usb_to_serial.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QDebug>

TestLib_usb_to_serial::TestLib_usb_to_serial(){
}

/** this function returns following information about the connected port
        bool *arduino_is_available,
        QString *arduino_port_name,
**/
void TestLib_usb_to_serial::device_identify(){
    arduino_is_available = false; //setting to false, before scanning

    //  For each available serial port
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        //  check if the serialport has both a product identifier and a vendor identifier
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
            //  check if the product ID and the vendor ID match those of the arduino uno
            if((serialPortInfo.productIdentifier() == arduino_product_id)
                    && (serialPortInfo.vendorIdentifier() == arduino_vendor_id)){
                arduino_is_available = true; //    arduino uno is available on this port
                arduino_port_name = serialPortInfo.portName();

                break;
            }
        }
    }
}

/*
 *  Open and configure the arduino port if available
 */
void TestLib_usb_to_serial::device_open_and_configure(){

    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);

        /** when arduino:SIGNAL(readyRead()) ---> SLOT(device_on_serial_read()):this **/
//        connect(
//                    arduino,
//                    SIGNAL(readyRead()),
//                    this,
//                    SLOT(device_on_serial_read())
//        );

        qDebug() << "Configured successfully.\n";
    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        //QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }
}

//returns config for serial port
unsigned char TestLib_usb_to_serial::device_read_config(unsigned char serial_port){

}

//write config for only the given port
void TestLib_usb_to_serial::device_write_config(int serial_port, unsigned char conf){

}

void TestLib_usb_to_serial::device_close_connection(){

    if(arduino->isOpen()){
        arduino->close();
        qDebug()<<"[arduino close]";
    }
}

//helper functions

// slot to be executed on serial read
void TestLib_usb_to_serial::device_on_serial_read(){
    if(lock == false){
          unsigned char token = device_read_serial_byte() & 0b00011111; //token byte
          if(token == M_FROM_DEVICE_DATA_SERIAL){
              //serial input
              unsigned char serial = (token >>5);
              unsigned char data = device_read_serial_byte();

              qDebug() << "[device_on_serial_read] : serial "<<(int)(serial)<<" data: "<<data;
          }
    }else{
        //ignore
    }
}

char TestLib_usb_to_serial::device_read_serial_byte(){ //should be a blocking???
    if(arduino->waitForReadyRead(50)){
        QByteArray r = arduino->read(1);
        qDebug() << "[serial read:byte]: " << r[0];
        return r[0];
    }else{
        return -1;
    }
}


//void TestLib_usb_to_serial::device_write_data(QString data){

//    arduino->write(data.toLatin1());
//    arduino->flush(); //considering time critical things
//    qDebug()<<"[serial write] "<<data.toLatin1();
//}

void TestLib_usb_to_serial::device_write_data_byte(char data){
    QByteArray bData; bData.resize(1); //1 byte
    bData[0] =  data;

    arduino->write(bData);
    arduino->flush(); //considering time critical things
    qDebug()<<"[serial write] "<<data;
}


unsigned char TestLib_usb_to_serial::get_config_number(
        int baud_rate,
        int serial_conf
        ){

    //baud rate [7:5]
    int BAUD[]={4800, 9600, 14400, 19200, 38400, 57600, 115200};
    unsigned char baud = 0;
    for(baud = 0; baud <8; baud++)
        if(BAUD[baud] == baud_rate) break;

    if(baud>=8){ //valid
        return 0xFF;
    }

    baud = baud<<5;

    //data, parity,stop
    //[5-8], [0,1,2], [1,2]
    unsigned char
            s = (unsigned char)serial_conf%10 - 1,
            p = ((unsigned char)serial_conf%100 - s )/10,
            d = (unsigned char)(serial_conf/100) - 5;

    //configs
    //baud,data, parity,stop
    //[7:5][4:3][2:1][0]
    unsigned char data_[] = {0, (1<<3), (1<<4), (1<<3 | 1<<4)};
    unsigned char parity_[] = {0, (1<<1), (1<<2), (1<<1 | 1<<2)};
    unsigned char stopbits_[] = {0, 1};


    return (
        baud_rate |

        data_[d]  |
        parity_[p]  |
        stopbits_[s]
    );

}



//GUI slots


void TestLib_usb_to_serial::read_config(
            unsigned char serial_selected,
            int *baud_rate,
            int *serial_conf
        ){

    int BAUD[]={4800, 9600, 14400, 19200, 38400, 57600, 115200};

    if(arduino_is_available && device_setup){
        lock = true;

        //get config information from the device
        device_write_data_byte(
                    M_CONF_READ | //command
                    (serial_selected << 5) //specifies to which serial
        );

        //device_read_serial_byte(); //token
        char conf = device_read_serial_byte(); //read byte

        if(conf == -1){ //error on reading
            qDebug()<<"errorOnReading";
        }
        else if((conf & 0b11100000) == 0b11100000){ //no saved configs, default
            qDebug()<<"defaultConfig";
            *baud_rate =9600;
            *serial_conf = SERIAL_8N1; //8N1
        }
        else{
            //configs
            //baud,data, parity,stop
            //[7:5][4:3][2:1][0]
            qDebug()<<"fromSavedData";

            *baud_rate = BAUD[(conf & 0b11100000) >> 5];  //baud rate


            *serial_conf =
               (((conf & 0b00011000) >> 3) + 5)*100 + //data
               ((conf & 0b00000110) >> 1)*10 +//parity
                (conf & 0b00000001)+1 ;//stopbit

        }


        lock = false;

    }

}


void TestLib_usb_to_serial::set_config(
        unsigned char serial_selected,
        int baud_rate,
        int serial_conf
        ){


    lock = true;

    //sending command
    device_write_data_byte(
                M_CONF_WRITE | //command
                (serial_selected << 5) //specifies to which serial

    ); //write byte

    //this is unsigned 8bit. the 8th bit could be 1. if converted to char, it will give different meaning??
    unsigned config_number = get_config_number(//geting the generated config data to be written
                baud_rate,
                serial_conf
    );

    //sending config data
    device_write_data_byte(
        config_number
    ); //write byte

    device_read_serial_byte(); //ack

    lock = false;

}

unsigned char TestLib_usb_to_serial::default_config(){

    unsigned char baud = 1<<5; //9600

    //8N1
    //data, parity,stop
    //[5-8], [0,1,2], [1,2]

    //configs
    //baud,data, parity,stop
    //[7:5][4:3][2:1][0]
    unsigned char data_[] = {0, (1<<3), (1<<4), (1<<3 | 1<<4)};
    unsigned char parity_[] = {0, (1<<1), (1<<2), (1<<1 | 1<<2)};
    unsigned char stopbits_[] = {0, 1};


    return (
        baud |

        (1<<3 | 1<<4) // | //data
        //0  | //parity
        //0 //stopbits
    );
}




