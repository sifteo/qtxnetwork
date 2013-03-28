#include "pluggablenetworkaccessmanager.h"
#include "protocolfactory.h"

QTX_BEGIN_NAMESPACE


PluggableNetworkAccessManager::PluggableNetworkAccessManager(QObject *parent /* = 0 */)
    : QNetworkAccessManager(parent)
{
}

PluggableNetworkAccessManager::~PluggableNetworkAccessManager()
{
}

QNetworkReply *PluggableNetworkAccessManager::createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData /* = 0 */)
{
    // Attempt to create a QNetworkReply using protocol plugins.  If this does
    // not yeild a valid instance, then utilize QNetworkAccessManager to handle
    // handle common protocols, such as HTTP and FTP, that are implemented
    // directly by Qt.
    QNetworkReply *reply = ProtocolFactory::createRequest(op, req, outgoingData);
    if (reply) {
        return reply;
    }
    
    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}


QTX_END_NAMESPACE
