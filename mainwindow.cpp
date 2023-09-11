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
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneMinute()));
    timer->start(60000);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (socket->state() != QAbstractSocket::ConnectedState)
    {
        socket->connectToHost("127.0.0.1", 5555);
    }
    else
    {
        ui->label_2->setText("Ошибка: already connected");
    }
}

void MainWindow::socketReady()
{
    QJsonDocument json = QJsonDocument::fromJson(socket->readAll());
    jsonToTable(&json, ui->treeWidget);
}

void MainWindow::socketDisconnected()
{
    qDebug() << "disconnected";
    socket->deleteLater();
}

void MainWindow::socketStateChanged(QAbstractSocket::SocketState state)
{
    QString s;
    switch (state)
    {
        case QAbstractSocket::UnconnectedState: s = "Сокет не подключён"; break;
        case QAbstractSocket::HostLookupState: s = "Сокет выполняет поиск имени хоста"; break;
        case QAbstractSocket::ConnectingState: s = "Сокет начал устанавливать соединение"; break;
        case QAbstractSocket::ConnectedState: s = "Соединение установлено"; break;
        case QAbstractSocket::BoundState: s = "Сокет привязан к адресу и порту"; break;
        case QAbstractSocket::ListeningState: s = "Только для внутреннего использования"; break;
        case QAbstractSocket::ClosingState: s = "Сокет вот-вот закроется (возможно, данные все еще ожидают записи)"; break;
    }

    ui->label->setText((QString)"Статус: " + s);
    ui->label_2->setText((socket->error() == QAbstractSocket::UnknownSocketError) ? "" : (QString)"Ошибка: " + socket->errorString());
    if (state == QAbstractSocket::ConnectedState)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject.insert("info", "dai");
        json.setObject(jsonObject);
        socket->write(json.toJson(QJsonDocument::Compact));
    }
}

void MainWindow::oneMinute()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject.insert("info", "dai");
        json.setObject(jsonObject);
        socket->write(json.toJson(QJsonDocument::Compact));
    }
}

void MainWindow::jsonToTable(QJsonDocument* json, QTreeWidget* treeWidget)
{
    if (json->isObject())
    {
        treeWidget->clear();
        QJsonObject jsonObject = json->object();
        QJsonArray jsonArray = jsonObject.value("block").toArray();
        for (int i = 0; i < jsonArray.count(); i++)
        {
            QJsonObject jsonObject2 = jsonArray.at(i).toObject();
            QTreeWidgetItem* item = new QTreeWidgetItem;

            item->setText(0, jsonObject2.value("id").toString());
            item->setText(1, "block");
            QJsonArray jsonArray2 = jsonObject.value("board").toArray();
            for (int j = 0; j < jsonArray2.count(); j++)
            {
                QJsonObject jsonObject3 = jsonArray2.at(j).toObject();

                if (jsonObject3.value("parent id").toString() == jsonObject2.value("id").toString())
                {
                    QTreeWidgetItem* itemChild = new QTreeWidgetItem;
                    itemChild->setText(0, jsonObject3.value("id").toString());
                    itemChild->setText(1, "board");
                    QJsonArray jsonArray3 = jsonObject.value("port").toArray();
                    for (int k = 0; k < jsonArray3.count(); k++)
                    {
                        QJsonObject jsonObject4 = jsonArray3.at(k).toObject();

                        if (jsonObject4.value("parent id").toString() == jsonObject3.value("id").toString())
                        {
                            QTreeWidgetItem* itemChild2 = new QTreeWidgetItem;
                            itemChild2->setText(0, jsonObject4.value("id").toString());
                            itemChild2->setText(1, "port");
                            itemChild->addChild(itemChild2);
                        }
                    }
                    item->addChild(itemChild);
                }
            }
            treeWidget->addTopLevelItem(item);
        }
    }
}

