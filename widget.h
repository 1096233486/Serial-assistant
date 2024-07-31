#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMutex>
#include <QSerialPort>//串口类
#include <QSerialPortInfo>//串口信息类
#include <QMessageBox>//弹窗
#include <QDateTime>
#include <QTest>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class LoopSendThrend : public QThread
{
    Q_OBJECT
public:
    LoopSendThrend(QWidget *parent = nullptr){}
    ~LoopSendThrend(){}
    void run();
    Ui::Widget *ui;//这个ui在实例化与下方主Widget是同一地址，这样就可以在线程中操作ui内容

    void threadPause(){//暂停线程
        qDebug()<<QString("pause :%1").arg(m_buttonState);
        this->m_mutex.lock();
        this->m_buttonState=false;
        qDebug()<<QString("pause");
    }

    void threadResume(){//继续线程
        qDebug()<<QString("resume :%1").arg(m_buttonState);
        this->m_mutex.unlock();
        this->m_buttonState=true;
        qDebug()<<QString("resume");
    }

private:
    bool m_buttonState; //if pause m_buttonState=false;else m_buttonState=true;
    int m_i;
    QMutex m_mutex;//互斥量
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked(bool checked);
    void readData();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Widget *ui;
    QSerialPort *serialPort;
    LoopSendThrend *loopSend;
};


#endif // WIDGET_H
