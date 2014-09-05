#include <QFileDialog>
#include <QDebug>

#include "serialconnconfigwidget.h"
#include "ui_serialconnconfigwidget.h"

SerialConnConfigWidget::SerialConnConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialConnConfigWidget)
{
    ui->setupUi(this);

    ui->btRefreshDeviceList->setIcon(QIcon(":/icons/icon-refresh"));
    ui->btConnect->setIcon(QIcon(":/icons/icon-connect"));

    connect(ui->btRefreshDeviceList, &QPushButton::clicked, this, &SerialConnConfigWidget::btRefreshDeviceClicked);
    connect(ui->btConnect, &QPushButton::clicked, this, &SerialConnConfigWidget::btConnectToDeviceClicked);

    // Load the combobox for the first time in the instatiation.
    refreshDeviceList();

    // Fill all other connection options to the user in the window.
    populateFields();
}

SerialConnConfigWidget::~SerialConnConfigWidget()
{
    delete ui;
}

void SerialConnConfigWidget::refreshDeviceList()
{
    ui->cbDeviceList->clear();
    ui->cbDeviceList->addItems(SerialCommunication::instance()->availablePorts());
}

void SerialConnConfigWidget::connectToDevice()
{
    if(SerialCommunication::instance()->connectToDevice(ui->cbDeviceList->currentText(),
                                              (QIODevice::OpenModeFlag)ui->cbOpenType->currentData().toInt(),
                                              (QSerialPort::BaudRate)ui->cbBaudRate->currentData().toInt(),
                                              (QSerialPort::DataBits)ui->cbDataBits->currentData().toInt(),
                                              (QSerialPort::StopBits)ui->cbStopBits->currentData().toInt(),
                                              (QSerialPort::Parity)ui->cbParity->currentData().toInt()
                                              )){

        //Notify MainWindow the connection is ready.
        emit serialCommunicationReady();

    }
}

void SerialConnConfigWidget::populateFields()
{
    ui->cbOpenType->addItem(tr("Read/Write"), QIODevice::ReadWrite);
    ui->cbOpenType->addItem(tr("Read Only"),QIODevice::ReadOnly);
    ui->cbOpenType->addItem(tr("Write Only"),QIODevice::WriteOnly);

    ui->cbBaudRate->addItem("9600",QSerialPort::Baud9600);
    ui->cbBaudRate->addItem("1200",QSerialPort::Baud1200);
    ui->cbBaudRate->addItem("2400",QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("4800",QSerialPort::Baud4800);
    ui->cbBaudRate->addItem("19200",QSerialPort::Baud19200);
    ui->cbBaudRate->addItem("38400",QSerialPort::Baud38400);
    ui->cbBaudRate->addItem("57600",QSerialPort::Baud57600);
    ui->cbBaudRate->addItem("115200",QSerialPort::Baud115200);

    ui->cbDataBits->addItem("8",QSerialPort::Data8);
    ui->cbDataBits->addItem("6",QSerialPort::Data6);
    ui->cbDataBits->addItem("7",QSerialPort::Data7);
    ui->cbDataBits->addItem("5",QSerialPort::Data5);

    ui->cbStopBits->addItem("1", QSerialPort::OneStop);
    ui->cbStopBits->addItem("2", QSerialPort::TwoStop);

    ui->cbParity->addItem("None", QSerialPort::NoParity);
    ui->cbParity->addItem("Odd", QSerialPort::OddParity);
    ui->cbParity->addItem("Mark", QSerialPort::MarkParity);
    ui->cbParity->addItem("Space", QSerialPort::SpaceParity);
    ui->cbParity->addItem("Even", QSerialPort::EvenParity);
}


void SerialConnConfigWidget::btRefreshDeviceClicked()
{
    refreshDeviceList();
}

void SerialConnConfigWidget::btConnectToDeviceClicked()
{
    connectToDevice();
}

