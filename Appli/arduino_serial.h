#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <QtWidgets>
#include <QtSerialPort>
#include <QtSql>

class RFIDReaderApp : public QMainWindow
{
    Q_OBJECT

public:
    RFIDReaderApp(QWidget *parent = nullptr);

private slots:
    void onScanButtonClicked();
    void onStopButtonClicked();
    void onSerialDataReceived();

private:
    QPushButton *scanButton;
    QSqlDatabase db;
    QPushButton *stopButton;
    QTextEdit *uidDisplay;
    QSerialPort *serialPort;
    QString ilayizy;

    bool scanning; // Indicateur de scan en cours
};


#endif // ARDUINO_SERIAL_H
