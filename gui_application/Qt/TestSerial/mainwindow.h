#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

//for gui
#include <QPushButton>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots: //functions
        /*
         * refresh usb
         */

        void refresh_usb();

        /*
         *  identify and get information about the connected port of arduino (in pc)
         */
        void identify_connected_port_of_arduino();

        /*
         *  Open and configure the arduino port if available
         */
        void open_and_configure_port(
                bool arduino_is_available,
                QString arduino_port_name
        );

        void open_and_configure_port_test();

        /** build conf string from Ui inputs **/
        QString buid_conf_string();

        void readSerial();
        void readSerial_test();
        void send_msg();

        /** cleaned arduino **/
        bool arduino_configure_port();
        void arduino_slot_serial_read();
        void arduino_serial_write(char *data);
        bool arduino_close();


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

        /** GUI related **/
//        QPushButton
//            *btn_set,
//            *btn_default,
//            *btn_refresh;



};

#endif // MAINWINDOW_H
