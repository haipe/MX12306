#include "mx12306widget.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>



MX12306Widget::MX12306Widget(QWidget *parent, const QString& url)
    : MiniBlinkWidget(parent,url)
{

}

void MX12306Widget::start(const QString &from, const QString &to, const QString &code, const QString &day)
{
    QString url;
    url.sprintf("https://kyfw.12306.cn/otn/leftTicket/init?linktypeid=dc&fs=%s,IZQ&ts=%s,SWZ&date=%s&flag=N,N,Y",
                from.toStdString().c_str(),to.toStdString().c_str(),day.toStdString().c_str());
    start(url);

    queryDay = day;
    sourceStations = from;
    destStation = to;
    findTrips = code;
}

void MX12306Widget::start(const QString &url)
{
    loadUrl(url);
}

void MX12306Widget::start()
{
    loadUrl(request_url);
}

void MX12306Widget::onDocumentReady()
{
    emit onLoadUrlCompleted();
}

bool MX12306Widget::onLoadUrlBegin(const QString& url, void *job)
{
    if (url.indexOf("https://kyfw.12306.cn/otn/leftTicket/queryZ?") != -1)
    {
        wkeNetHookRequest(job);
    }

    return true;
}

void MX12306Widget::onLoadUrlEnd(const QString& url, void *job, void *buf, int len)
{
    if (url.indexOf("https://kyfw.12306.cn/otn/leftTicket/queryZ?") != -1)
    {
        QJsonParseError e;
        QJsonDocument jsonDoc = QJsonDocument::fromJson((const char*)buf, &e);

        if(e.error != QJsonParseError::NoError || jsonDoc.isNull())
        {
            qDebug() << "onLoadUrlEnd was not json!";
            //qDebug() << jsonDoc;
            return;
        }

        int webStatus = jsonDoc["httpstatus"].toInt();
        bool status = jsonDoc["status"].toBool();

        QJsonObject dataObj = jsonDoc["data"].toObject();

        QJsonArray result = dataObj["result"].toArray();
        foreach (QJsonValue v, result)
        {
            QString trips = v.toString();
            if(trips.indexOf(findTrips) >= 0)
            {
                tripsStatus = true;
                if(trips.indexOf("暂停发售") >= 0)
                    tripsStatus = false;

                //qDebug() << "车次：" << v.toString();

                emit onTripsStatusSignal(sourceStations, destStation, queryDay, findTrips, tripsStatus);
                break;
            }
        }
    }
}
