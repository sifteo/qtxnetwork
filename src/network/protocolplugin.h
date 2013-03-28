#ifndef QTXNETWORK_PROTOCOLPLUGIN_H
#define QTXNETWORK_PROTOCOLPLUGIN_H

#include "networkglobal.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class ProtocolPlugin
{
public:
    virtual ~ProtocolPlugin() {};
    
    virtual QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0) = 0;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_PROTOCOLPLUGIN_H
