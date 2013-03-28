#ifndef QTXNETWORK_PLUGGABLENETWORKACCESSMANAGER_H
#define QTXNETWORK_PLUGGABLENETWORKACCESSMANAGER_H

#include "networkglobal.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class PluggableNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
    
public:
    PluggableNetworkAccessManager(QObject *parent = 0);
    virtual ~PluggableNetworkAccessManager();
    
protected:
    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0);
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_PLUGGABLENETWORKACCESSMANAGER_H
