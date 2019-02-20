#ifndef TESTLIB_USB_TO_SERIAL_H
#define TESTLIB_USB_TO_SERIAL_H

#include "testlib_global.h"
#include <QtSerialPort/QSerialPort>


//communication
#define M_CONF_WRITE    0b00000001
#define M_CONF_READ     0b00000010
#define M_DATA_READ     0b00000100
#define M_DATA_WRITE    0b00001000

#define M_FROM_DEVICE_CONF_WRITE_ACK    0b00010001
#define M_FROM_DEVICE_CONF_READ         0b00010010
#define M_FROM_DEVICE_DATA_READ         0b00010100
#define M_FROM_DEVICE_DATA_WRITE_ACK    0b00011000

#define M_FROM_DEVICE_DATA_SERIAL       0b00011111


#define SERIAL_5N1 501
#define SERIAL_6N1 601
#define SERIAL_7N1 701
#define SERIAL_8N1 801

#define SERIAL_5N2 502
#define SERIAL_6N2 602
#define SERIAL_7N2 702
#define SERIAL_8N2 802

#define SERIAL_5O1 511
#define SERIAL_6O1 611
#define SERIAL_7O1 711
#define SERIAL_8O1 811

#define SERIAL_5O2 512
#define SERIAL_6O2 612
#define SERIAL_7O2 712
#define SERIAL_8O2 812

#define SERIAL_5E1 521
#define SERIAL_6E1 621
#define SERIAL_7E1 721
#define SERIAL_8E1 821

#define SERIAL_5E2 522
#define SERIAL_6E2 622
#define SERIAL_7E2 722
#define SERIAL_8E2 822


class TESTLIBSHARED_EXPORT TestLib_usb_to_serial
{

public:
    TestLib_usb_to_serial();


    /** PROJECT**/
    // main functions
    void device_identify();
    void device_open_and_configure();
    unsigned char device_read_config(unsigned char serial_port); //returns config for serial port
    void device_write_config(int serial_port, unsigned char conf); //write config for only the given port
    void device_close_connection();

    //helper functions
    void device_on_serial_read(); // slot to be executed on serial read
    //void device_write_data(QString data);
    void device_write_data_byte(char data);
    unsigned char get_config_number(
            int baud_rate,
            int serial_conf
    );

    char device_read_serial_byte();

    //GUI slots

    void read_config(
            unsigned char serial_selected,
            int *baud_rate,
            int *serial_conf
    );
    void set_config(
            unsigned char serial_selected,
            int baud_rate,
            int serial_conf
            );
    unsigned char default_config();

private:
    //Serial Port Related
    QSerialPort *arduino;
    //working
    static const quint16
        arduino_vendor_id = 0x1a86,
        arduino_product_id = 0x7523;

    QByteArray serialData;

    QString
        arduino_port_name;


    bool
        device_setup = false,
        arduino_is_available =false,
        lock = false;

    //QByteArray send_data;

};

#endif // TESTLIB_USB_TO_SERIAL_H
