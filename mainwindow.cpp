#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1", 5555);
    if (socket->isOpen())
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject.insert("info", "dai");
        json.setObject(jsonObject);
        socket->write(json.toJson(QJsonDocument::Compact));
    }
}

void MainWindow::socketReady()
{
    QJsonDocument json = QJsonDocument::fromJson(socket->readAll());
    QJsonObject jsonObject;


    qDebug() << json;
}

void MainWindow::socketDisconnected()
{
    socket->deleteLater();
}

