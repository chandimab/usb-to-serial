#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

//for gui
#include <QPushButton>


//communication
#define M_CONF_WRITE    0b00000001
#define M_CONF_READ     0b00000010
#define M_DATA_READ     0b00000100
#define M_DATA_WRITE    0b00001000


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
        void device_read_config(); //returns config for all 8 serial ports
        void device_write_config(int serial_port_id); //write config for only the given port
        void device_close_connection();
        //helper functions
        void device_on_serial_read(); // slot to be executed on serial read
        void device_write_data(QString data);
        void device_write_data_byte(char data);
        char get_config_number();
        char device_read_serial_byte();

        void device_is_ready_for_transmission();

        //GUI slots
        void device_rescan(); // identify and connect
        void display_config();
        void set_config();
        void default_config();

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
            serialBuffer,
            parsed_data,
            arduino_port_name;


        bool arduino_is_available;

        QByteArray send_data;

        /** GUI related **/
//        QPushButton
//            *btn_set,
//            *btn_default,
//            *btn_refresh;



};

#endif // MAINWINDOW_H
