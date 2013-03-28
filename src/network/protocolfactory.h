#ifndef QTXNETWORK_PROTOCOLFACTORY_H
#define QTXNETWORK_PROTOCOLFACTORY_H

#include "networkglobal.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class ProtocolPlugin;

class ProtocolFactory
{
public:
    static void registerPlugin(ProtocolPlugin *plugin);
    static QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0);
    
private:
    static QList<ProtocolPlugin *> mPlugins;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_PROTOCOLFACTORY_H
