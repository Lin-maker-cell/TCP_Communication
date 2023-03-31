#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ServerWindow->setReadOnly(true);
    ui->ClientWindow->setReadOnly(true);
    ServerConnectFun();
    ClientConnectFun();
}

MainWindow::~MainWindow()
{
    server->close();
    server->deleteLater();
    delete this->socketClient;
    delete ui;
}
// 服务端操作
void MainWindow::ServerConnectFun()
{
    ui->ServerPort->setText("8888");
    ui->ServerSend->setEnabled(false);

    server = new QTcpServer();

    connect(server, &QTcpServer::newConnection, this, &MainWindow::server_New_Connect);

    connect(ui->ServerConnect, &QPushButton::clicked, this, [&](){
        if(ui->ServerConnect->text() == tr("侦听"))
        {
            // 从输入框获取端口号
            int port = ui->ServerPort->text().toInt();

            // 监听指定的端口
            if(!server->listen(QHostAddress::Any, port))
            {
                // 若出错，则输出错误信息
                qDebug() << server->errorString();
                return;
            }
            // 修改按键文字
            ui->ServerConnect->setText("取消侦听");
            ui->ServerPort->setDisabled(true);
            qDebug() << "Listen succeessfully!";
        }
        else
        {
            socketServer->abort();
            // 取消侦听
            server->close();
            // 修改按键文字
            ui->ServerConnect->setText("侦听");
            // 发送按键失能
            ui->ServerSend->setEnabled(false);
            ui->ServerPort->setDisabled(false);
        }
    });
    connect(ui->ServerSend, &QPushButton::clicked, this, [&](){
        if (!ui->ServerMsg->text().isEmpty()) {
            qDebug() << "Send: " << ui->ServerMsg->text();
            // 获取文本框内容并以 ASCII 码形式发送
            socketServer->write(ui->ServerMsg->text().toLatin1());
            socketServer->flush();
            ui->ServerWindow->appendPlainText(GetCurTime().append(" -> ").append(ui->ServerMsg->text()));
        }
    });
}

void MainWindow::server_New_Connect()
{
    // 获取客户端连接
    socketServer = server->nextPendingConnection();
    // 连接 QTcpSocket 的信号槽，以读取新数据
    QObject::connect(socketServer, &QTcpSocket::readyRead, this, &MainWindow::Ser_socket_Read_Data);
    QObject::connect(socketServer, &QTcpSocket::disconnected, this, &MainWindow::Ser_socket_Disconnected);
    // 发送按键使能
    ui->ServerSend->setEnabled(true);

    qDebug() << "A Client connect!";
}

void MainWindow::Ser_socket_Read_Data()
{
    QByteArray buffer;
    // 读取缓冲区数据
    buffer = socketServer->readAll();
    if(!buffer.isEmpty())
    {
        QString str = ui->ServerWindow->toPlainText();
        if (!str.isEmpty()) {
            str.append('\n');
        }
        str.append(GetCurTime()).append(" <- ");
        str+=tr(buffer);
        // 刷新显示
        ui->ServerWindow->setPlainText(str);
    }
}

void MainWindow::Ser_socket_Disconnected()
{
    // 发送按键失能
    ui->ServerSend->setEnabled(false);
    qDebug() << "Disconnected!";
}
// 客户端操作
void MainWindow::ClientConnectFun()
{
    socketClient = new QTcpSocket();// 创建套接字对象

    // 连接信号槽
    QObject::connect(socketClient, &QTcpSocket::readyRead, this, &MainWindow::cli_socket_Read_Data);
    QObject::connect(socketClient, &QTcpSocket::disconnected, this, &MainWindow::cli_socket_Disconnected);

    ui->ClientSend->setEnabled(false);
    ui->ClientIP->setText("127.0.0.1");
    ui->ClientPort->setText("8888");

    connect(ui->ClientConnect, &QPushButton::clicked, this, [&](){
        if(ui->ClientConnect->text() == tr("连接"))
        {
            QString IP;
            int port;

            // 获取 IP 地址
            IP = ui->ClientIP->text();
            // 获取端口号
            port = ui->ClientPort->text().toInt();

            // 取消已有的连接
            socketClient->abort();
            // 连接服务器
            socketClient->connectToHost(IP, port);

            // 等待连接成功
            if(!socketClient->waitForConnected(30000))
            {
                qDebug() << "Connection failed!";
                return;
            }
            qDebug() << "Connect successfully!";

            // 发送按键使能
            ui->ClientSend->setEnabled(true);
            // 修改按键文字
            ui->ClientConnect->setText("断开连接");
            ui->ClientIP->setDisabled(true);
            ui->ClientPort->setDisabled(true);
        }
        else
        {
            // 断开连接
            socketClient->disconnectFromHost();
            // 修改按键文字
            ui->ClientConnect->setText("连接");
            ui->ClientSend->setEnabled(false);
            ui->ClientIP->setDisabled(false);
            ui->ClientPort->setDisabled(false);
        }
    });
    connect(ui->ClientSend, &QPushButton::clicked, this, [&](){
        if (!ui->ClientMsg->text().isEmpty()) {
            qDebug() << "Send: " << ui->ClientMsg->text();
             // 获取文本框内容并以 ASCII 码形式发送
            socketClient->write(ui->ClientMsg->text().toLatin1());
            socketClient->flush();
            ui->ClientWindow->appendPlainText(GetCurTime().append(" -> ").append(ui->ClientMsg->text()));
        }
    });
}

QString MainWindow::GetCurTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();// 获取当前日期和时间
    QString currentDateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");// 转换为字符串格式
    currentDateTimeString.prepend('[');
    currentDateTimeString.append(']');
    return currentDateTimeString;
}

void MainWindow::cli_socket_Read_Data()
{
    QByteArray buffer;
    // 读取缓冲区数据
    buffer = socketClient->readAll();
    if(!buffer.isEmpty())
    {
        QString str = ui->ClientWindow->toPlainText();
        if (!str.isEmpty()) {
            str.append('\n');
        }
        str.append(GetCurTime().append(" <- "));
        str+=tr(buffer);
        // 刷新显示
        ui->ClientWindow->setPlainText(str);
    }
}

void MainWindow::cli_socket_Disconnected()
{
    // 发送按键失能
    ui->ClientSend->setEnabled(false);
    // 修改按键文字
    ui->ClientConnect->setText("连接");
    qDebug() << "Disconnected!";
}

