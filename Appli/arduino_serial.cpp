#include "arduino_serial.h"

RFIDReaderApp::RFIDReaderApp(QWidget *parent)
    : QMainWindow(parent), scanning(false)
{
    serialPort = new QSerialPort(this);
    serialPort->setPortName("/dev/ttyACM0"); // Remplacez par le port série de votre Arduino
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->open(QIODevice::ReadWrite);
}
