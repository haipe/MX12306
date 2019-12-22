#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mx12306widget.h"
#include <QTimer>
#include <QSoundEffect>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
protected:
    void updateStatus(const QString& msg);
    void restartQueryTimer();
    void playVoice(bool play);

private slots:
    void on_pushButton_Doit_clicked();
    void on_checkBox_Config_clicked(bool checked);
    void on_checkBox_Debug_clicked(bool checked);

    void on_query_timne_out();
    void on_12306_load_completed();

    void on_trips_status(const QString& source, const QString& dest, const QString& day, const QString& trips, bool canBuy);

protected:
    MX12306Widget* web12306Widget = nullptr;

    QTimer* queryTimer = nullptr;
    bool havSuccessQuery = false;

    QSoundEffect* playMusic = nullptr;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
