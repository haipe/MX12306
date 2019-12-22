#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "_mx_qt_include.h"
#include "mx12306widget.h"
#include "QTime"
#include <QDate>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateStatus("Ready");

    setWindowTitle("MX12306 列车调整监控");
    //ui->centralwidget->layout()->setAlignment(Qt::AlignTop);
    //ui->verticalLayout_Result->setAlignment(Qt::AlignTop);

    ui->lineEdit_From->setText("广州南");
    ui->lineEdit_To->setText("三江南");
    ui->lineEdit_Code->setText("D1826");
    ui->dateEdit_Day->setDisplayFormat("yyyy-MM-dd");
    ui->dateEdit_Day->setDate(QDate::currentDate().addDays(29));

    ui->widget_QueryConfig->layout()->setAlignment(Qt::AlignLeft);
    ui->spinBox_QueryTimeData->setValue(5);
    ui->widget_QueryConfig->hide();

    web12306Widget = new MX12306Widget(ui->centralwidget,"https://www.12306.cn/index/index.html");
    //ui->verticalLayout_Result->addWidget(web12306Widget);
    ui->centralwidget->layout()->addWidget(web12306Widget);
    web12306Widget->hide();

    QObject::connect(web12306Widget,SIGNAL(onLoadUrlCompleted()),this,SLOT(on_12306_load_completed()),Qt::QueuedConnection);

    QObject::connect(
                web12306Widget,
                SIGNAL(onTripsStatusSignal(const QString&,const QString&,const QString&, const QString&, bool)),
                this,
                SLOT(on_trips_status(const QString&,const QString&,const QString&, const QString&, bool)),
                Qt::QueuedConnection);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateStatus(const QString &msg)
{
    QString statusMsg;
    statusMsg.sprintf("[ %s ] ", QTime::currentTime().toString().toStdString().c_str());
    statusMsg += msg;
    ui->statusbar->showMessage(statusMsg);
}

void MainWindow::restartQueryTimer()
{
    if(ui->spinBox_QueryTimeData->value() <= 0)
        ui->spinBox_QueryTimeData->setValue(5);

    if(!queryTimer)
    {
        queryTimer = new QTimer;
        queryTimer->setSingleShot(false);
        QObject::connect(queryTimer, SIGNAL(timeout()), this, SLOT(on_query_timne_out()));
    }

    if(queryTimer->isActive())
        queryTimer->stop();

    int queryClock = ui->spinBox_QueryTimeData->value() * 1000;
#ifdef QT_NO_DEBUG
    queryClock *= 60;
#endif

    queryTimer->start(queryClock);
}

void MainWindow::playVoice(bool play)
{
    if(!playMusic)
    {
        playMusic = new QSoundEffect;
    }

    if(play)
    {
        QString path = QCoreApplication::applicationDirPath() + "/music.wav";
        playMusic->setSource(QUrl::fromLocalFile(path));
        playMusic->setLoopCount(QSoundEffect::Infinite);
        playMusic->setVolume(0.9);
        playMusic->play();
    }
    else
    {
        playMusic->stop();
    }
}

void MainWindow::on_pushButton_Doit_clicked()
{
    playVoice(false);
    if(queryTimer && queryTimer->isActive())
    {
        ui->pushButton_Doit->setText("Start");

        havSuccessQuery = true;
        queryTimer->stop();
        updateStatus("Ready");
    }
    else
    {
        ui->pushButton_Doit->setText("Stop");

        QString src = ui->lineEdit_From->text();
        QString dst = ui->lineEdit_To->text();
        QString code =ui->lineEdit_Code->text();

        QString day = ui->dateEdit_Day->text();

        havSuccessQuery = false;
        web12306Widget->start(src,dst,code,day);

        restartQueryTimer();

        updateStatus("Start Query...");
    }
}

void MainWindow::on_checkBox_Debug_clicked(bool checked)
{
    if(checked)
    {
        web12306Widget->show();
        //ui->centralwidget->layout()->addWidget(web12306Widget);
        showMaximized();
    }
    else
    {
        web12306Widget->hide();
        //ui->centralwidget->layout()->removeWidget(web12306Widget);
        showNormal();
    }
}

void MainWindow::on_checkBox_Config_clicked(bool checked)
{
    if(checked)
    {
        ui->widget_QueryConfig->show();
        if(!this->isMaximized())
            this->resize(this->geometry().width(),this->geometry().height() + 40);
    }
    else
    {
        ui->widget_QueryConfig->hide();
        if(!this->isMaximized())
            this->resize(this->geometry().width(),this->geometry().height() - 40);
    }
}

void MainWindow::on_query_timne_out()
{
    qDebug() << "MainWindow" << __FUNCTION__;
    updateStatus("Auto Query...");

    web12306Widget->start();
}

void MainWindow::on_12306_load_completed()
{
    qDebug() << "MainWindow" << __FUNCTION__;
    if(!queryTimer || queryTimer->isActive())
        return;

    playVoice(true);
    if(!havSuccessQuery)
    {
        web12306Widget->start();
    }
    else
    {
        updateStatus("Have Error.");
    }
}

void MainWindow::on_trips_status(const QString& source, const QString& dest, const QString& day, const QString &trips, bool canBuy)
{
    qDebug() << "MainWindow" << __FUNCTION__;
    havSuccessQuery = true;
    updateStatus("Query once.");

    ui->lineEdit_From->setText(source);
    ui->lineEdit_To->setText(dest);
    ui->lineEdit_Code->setText(trips);
    ui->dateEdit_Day->setDate(QDate::fromString(day));

    ui->label_TimeDate->setText(QTime::currentTime().toString());
    ui->label_CodeDate->setText(trips);
    ui->label_DayDate->setText(day);
    ui->label_SourceDate->setText(source);
    ui->label_DestDate->setText(dest);
    ui->label_StatusDate->setText(canBuy ? "可以预定" : "列车运行图调整,暂停发售");
    if(canBuy)
    {
        ui->widget_Result->setStyleSheet("background-color: #ff0000;");

        Qt::WindowFlags flags = windowFlags();
        setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    }
}
