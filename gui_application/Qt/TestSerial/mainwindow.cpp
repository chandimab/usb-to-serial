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
    //ui->label_header->setText("USB to Serial");


    arduino = new QSerialPort(this);
    serialBuffer = "";
    parsed_data = "";


    // update USB list
//    identify_connected_port_of_arduino();
//    if(arduino_is_available){

//        ui->label_device_port->setText(": [Available] "+arduino_port_name);
//    }
//    else{
//        ui->label_device_port->setText(": [Not Available]");
//    }
    refresh_usb();

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
    //menu item
    connect(
                ui->actionReload_USB_Interface_List, //menu item reference
                SIGNAL(triggered()),
                this,
                SLOT(refresh_usb())
    );

    //set button , serial read
    connect(
                ui->pushButton_set,
                SIGNAL(released()),
                this,
                SLOT(arduino_configure_port())
    );

    //default button , serial read
    connect(
                ui->pushButton_default,
                SIGNAL(released()),
                this,
                SLOT(send_msg_test())
    );

    // load saved configurations (from the arduino)
    // if failed, load default configurations

    // connect/disconnect to the arduino on connect/disconnect button press
    // enable/disable appropriate ui elements

    // serial communication class

}


MainWindow::~MainWindow(){
    delete ui;
}

/** cleaned arudino functions **/
bool MainWindow::arduino_configure_port(){
    /*
     *  Open and configure the arduino port if available
     */
    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);

        /** when arduino:SIGNAL(readyRead()) ---> SLOT(readSerial()):this **/
        QObject::connect(
                    arduino,
                    SIGNAL(readyRead()),
                    this,
                    SLOT(arduino_slot_serial_read())
        );

        return true;
    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
        return false;
    }
}


void MainWindow::arduino_slot_serial_read(){
    qDebug()<<"[serial read]\n";


}

void MainWindow::arduino_serial_write(char *data){
    arduino->write(data);
    arduino->flush(); //considering time critical things
    qDebug()<<"[serial write] "<<data<<"\n";
}

bool MainWindow::arduino_close(){
    arduino->close();
    qDebug()<<"[arduino close]\n";
}

void MainWindow::send_msg(){
    open_and_configure_port_test();

    qDebug() << "writeSerial_test(): start\n";
    serialData = arduino->readAll();
    arduino->write("A");
    arduino->flush();
    qDebug() << "writeSerial_test(): A\n";
}

void MainWindow::refresh_usb(){
    qDebug() << "RefreshUsb()...\n";
    identify_connected_port_of_arduino();
    if(arduino_is_available){

        ui->label_device_port->setText(": [Available] "+arduino_port_name);
    }
    else{
        ui->label_device_port->setText(": [Not Available]");
    }
}

/** this function returns following information about the connected port

        bool *arduino_is_available,
        QString *arduino_port_name,

**/
void MainWindow::identify_connected_port_of_arduino(){

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
            }
        }
    }

}

void MainWindow::open_and_configure_port_test(){

    /*
     *  Open and configure the arduino port if available
     */
    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);

        /** when arduino:SIGNAL(readyRead()) ---> SLOT(readSerial()):this **/
        QObject::connect(
                    arduino,
                    SIGNAL(readyRead()),
                    this,
                    SLOT(readSerial_test())
        );

    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }

}

void MainWindow::open_and_configure_port(
        bool arduino_is_available,
        QString arduino_port_name
        ){

    /*
     *  Open and configure the arduino port if available
     */
    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }

}

QString MainWindow::buid_conf_string(){
    QString temp="";
    /** build the conf string from Ui inputs **/

    return  temp;
}

void MainWindow::readSerial_test(){
    qDebug() << "readSerial_test(): start\n";
    serialData = arduino->readAll();
    qDebug() << "readSerial_test(): " << serialData <<"\n";
}

void MainWindow::readSerial(){

    int temperature_value; //just to avoid the error
    /*
     * readyRead() doesn't guarantee that the entire message will be received all at once.
     * The message can arrive split into parts.  Need to buffer the serial data and then parse for the temperature value.
     *
     */
    QStringList buffer_split = serialBuffer.split(","); //  split the serialBuffer string, parsing with ',' as the separator

    //  Check to see if there less than 3 tokens in buffer_split.
    //  If there are at least 3 then this means there were 2 commas,
    //  means there is a parsed temperature value as the second token (between 2 commas)
    if(buffer_split.length() < 3){
        // no parsed value yet so continue accumulating bytes from serial in the buffer.
        serialData = arduino->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        // the second element of buffer_split is parsed correctly, update the temperature value on temp_lcdNumber
        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        parsed_data = buffer_split[1];
        temperature_value = (9/5.0) * (parsed_data.toDouble()) + 32; // convert to fahrenheit
        qDebug() << "Temperature: " << temperature_value << "\n";
        parsed_data = QString::number(temperature_value, 'g', 4); // format precision of temperature_value to 4 digits or fewer
        //Dialog::updateTemperature(parsed_data);
    }

}
