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
    jsonToTable(&json, ui->treeWidget);
}

void MainWindow::socketDisconnected()
{
    socket->deleteLater();
}

void MainWindow::jsonToTable(QJsonDocument* json, QTreeWidget* treeWidget)
{
    if (json->isObject())
    {
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

