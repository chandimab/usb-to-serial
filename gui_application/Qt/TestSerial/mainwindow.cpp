#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QFile>
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QtSerialPort/QSerialPort>
//#include <QtSerialPort/QSerialPortInfo>
//#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>



#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QDialog>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // setting initial values
    arduino = new QSerialPort(this);
    serialBuffer = "";
    parsed_data = "";

    // scan for devices
    device_identify();

    if(arduino_is_available){
        ui->label_device_port->setText("Device Status : [Available] on PORT "+ arduino_port_name);
        // open and configure the device, if available
        device_open_and_configure();

        // load/read configurations from the device, and store in a datastructure
        device_read_config();

        // display loaded configurations
        display_config();

    }else{
        ui->label_device_port->setText("Device Status : [Unavailable]");
    }

    /** GUI SIGNALS -> SLOTS **/

    //menu item : [Device Rescan]
    connect(
                ui->actionReload_USB_Interface_List, //menu item reference
                SIGNAL(triggered()),
                this,
                SLOT(device_rescan()) //perform this
    );

    //menu item : [Disconnect]
    connect(
                ui->actionDisconnect, //menu item reference
                SIGNAL(triggered()),
                this,
                SLOT(device_close_connection()) //perform this
    );

    //button : [Set]
    connect(
                ui->pushButton_set,
                SIGNAL(released()),
                this,
                SLOT(set_config()) //set the current config (for the selected single serial port)
    );

    //button : [Default]
    connect(
                ui->pushButton_default,
                SIGNAL(released()),
                this,
                SLOT(default_config())
    );



    /**
        //Button click event test - working
        connect(
                    ui->pushButton_refresh,
                    SIGNAL(released()),
                    this,
                    SLOT(refresh_usb())
        );
    **/

    /*
        (object1 :signal1) ----->(slot2: object2)
        connect(
            object1,
            signal1,
            object2,
            slot2
        );

        connect(
                    ui->actionReload_USB_Interface_List, //menu item reference
                    SIGNAL(triggered()),
                    this,
                    SLOT(refresh_usb())
        );

    */

}


MainWindow::~MainWindow(){
    delete ui;
}

/**
  References for further developments:
  https://doc.qt.io/qt-5/qtserialport-examples.html
  **/

// main functions

/** this function returns following information about the connected port
        bool *arduino_is_available,
        QString *arduino_port_name,
**/
void MainWindow::device_identify(){
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
void MainWindow::device_open_and_configure(){

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
        QObject::connect(
                    arduino,
                    SIGNAL(readyRead()),
                    this,
                    SLOT(device_on_serial_read())
        );


    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }
}

//returns config for all 8 serial ports
void MainWindow::device_read_config(){

}

//write config for only the given port
void MainWindow::device_write_config(int serial_port_id){

}

void MainWindow::device_close_connection(){
    arduino->close();
    qDebug()<<"[arduino close]\n";
}

//helper functions

// slot to be executed on serial read
void MainWindow::device_on_serial_read(){
    qDebug()<<"[serial read]\n";

    /**
    qDebug() << "readSerial_test(): start\n";
    serialData = arduino->readAll();
    qDebug() << "readSerial_test(): " << serialData <<"\n";
      **/
}

void MainWindow::device_write_data(char *data){
    arduino->write(data);
    arduino->flush(); //considering time critical things
    qDebug()<<"[serial write] "<<data<<"\n";
}

//GUI slots

// identify and connect
void MainWindow::device_rescan(){
    qDebug() << "RefreshUsb()...\n";

    //identify
    device_identify();

    if(arduino_is_available){
        ui->label_device_port->setText("Device Status : [Available] on PORT "+ arduino_port_name);
        //open and configure
        device_open_and_configure();

    }else{
        ui->label_device_port->setText("Device Status : [Unavailable]");
    }


}
void MainWindow::display_config(){

}
void MainWindow::set_config(){

}
void MainWindow::default_config(){

}

int MainWindow::get_config_number(){

}
