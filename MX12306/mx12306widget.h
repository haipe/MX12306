#ifndef MX12306WIDGET_H
#define MX12306WIDGET_H

#include "miniblinkwidget.h"

class MX12306Widget : public mxtoolkit::MiniBlinkWidget
{
    Q_OBJECT
public:
    explicit MX12306Widget(QWidget *parent = nullptr, const QString& url = "");

    void start(const QString& from,const QString& to,const QString& code,const QString& day);
    void start(const QString& url);
    void start();

public:
    //自定义信号
signals:
    void onLoadUrlCompleted();
    void onTripsStatusSignal(const QString& source, const QString& dest, const QString& day, const QString& trips, bool isCanBuy);

protected:
    virtual void closeEvent(QCloseEvent *event);

    // 回调：文档加载成功
    virtual void onDocumentReady();
    virtual bool onLoadUrlBegin(const QString& url, void *job);
    virtual void onLoadUrlEnd(const QString& url, void *job, void* buf, int len);

protected:
    bool    tripsStatus;
    QString findTrips;
    QString queryDay;
    QString sourceStations;
    QString destStation;
    QString loadDataUrl;
};

#endif // MX12306WIDGET_H
