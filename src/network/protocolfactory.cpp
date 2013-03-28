#include "protocolfactory.h"
#include "protocolplugin.h"

QTX_BEGIN_NAMESPACE


void ProtocolFactory::registerPlugin(ProtocolPlugin *plugin)
{
    mPlugins.prepend(plugin);
}

QNetworkReply * ProtocolFactory::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest & req, QIODevice * outgoingData /* = 0 */)
{
    foreach (ProtocolPlugin *plugin, mPlugins) {
        QNetworkReply *reply = plugin->createRequest(op, req, outgoingData);
        if (reply) { return reply; }
    }
    return 0;
}


QList<ProtocolPlugin *> ProtocolFactory::mPlugins;


QTX_END_NAMESPACE
