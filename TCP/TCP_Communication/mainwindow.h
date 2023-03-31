#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void ServerConnectFun();
    void ClientConnectFun();
    QString GetCurTime();

private slots:
    void server_New_Connect();// Server

    void Ser_socket_Read_Data();

    void Ser_socket_Disconnected();

    void cli_socket_Read_Data();// Client

    void cli_socket_Disconnected();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socketServer;
    QTcpSocket *socketClient;
    QTcpServer* server;
};
#endif // MAINWINDOW_H
