#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    serialPort = new QSerialPort(this);
    loopSend = new LoopSendThrend(this);
    loopSend->ui = ui;
    loopSend->start();
    loopSend->threadPause();
    this->setLayout(ui->verticalLayout_3);

    //扫描本机串口编号并添加到下拉选择
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
    ui->comboBox->addItem(info.portName());
    }

    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readData()));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked(bool checked)
{
    if(checked){
        serialPort->setPortName(ui->comboBox->currentText());//设置串口名字
        serialPort->setBaudRate(ui->comboBox_2->currentText().toInt());//设置波特率
        serialPort->setStopBits(QSerialPort::StopBits(ui->comboBox_3->currentText().toInt()));//设置停止位，这里参数是枚举类型，把数字转换为StopBits类型
        serialPort->setDataBits(QSerialPort::DataBits(ui->comboBox_4->currentText().toInt()));//设置数据位，和上同理
        switch (ui->comboBox_5->currentIndex()) {//配置校验位
            case 0:
                serialPort->setParity(QSerialPort::NoParity);
                break;
            case 1:
                serialPort->setParity(QSerialPort::EvenParity);
                break;
            case 2:
                serialPort->setParity(QSerialPort::OddParity);
                break;
            case 3:
                serialPort->setParity(QSerialPort::SpaceParity);
                break;
            case 4:
                serialPort->setParity(QSerialPort::MarkParity);
                break;
        }
        serialPort->setFlowControl(QSerialPort::NoFlowControl);//配置流控（默认无流控）
        if(!serialPort->open(QSerialPort::ReadWrite)){//打开串口并判断是否打开成功
            /*如果打开失败*/
            ui->comboBox->clear();//当打开失败时再次扫描是否有串口
            foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
                ui->comboBox->addItem(info.portName());
            }
            ui->pushButton->setChecked(false);//打开失败让按钮回到未被选中状态
            QMessageBox::about(this,"打开错误","请检查串口是否被占用");
            return;
        }
        else
        {
            ui->textBrowser->insertPlainText("打开串口成功\n");
        }
        ui->comboBox->setEnabled(false);//当打开时选项不可选
        ui->comboBox_2->setEnabled(false);
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->setEnabled(false);
        ui->pushButton->setText("关闭串口");
    }
    else{
        serialPort->close();
        ui->comboBox->setEnabled(true);//当关闭时选项可选
        ui->comboBox_2->setEnabled(true);
        ui->comboBox_3->setEnabled(true);
        ui->comboBox_4->setEnabled(true);
        ui->comboBox_5->setEnabled(true);
        ui->pushButton->setText("打开串口");

        ui->comboBox->clear();//当关闭时再次扫描是否有串口
        foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
            ui->comboBox->addItem(info.portName());
        }
    }
}

void Widget::readData()//读取数据并显示
{
    ui->textBrowser->insertPlainText(QDateTime::currentDateTime().toString());//时间
    ui->textBrowser->insertPlainText(":  ");
    ui->textBrowser->insertPlainText(serialPort->readAll());//数据
    if(ui->checkBox_2->isChecked()){//换行
        ui->textBrowser->insertPlainText("\n");
    }
}

void Widget::on_pushButton_2_clicked()//发送数据
{
    //ui->textBrowser->insertPlainText(
    serialPort->write(ui->textEdit->toPlainText().toUtf8());
}

void Widget::on_pushButton_3_clicked()//清除
{
    ui->textEdit->clear();
    ui->textBrowser->clear();
}

void LoopSendThrend::run()
{
    QThread::msleep(1);
    m_buttonState=true;
    while(true)
    {
        m_mutex.lock();
        m_i++;
        ui->pushButton_2->click();
        QThread::msleep(ui->spinBox->value());
        m_mutex.unlock();
    }
}

void Widget::on_checkBox_stateChanged(int arg1)
{
    //如果勾选循环发送
    bool status =ui->checkBox->isChecked();
    if(arg1)
    {
        loopSend->threadResume();
    }
    else
    {
        loopSend->threadPause();
    }
}

