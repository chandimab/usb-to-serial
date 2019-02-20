#define Include_Path /mnt/LinuxOpt/qt_test/CommanLineReaderSync/



#include <QtCore/QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication coreApplication(argc, argv);
    const int argumentCount = QCoreApplication::arguments().size();
    const QStringList argumentList = QCoreApplication::arguments();

    QTextStream standardOutput(stdout);

    if (argumentCount == 1) {
        standardOutput << QObject::tr("Usage: %1 <serialportname> [baudrate]")
                          .arg(argumentList.first()) << endl;
        return 1;
    }

    QSerialPort serialPort;
    const QString serialPortName = argumentList.at(1);
    serialPort.setPortName(serialPortName);

    const int serialPortBaudRate = (argumentCount > 2)
            ? argumentList.at(2).toInt() : QSerialPort::Baud9600;
    serialPort.setBaudRate(serialPortBaudRate);

    if (!serialPort.open(QIODevice::ReadOnly)) {
        standardOutput << QObject::tr("Failed to open port %1, error: %2")
                          .arg(serialPortName).arg(serialPort.error()) << endl;
        return 1;
    }

    QByteArray readData = serialPort.readAll();
    while (serialPort.waitForReadyRead(5000))
        readData.append(serialPort.readAll());

    if (serialPort.error() == QSerialPort::ReadError) {
        standardOutput << QObject::tr("Failed to read from port %1, error: %2")
                          .arg(serialPortName).arg(serialPort.errorString()) << endl;
        return 1;
    } else if (serialPort.error() == QSerialPort::TimeoutError && readData.isEmpty()) {
        standardOutput << QObject::tr("No data was currently available"
                                      " for reading from port %1")
                          .arg(serialPortName) << endl;
        return 0;
    }

    standardOutput << QObject::tr("Data successfully received from port %1")
                      .arg(serialPortName) << endl;
    standardOutput << readData << endl;

    return 0;
}
