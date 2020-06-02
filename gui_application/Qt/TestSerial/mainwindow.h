#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

//for gui
#include <QPushButton>


//token flags
#define M_CONF_WRITE    0b00000001
#define M_CONF_READ     0b00000010
#define M_DATA_READ     0b00000100
#define M_DATA_WRITE    0b00001000

#define M_FROM_DEVICE_CONF_WRITE_ACK    0b00010001
#define M_FROM_DEVICE_CONF_READ         0b00010010
#define M_FROM_DEVICE_DATA_READ         0b00010100
#define M_FROM_DEVICE_DATA_WRITE_ACK    0b00011000

#define M_FROM_DEVICE_DATA_SERIAL       0b00011111


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots: //functions
        /** build conf string from Ui inputs **/
 //       QString buid_conf_string();



        /** PROJECT**/
        // main functions
        void device_identify();
        void device_open_and_configure();
        //void device_read_config(); //returns config for all 8 serial ports
        //void device_write_config(int serial_port_id); //write config for only the given port
        void device_close_connection();
        //helper functions
        //void device_on_serial_read(); // slot to be executed on serial read
        //void device_write_data(QString data);
        void device_write_data_byte(char data);
        unsigned char get_config_number();
        char device_read_serial_byte();

        //GUI slots
        void device_rescan(); // identify and connect
        void display_config();
        void display_config(unsigned char serial_selected);
        void set_config();
        void default_config();


        void quit_program();


        //newly added, for testing purpose
//        void test_serial_7();


    private:
        Ui::MainWindow *ui;

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

#endif // MAINWINDOW_H
