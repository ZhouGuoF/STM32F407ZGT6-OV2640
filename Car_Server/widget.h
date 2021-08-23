#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QImage>
#include <QHostAddress>


#include "qmqtt.h"



namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();



private slots:

    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_btnClear_clicked();

    void on_newConnection();

    void onConnected();

    void onDisConnected();

    void onStateChanged(QAbstractSocket::SocketState socketState);

    void onReadyRead();

    void on_btnSend_clicked();


 /*-------MQTT-------------------*/
private slots:
    void on_btn_connect_clicked();

    void on_btn_publish_clicked();

    void on_btn_subscribe_clicked();

    void on_btn_unsubscrible_clicked();

    void on_btn_clear_clicked();

    void on_btn_disconnect_clicked();

    void car_publish(QByteArray(statusarray));

private slots:
    void doprocess_connected();
    void doprocess_subscribled(QString,quint8);
    void doprocess_receivemessage(QMQTT::Message message);
    void doprocess_unsubscrbled(QString);
    void doprocess_mqtterror(QMQTT::ClientError);
    void doprocess_disconnected();





private:
    Ui::Widget *ui;

    QTcpServer *m_tcpServer = nullptr;
    QTcpSocket *m_tcpSocket = nullptr;

    QMQTT::Client *client;


};

#endif // WIDGET_H
