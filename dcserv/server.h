#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include <QRegExp>

class server : public QObject
{
    Q_OBJECT
public:
    explicit server(QObject *parent = 0);


signals:

public slots:
    void newClient();
    void onDisconection();
    void serverReadWrite();
    void clientDisconnected();
    void refresh();

private:
    QTcpServer *myServer;
    QSet<QTcpSocket *> hosts;
    QList<QByteArray> myNicks;
    QMap<QTcpSocket *, QString> myUsers;
};

#endif // SERVER_H
