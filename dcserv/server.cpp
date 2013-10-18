#include "server.h"

server::server(QObject *parent) :
    QObject(parent)
{
    myServer = new QTcpServer(this);
    myServer->listen(QHostAddress::Any, 9753);

    // timer do odswiezania polaczenia
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start(120000);

    connect(myServer, SIGNAL(newConnection()), this, SLOT(newClient()));
}

void server::newClient() {
    QTcpSocket *client = myServer->nextPendingConnection();

    connect(client, SIGNAL( readyRead() ), this, SLOT( serverReadWrite() ) );
    connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

//   qDebug() << "Hostow: " << hosts.size();
}

void server::serverReadWrite() {
    QTcpSocket *client = (QTcpSocket*)sender();

    while (client->canReadLine()) {
//        qDebug() << "Hostow: " << hosts.size();
        QString message = QString::fromUtf8(client->readLine()).trimmed();
//        qDebug() << message;

        QRegExp users("^(.*)<users>(.*)</users>(.*)$");
        QRegExp msg("^/msg (.*)$");
        QRegExp nick("^/nick (.*) (.*)$");
        QRegExp join("^/join (.*)$");

        if (users.indexIn(message) != -1) {
            // dostajemy liste userow od uzytkownika? oj nieladnie...
        } else if (join.indexIn(message) != -1 && !hosts.contains(client)) {
            // dorzucamy nowego clienta do listy
            hosts.insert(client);

            // wywalamy niepotrzebne smieci
            QString mynick = join.cap(1);
            mynick.remove(",");
            mynick.remove(" ");
            mynick.remove("\n");
            mynick.remove("<br>");

            // dodajemy do listy
            myUsers[client] = mynick;

            // tworzymy liste userow podlaczonych do serwera
            QStringList userList, ipList;
            userList << "(/)";
            foreach(QString user, myUsers.values()) {
                userList << user;
            }
            foreach(QTcpSocket *clients, hosts) {
                ipList << clients->peerAddress().toString();
//                qDebug() << clients->peerAddress().toString();
            }

            // wysylamy info o dolaczeniu sie nowej osoby i liste nickow
            foreach (QTcpSocket *clients, hosts) {
                clients->write(QString("<info>" + mynick + " sie pojawil (" + client->peerAddress().toString()  +")</info>\n").toUtf8());
                clients->write(QString("<users>" + userList.join(",") + "</users>\n").toUtf8());
//                clients->write(QString("<ip>" + ipList.join(",") + "</ip>\n").toUtf8());
            }

        } else if (nick.indexIn(message) != -1) {
        // zmiana nicka
            QString oldnick = myUsers[client];
            myUsers[client].clear();
            myUsers[client] = nick.cap(2);

            QStringList userList, ipList;
            userList << "(/)";
            foreach(QString user, myUsers.values()) {
                userList << user;
            }
            foreach (QTcpSocket *clients, hosts) {
                clients->write(QString("<info>" + oldnick + " zmienil ksywe na " + nick.cap(2) +"</info>\n").toUtf8());
                clients->write(QString("<users>" + userList.join(",") + "</users>\n").toUtf8());
//                clients->write(QString("<ip>" + ipList.join(",") + "</ip>\n").toUtf8());
            }
        } else if (msg.indexIn(message) != -1) {
            // Wysylamy wiadomosc do wszystkich
            QString message = "&lt;" + myUsers[client].toUtf8() + "&gt; " + msg.cap(1) + "\n";
            foreach (QTcpSocket *clients, hosts) {
                clients->write(message.toUtf8());
//                qDebug() << message;
            }
        } // else
//            qDebug() << "Zle dane";
    }

}

void server::clientDisconnected() {
    QTcpSocket *client = (QTcpSocket*)sender();
    hosts.remove(client);
    QString nick = myUsers.take(client);
    myUsers.remove(client);

    QStringList userList;
    userList << "(/)";
    foreach(QString user, myUsers.values())
        userList << user;

    foreach (QTcpSocket *clients, hosts) {
        clients->write(QString("<info>" + nick + " wyszedl.</info>\n").toUtf8());
        clients->write(QString("<users>" + userList.join(",") + "</users>\n").toUtf8());
    }
//    qDebug() << "Hostow: " << hosts.size();
}


void server::onDisconection()
{
}

void server::refresh() {
    // odswiezamy polaczenia
    QString message = "/ping\n";
    foreach (QTcpSocket *clients, hosts) {
        clients->write(message.toUtf8());
    }
}
