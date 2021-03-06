#include "widget.h"
#include "ui_widget.h"
#include <QHostInfo>
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <vector>
#include <QVector>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QTimer>

using std::vector;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
using namespace cv;


cv::Mat show_img(Mat& showimg){
    if(!showimg.empty()){
        namedWindow("binary", WINDOW_AUTOSIZE );
        imshow("binary", showimg);
        // waitKey(1000);
    }
}

cv::Mat region_of_interest(Mat& srcImg)
{

    Mat dstImg;
    Mat mask = Mat::zeros(srcImg.size(), CV_8UC1);
    int img_width = srcImg.size().width;
    int img_height = srcImg.size().height;

    vector<vector<Point>> contour;
    vector<Point> pts;
    pts.push_back(Point(img_width * 0.45, img_height * 0.6));
    pts.push_back(Point(img_width * 0.55, img_height * 0.6));
    pts.push_back(Point(img_width, img_height));
    pts.push_back(Point(img_width, img_height));
    pts.push_back(Point(0, img_height));
    pts.push_back(Point(0, img_height));
    contour.push_back(pts);

    drawContours(mask, contour, 0, Scalar::all(255), -1);
    srcImg.copyTo(dstImg, mask);
    return dstImg;
}

cv::Mat draw_lines(Mat& img, vector<Vec4f>  lines)
{
    Mat line_image = img.clone();
    vector<Point> PointsL, PointsR;
    cv::Vec4f lineL, lineR;
    int y_min = img.size().height;
    int y_max = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        Vec4f point1 = lines[i];
        double k;

        if (point1[1] < point1[3])
        {
            if (point1[1] < y_min)
            {
                y_min = point1[1];
            }
            if (point1[3] > y_max)
            {
                y_max = point1[3];
            }
            k=(point1[3]-point1[1])/(point1[2]-point1[0]);
            if (k>0.3&&k<2.0)
            {
                PointsR.push_back(Point(point1[0], point1[1]));
                PointsR.push_back(Point(point1[2], point1[3]));

            }
            else if(k<-0.3&&k>-2.0)
            {
                PointsL.push_back(Point(point1[0], point1[1]));
                PointsL.push_back(Point(point1[2], point1[3]));
            }
        }
        else
        {
            if (point1[3] < y_min)
            {
                y_min = point1[3];
            }
            if (point1[1] > y_max)
            {
                y_max = point1[1];
            }
            k=(point1[3]-point1[1])/(point1[2]-point1[0]);
            if (k>0.3&&k<2.0)
            {
                PointsR.push_back(Point(point1[0], point1[1]));
                PointsR.push_back(Point(point1[2], point1[3]));
            }
            else if(k<-0.3&&k>-2.0)
            {
                PointsL.push_back(Point(point1[0], point1[1]));
                PointsL.push_back(Point(point1[2], point1[3]));
            }
        }

    }
    //????????????
    cv::fitLine(PointsL, lineL, DIST_HUBER, 0, 1e-2, 1e-2);
    cv::fitLine(PointsR, lineR, DIST_HUBER, 0, 1e-2, 1e-2);

    double k1,k2,b1,b2;
    cv::Point pointL1,pointL2, pointR1, pointR2;
    k1 = lineL[1] / lineL[0];
    b1 = lineL[3] - k1 * lineL[2];
    k2 = lineR[1] / lineR[0];
    b2 = lineR[3] - k2 * lineR[2];
    pointL1.x = (y_min - b1) / k1;
    pointL1.y = y_min;
    pointL2.x = (y_max - b1) / k1;
    pointL2.y = y_max;
    pointR1.x = (y_min - b2) / k2;
    pointR1.y = y_min;
    pointR2.x = (y_max - b2) / k2;
    pointR2.y = y_max;
    cv::line(line_image, Point(pointL1.x, pointL1.y), Point(pointL2.x, pointL2.y), Scalar(0, 0, 255), 2, LINE_AA);
    cv::line(line_image, Point(pointR1.x, pointR1.y), Point(pointR2.x, pointR2.y), Scalar(255, 0, 0), 2, LINE_AA);
    //cv::imshow("line_image", line_image);
    return line_image;
}




Widget::Widget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*-----------------------------MQTT-------------------------------------------------------------------------------*/
    QHostAddress m_address("a1G6ff0Lu1m.iot-as-mqtt.cn-shanghai.aliyuncs.com");
    client = new QMQTT::Client(m_address, 1883);
    client->setClientId("mqtt_test1|securemode=3,signmethod=hmacsha1|");
    client->setUsername("mqtt_test1&a1G6ff0Lu1m");
    client->setPassword("fbea3b809d1fa8fc22087c7cf1f8c9b7dfdda662");
    //client->setVersion(QMQTT::MQTTVersion::V3_1_1);
    client->setKeepAlive(120);
    connect(client,SIGNAL(connected()),this,SLOT(doprocess_connected()));
    connect(client,SIGNAL(subscribed(QString,quint8)),this,SLOT(doprocess_subscribled(QString,quint8)));
    connect(client,SIGNAL(received(QMQTT::Message)),this,SLOT(doprocess_receivemessage(QMQTT::Message)));
    connect(client,SIGNAL(unsubscribed(QString)),this,SLOT(doprocess_unsubscrbled(QString)));
    connect(client,SIGNAL(error(QMQTT::ClientError)),this,SLOT(doprocess_mqtterror(QMQTT::ClientError)));
    connect(client,SIGNAL(disconnected()),this,SLOT(doprocess_disconnected()));

    /*--------------------------------------------------------------------------------------------------------------------------*/

    //???????????????
    QString hostName = QHostInfo::localHostName();

    //??????IP??????
    QHostInfo hostInfo = QHostInfo::fromName(hostName);

    //IP????????????
    QList<QHostAddress> addrList = hostInfo.addresses();
    for(int i=0;i<addrList.count();i++)
    {
        QHostAddress host = addrList.at(i);

        if(QAbstractSocket::IPv4Protocol == host.protocol())
        {
            QString ip = host.toString();

            ui->comboBox->addItem(ip);

        }
    }

    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer,&QTcpServer::newConnection,this,&Widget::on_newConnection);


}



Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnStart_clicked()
{
    //???????????????ip
    QString ip = ui->comboBox->currentText();

    //??????
    int port = ui->lineEditPort->text().toInt();

    QHostAddress addr(ip);

    //??????
    m_tcpServer->listen(addr,port);

    ui->plainTextEdit->appendPlainText("**????????????...");

    ui->plainTextEdit->appendPlainText("**???????????????: "+m_tcpServer->serverAddress().toString());

    ui->plainTextEdit->appendPlainText("**???????????????: "+QString::number(m_tcpServer->serverPort()));

    ui->btnStart->setEnabled(false);

    ui->btnStop->setEnabled(true);

    ui->lbListen->setText("????????????");


}

void Widget::on_btnStop_clicked()
{
    if(m_tcpServer->isListening())
    {
        m_tcpServer->close();
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);
        ui->lbListen->setText("????????????");
        ui->plainTextEdit->appendPlainText("**????????????**");
    }
}

void Widget::on_btnClear_clicked()
{
    ui->plainTextEdit->clear();
}

void Widget::on_newConnection()
{
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket,&QTcpSocket::connected,this,&Widget::onConnected);
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&Widget::onDisConnected);
    connect(m_tcpSocket,&QTcpSocket::stateChanged,this,&Widget::onStateChanged);
    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&Widget::onReadyRead);

    ui->plainTextEdit->appendPlainText("** client socket connected");
    ui->plainTextEdit->appendPlainText("** peer address: "+m_tcpSocket->peerAddress().toString());
    ui->plainTextEdit->appendPlainText("** peer port: "+QString::number(m_tcpSocket->peerPort()));
}

void Widget::onConnected()
{
    ui->plainTextEdit->appendPlainText("** client socket connected");
    ui->plainTextEdit->appendPlainText("** peer address: "+m_tcpSocket->peerAddress().toString());
    ui->plainTextEdit->appendPlainText("** peer port: "+QString::number(m_tcpSocket->peerPort()));
}

void Widget::onDisConnected()
{
    ui->plainTextEdit->appendPlainText("** client socket disconnected");
    m_tcpSocket->deleteLater();
}

void Widget::onStateChanged(QAbstractSocket::SocketState socketState)
{
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        ui->lbListen->setText("UnconnectedState");break;
    case QAbstractSocket::HostLookupState:
        ui->lbListen->setText("HostLookupState");break;
    case QAbstractSocket::ConnectedState:
        ui->lbListen->setText("ConnectedState");break;
    case QAbstractSocket::ConnectingState:
        ui->lbListen->setText("ConnectingState");break;
    case QAbstractSocket::BoundState:
        ui->lbListen->setText("BoundState");break;
    case QAbstractSocket::ClosingState:
        ui->lbListen->setText("ClosingState");break;
    case QAbstractSocket::ListeningState:
        ui->lbListen->setText("ListeningState");break;
    }

}

void Widget::onReadyRead()
{
    /*
    static QByteArray sumData;
        QByteArray tempData;

        while(m_tcpSocket->waitForReadyRead(100))
        {
            tempData.append((QByteArray)m_tcpSocket->readAll());


        if(!tempData.isEmpty())
        {
            sumData.append(tempData);
            if(sumData.contains("\n")) // ??????????????????
            {
               // do_DataHandler(sumData); // ????????????
                if(sumData.right(2)!="\xFF\xD9")

                sumData.clear();
            }
        }
        tempData.clear();


    }*/
    /*
    bool m_bReceive;  //????????????false
    QByteArray m_Message; //????????????????????????
    int m_length;   //????????????????????????
    QByteArray m_resultMessage;  //??????????????????
    if (!m_bReceive)
       {
           m_bReceive = true;
           //????????????10?????????????????????
           m_length = QString(m_tcpSocket->read(10)).toInt();
       }
       //????????????
       m_Message += m_tcpSocket->readAll();
       int len = m_Message.length();
       //????????????????????????
       if (len >= m_length)
       {
           //?????? QByteArray --> QString
           //??????????????????????????????????????????????????????????????? QByteArray ????????????
           m_resultMessage =
       }

    */

    /*----------------------------------*/
    QByteArray array;
    QByteArray temparray;
    QByteArray jpgarray;
    while(m_tcpSocket->waitForReadyRead(100))
    {
        array.append((QByteArray)m_tcpSocket->readAll());

        //if(array.right(2)!="\xFF\xD9"){
        // array.append((QByteArray)m_tcpSocket->readAll());

        // }


        /*
        temparray= array.left(3);
        qDebug()<<temparray.toHex().toUpper();
       // jpgarray=temparray.replace(1,3,'\0');
        jpgarray=temparray.replace(1,3,"\0");
        qDebug()<<jpgarray.toHex().toUpper();*/

    }
    //qDebug()<<array.right(2);

    //ui->plainTextEdit->appendPlainText(array.toHex().toUpper());
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer,"JPG");
    QImage img = reader.read();


    //QImage???Mat
    QImage tempimg;
    tempimg=img.convertToFormat(QImage::Format_RGB888);

    Mat frame = Mat(tempimg.height(),
                    tempimg.width(),
                    CV_8UC(3),
                    tempimg.bits(),
                    tempimg.bytesPerLine());
    // namedWindow("Display window", WINDOW_AUTOSIZE );
    // imshow("Display window", frame );
    // waitKey(1000);



    /*

    Mat  gray_image;
    cv::cvtColor(frame, gray_image, COLOR_BGR2GRAY);

    Mat bin_image;
    cv::threshold(gray_image,bin_image,150,255,CV_THRESH_BINARY);

    Mat binnot_image;
    bitwise_not(bin_image,binnot_image);

    if(!bin_image.empty()){
        namedWindow("Nbinary", WINDOW_AUTOSIZE );
        imshow("Nbinary", bin_image);
        // waitKey(1000);
    }

    //??????Canny??????????????????
    Mat canny_output;
    cv::Canny(bin_image, canny_output, 50, 150, 3);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //??????canny??????????????????
    if(!canny_output.empty()){
        namedWindow("dil", WINDOW_AUTOSIZE );
        imshow("dil", canny_output);
        // waitKey(1000);
    }

*/


    /*Mat dil_image;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    cv::dilate(bin_image,dil_image,element);
    if(!dil_image.empty()){
        namedWindow("dil", WINDOW_AUTOSIZE );
        imshow("dil", dil_image);
        // waitKey(1000);
    }*/





    //????????????
    Mat  gray_image;
    cv::cvtColor(frame, gray_image, COLOR_BGR2GRAY);
    //show_img(gray_image);

    //????????????
    Mat blur_image;
    //bilateralFilter(gray_image, blur_image, 13, 15, 15);
    cv::GaussianBlur(gray_image, blur_image, cv::Size(3, 3), 0, 0);
    //show_img(blur_image);

    //????????????
    Mat canny_image;
    cv::Canny(blur_image, canny_image, 50, 150, 3);
    // show_img(canny_image);



    //?????????????????????
    // Mat ROI_image;
    //   ROI_image= region_of_interest(canny_image);

    /*
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //????????????
    findContours( canny_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    //???????????????
    vector<Moments> mu(contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {
        mu[i] = moments( contours[i], false );
    }
    //?????????????????????
    vector<Point2f> mc( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {
        mc[i] = Point2d( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

    }


    //??????????????????????????????
    Mat drawing = Mat::zeros( canny_image.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( 255, 0, 0);
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
        circle( drawing, mc[i], 5, Scalar( 0, 0, 255), -1, 8, 0 );
        rectangle(drawing, boundingRect(contours.at(i)), cvScalar(0,255,0));
        char tam[100];
        sprintf(tam, "(%0.0f,%0.0f)",mc[i].x,mc[i].y);
        putText(drawing, tam, Point(mc[i].x, mc[i].y), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(255,0,255),1);
    }
    if(!drawing.empty()){
        namedWindow("Hough_image", WINDOW_AUTOSIZE );
        imshow("Hough_image", drawing);
        // waitKey(1000);
    }
    */

    //????????????
    Mat Hough_image;
    cv::cvtColor(canny_image, Hough_image, COLOR_GRAY2BGR);
    // cv::cvtColor(ROI_image, Hough_image, COLOR_GRAY2BGR);
    vector<Vec4f>plines;//???????????????????????????????????????
    double rho = 1;
    double theta = CV_PI / 180;
    int threshold = 15;
    double minLineLength = 40;
    double maxLineGap = 20;
    cv::HoughLinesP(canny_image, plines, rho, theta, threshold, minLineLength, maxLineGap);
    // cv::HoughLinesP(ROI_image, plines, rho, theta, threshold, minLineLength, maxLineGap);
    for (size_t i = 0; i < plines.size(); i++)
    {
        //Vec4f point1 = plines[i];
        //cv::line(Hough_image, Point(point1[0], point1[1]), Point(point1[2], point1[3]), Scalar(255, 255, 0), 1, LINE_AA);

        cv::line(Hough_image, Point(plines[i][0], plines[i][1]),
                Point(plines[i][2], plines[i][3]),
                Scalar(255, 255, 0), 1, 8);
    }

    if(!Hough_image.empty()){
        namedWindow("Hough_image", WINDOW_AUTOSIZE );
        imshow("Hough_image", Hough_image);
        // waitKey(1000);
    }

    QByteArray Left="{\"method\":\"thing.event.property.post\",\"id\":\"000000014\",\"params\":{\"Right\":0,\"Left\":1,\"Straight\":0},\"version\":\"1.0.0\"}";
    QByteArray Right="{\"method\":\"thing.event.property.post\",\"id\":\"000000014\",\"params\":{\"Right\":1,\"Left\":0,\"Straight\":0},\"version\":\"1.0.0\"}";
    QByteArray Straight="{\"method\":\"thing.event.property.post\",\"id\":\"000000014\",\"params\":{\"Right\":0,\"Left\":0,\"Straight\":1},\"version\":\"1.0.0\"}";
    //qDebug()<<Left;

    if(!Hough_image.empty()){

        Mat line = Hough_image.row(16);
        int width = line.cols;//??????????????????
        int count=0,sum=0,res=0;
        for (int col = 0; col < width; col++) {

            if (line.at<Vec3b>(16, col) [0]== 255){
                //qDebug()<<col;
                count++;
                sum+=col;
                res=sum/count;
                //qDebug()<<"res:"+res;
                if(res<=64){
                    //?????????
                    /*QByteArray jsonarray;
                 QJsonObject carjson;
                 QJsonArray   cararray;
                 carjson.insert("Right","0");
                 carjson.insert("Left","0");
                 carjson.insert("Straight","1");
                 cararray.push_back(carjson);
                 QJsonDocument jsonDoc(cararray);
                 QByteArray strArray = jsonDoc.toJson(QJsonDocument::Indented);
                 QString string(strArray);
                 QJsonObject json;
                 json.insert("id","000000014");
                 json.insert("method","thing.event.property.post");
                 json.insert("params",string);
                 json.insert("version","1.0.0");
                 QJsonDocument strdocument;
                 strdocument.setObject(json);
                 jsonarray = strdocument.toJson(QJsonDocument::Compact);
                 qDebug()<<jsonarray;*/

                    car_publish(Left);
                    qDebug()<<" Left success";


                }else if (res>64 && res<=128) {
                    //?????????
                    car_publish(Left);
                    qDebug()<<" Left success";

                }else if (res>128 && res<=192) {
                    //??????
                    car_publish(Straight);
                    qDebug()<<" Straight success";
                }else if (res>192 && res<=256) {
                    //?????????
                    car_publish(Right);
                    qDebug()<<" Right success";
                }else if (res>256 && res<=320) {
                    //?????????
                    car_publish(Right);
                    qDebug()<<" Right success";
                }

            }
        }
    }

    //????????????
    /* Mat lane_image;
             lane_image=draw_lines(frame, plines);
            if(!frame.empty()){
                                    namedWindow("binary", WINDOW_AUTOSIZE );
                                    imshow("binary", frame);
                                   // waitKey(1000);
                  }*/

    if(!img.isNull()){
        QPixmap pix = QPixmap::fromImage(img);
        ui->showImageLabel->setPixmap(pix.scaled(ui->showImageLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    }

    /*
     //?????????
     QImage * newImage = new QImage(img.width(), img.height(), QImage::Format_ARGB32);


         for(int y = 0; y<newImage->height(); y++){
             //??????????????????????????????
             QRgb * line = (QRgb *)img.scanLine(y);

             for(int x = 0; x<newImage->width(); x++){
                 //????????????
                 //??????????????????????????????R G B ???????????????
                 int average = (qRed(line[x])*0.299 + qGreen(line[x])*0.587 + qBlue(line[x])*0.114);
                 newImage->setPixel(x,y, qRgb(average, average, average));
             }

         }
         QPixmap *pixmap=new QPixmap();
            pixmap->convertFromImage(*newImage);

            ui->binLable->setPixmap(*pixmap);
            */

    //?????????
    int temthreshold=122;
    //img.load(currentPath);
    QImage image = img.convertToFormat(QImage::Format_Indexed8);
    image.setColorCount(256);
    for(int i = 0; i < 256; i++)
    {
        int j;
        if (i < temthreshold)
        {
            j = 0;
        }
        else
        {
            j = 255;
        }
        image.setColor(i, qRgb(j, j, j));
    }
    //QPixmap pixmap1(QPixmap::fromImage (image));
    //ui->binLable->setPixmap(pixmap1);// ??????????????????
    //qDebug() << "???????????????????????????" << threshold;

    if(!image.isNull()){
        QPixmap pixmap1(QPixmap::fromImage (image));
        ui->binLable->setPixmap(pixmap1.scaled(ui->showImageLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    }
}

void Widget::on_btnSend_clicked()
{
    QString msg =ui->lineEditMsg->text();
    ui->plainTextEdit->appendPlainText("[out]"+msg);
    QByteArray str = msg.toUtf8();
    str.append('\n');
    m_tcpSocket->write(str);
}


/*------------MQTT----------------------------------------*/

/* ------??????connect------*/
void Widget::on_btn_connect_clicked()
{
    QString ledit_address = ui->lineEdit_address->text(); //????????????????????????????????????
    int ledit_port = ui->lineEdit_port->text().toInt();
    if(0 == ledit_port)
        return;

    client->setHostName(ledit_address);
    client->setPort(ledit_port);
    client->connectToHost();
}

/*---------??????publish------------*/
void Widget::on_btn_publish_clicked()
{
    // QByteArray m_payload  = ui->textEdit_publish->toPlainText().toLatin1();
    QByteArray m_payload  = ui->textEdit_publish->toPlainText().toUtf8();
    QString m_topic = ui->lineEdit_publishtopic->text();
    // qDebug()<<m_payload;
    // qDebug()<<ui->textEdit_publish->toPlainText();
    if(m_topic.isNull())
        return;
    //QMQTT::Message m_message(m_topic,m_payload);
    QMQTT::Message m_message(1,m_topic,m_payload);//?????????1
    client->publish(m_message);
}

void Widget::car_publish(QByteArray(statusarray))
{
    // QByteArray m_payload  = ui->textEdit_publish->toPlainText().toLatin1();
    // QByteArray m_payload  = ui->textEdit_publish->toPlainText().toUtf8();
    QString m_topic = ui->lineEdit_publishtopic->text();
    // qDebug()<<m_payload;
    // qDebug()<<ui->textEdit_publish->toPlainText();
    if(m_topic.isNull())
        return;
    //QMQTT::Message m_message(m_topic,m_payload);
    QMQTT::Message m_message(1,m_topic,statusarray);//?????????1
    client->publish(m_message);
}

/*-------------- ??????subscribe----------*/
void Widget::on_btn_subscribe_clicked()
{
    QString m_topic = ui->lineEdit_subscribletopic->text();
    int m_qos = ui->lineEdit_subscribleqos->text().toInt();
    if(m_topic.isNull())  //m_topic????????????1
        return;
    client->subscribe(m_topic,m_qos);
}

/*---------- ????????????----------*/
void Widget::on_btn_clear_clicked()
{
    ui->textEdit_message->clear();
}

/*----------- ????????????unsubscribe-----------*/
void Widget::on_btn_unsubscrible_clicked()
{
    QString m_topic = ui->lineEdit_unsubscribletopic->text();
    if(m_topic.isNull())
        return;
    client->unsubscribe(m_topic);
}

/*----------????????????-------------*/
void Widget::on_btn_disconnect_clicked()
{
    client->disconnectFromHost();
}

/*------------????????????---------------*/
void Widget::doprocess_connected()
{
    ui->textEdit_message->append("server connected");
    ui->btn_connect->setText("?????????");
}

/*------------????????????----------*/
void Widget::doprocess_subscribled(QString topic, quint8 qos)
{
    ui->textEdit_message->append(QString("topic:%1,QOS:%2,subscribled").arg(topic).arg(qos));
}

/*------------???????????????----------*/
void Widget::doprocess_receivemessage(QMQTT::Message message)
{
    /*
    QString recv_msg = "Topic:";
    QString data = message.payload();
    recv_msg += message.topic();
    recv_msg += "    Payload:";
    recv_msg += data;
    ui->textEdit_message->append(recv_msg);
    */
    ui->textEdit_message->append(QString("topic:%1,payload:%2").arg(message.topic()).arg(QString(message.payload())));
}

/*----------- ??????????????????----------*/
void Widget::doprocess_unsubscrbled(QString topic)
{
    ui->textEdit_message->append(QString("topic:%1,unsubscribled").arg(topic));
}

/*-----------mqtt??????------------*/
void Widget::doprocess_mqtterror(QMQTT::ClientError err)
{
    ui->textEdit_message->append(QString::number(err));
}

/*----------????????????--------------*/
void Widget::doprocess_disconnected()
{
    ui->textEdit_message->append("disconnected");
    ui->btn_connect->setText("?????????");
}


