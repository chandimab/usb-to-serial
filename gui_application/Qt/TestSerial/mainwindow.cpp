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
        ui->label_device_status->setText(": [Available]");
        ui->label_device_port->setText(": "+ arduino_port_name);
        // open and configure the device, if available
        device_open_and_configure();

        // load/read configurations from the device, and store in a datastructure
        device_read_config();

        // display loaded configurations
        display_config();

    }else{
        ui->label_device_status->setText(": [Unavailable]");
        ui->label_device_port->setText(": - ");
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
//        QObject::connect(
//                    arduino,
//                    SIGNAL(readyRead()),
//                    this,
//                    SLOT(device_on_serial_read())
//        );

        qDebug() << "Configured successfully.\n";
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
    qDebug()<<"[arduino close]";
}

//helper functions

// slot to be executed on serial read
void MainWindow::device_on_serial_read(){

//    qDebug() << "[serial read]: " << QString::fromLatin1(arduino->readLine());
    //qDebug() << "[serial read]: " << arduino->readAll();

    //qDebug() << "[serial read]: " << arduino->read(1);


}

char MainWindow::device_read_serial_byte(){ //should be a blocking???
    if(arduino->waitForReadyRead(50)){
        QByteArray r = arduino->read(1);
        qDebug() << "[serial read:byte]: " << r;
        return r[0];
    }else{
        return -1;
    }
}


void MainWindow::device_write_data(QString data){

    arduino->write(data.toLatin1());
    arduino->flush(); //considering time critical things
    qDebug()<<"[serial write] "<<data;
}

void MainWindow::device_write_data_byte(char data){
    QByteArray bData; bData.resize(1); //1 byte
    bData[0] =  data;

    arduino->write(bData);
    arduino->flush(); //considering time critical things
    qDebug()<<"[serial write] "<<data;
}


//GUI slots

// identify and connect
void MainWindow::device_rescan(){
    qDebug() << "RefreshUsb()...";

    //identify
    device_identify();

    if(arduino_is_available){
        ui->label_device_status->setText(": [Available]");
        ui->label_device_port->setText(": "+ arduino_port_name);
        //open and configure
        device_open_and_configure();

    }else{
        ui->label_device_status->setText(": [Unavailable]");
        ui->label_device_port->setText(": - ");
    }


}
void MainWindow::display_config(){

}
void MainWindow::set_config(){


    //*send_data = M_CONF_WRITE;
    //device_write_data(&send_data);
    //send_data = "1";
    //device_write_data(send_data);
    unsigned char send[] = {
        M_CONF_WRITE,
        0xF1, //data

        M_CONF_READ,

        M_DATA_READ,

        M_DATA_WRITE,
        0xF1,

        M_DATA_WRITE+1
    };

        device_write_data_byte(send[0]); //write byte
        device_write_data_byte(send[1]); //write byte
        device_read_serial_byte(); //read byte

        device_write_data_byte(send[2]); //write byte
        device_read_serial_byte(); //read byte

        device_write_data_byte(send[3]); //write byte
        device_read_serial_byte(); //read byte

        device_write_data_byte(send[4]); //write byte
        device_write_data_byte(send[5]); //write byte
        device_read_serial_byte(); //read byte

        //else
        device_write_data_byte(send[6]); //write byte
        device_read_serial_byte(); //read byte


}
void MainWindow::default_config(){

}

char MainWindow::get_config_number(){
    /**
    int data_[]={500,600,700,800};
    int parity_[]={0, 10, 20};
    int stopbits_[] = {1, 2};
    // databits[5-8] parity[n-0,odd-1, even 2] stopbits[1-2]
    return (
        data_[ui->comboBox_databits->currentIndex()]+
        parity_[ui->comboBox_parity->currentIndex()]+
        stopbits_[ui->comboBox_stop_bits->currentIndex()]
    );

    **/

    //how to send them in one byte
    //[4:3][2:1][0]
    char data_[] = {0, (1<<3), (1<<4), (1<<3 | 1<<4)};
    char parity_[] = {0, (1<<1), (1<<2), (1<<1 | 1<<2)};
    char stopbits_[] = {1, 2};

    return (
        data_[ui->comboBox_databits->currentIndex()] |
        parity_[ui->comboBox_parity->currentIndex()] |
        stopbits_[ui->comboBox_stop_bits->currentIndex()]
    );
}

//this function will block until the device is ready for transmission.
void MainWindow::device_is_ready_for_transmission(){
    //for(int i=0;i<10;i++) device_read_serial_byte();
}
