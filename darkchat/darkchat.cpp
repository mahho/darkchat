#include "darkchat.h"
#include "ui_darkchat.h"
#include <QSettings>

darkchat::darkchat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::darkchat)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    ui->setupUi(this);
    ui->chat->setReadOnly(true);
    ui->chat->setContentsMargins(0, 0, 0, 0);
    ui->linia->setContentsMargins(0, 0, 0, 0);
//    ui->users->setReadOnly(true);

    // tworzymy tray ikonke
    createActions(); // akcje //! TODO
    createTrayIcon();
    ico.addPixmap(QPixmap(QString::fromUtf8(":/dc.png")), QIcon::Normal, QIcon::Off);
    icored.addPixmap(QPixmap(QString::fromUtf8(":/dcred.png")), QIcon::Normal, QIcon::Off);
    icogreen.addPixmap(QPixmap(QString::fromUtf8(":/dcgreen.png")), QIcon::Normal, QIcon::Off);

    trayIcon->setIcon(ico);
    trayIcon->show();
    visible = true;

    // ustawienia
    konfiguracja *konfig = new konfiguracja();
    konfig->checkconfig();
    delete konfig;

    sett = new QSettings(QApplication::applicationDirPath() + "/darkchat.ini",QSettings::IniFormat);
    myNick = sett->value("ksywka").toString();
    setWindowTitle("Darkchat - " + myNick);
    myIP = sett->value("serverIP").toString();
    myColor = sett->value("kolor").toString();
    show_clock = sett->value("print_time").toBool();
    busy = sett->value("zajety").toBool();
    QString logpath = sett->value("defdir").toString();
    if (logpath.isEmpty())
        logpath = QApplication::applicationDirPath();

    myLogPath = logpath + "/logs/";
    QDir dir(myLogPath);
    if (!dir.exists(myLogPath))
        dir.mkdir(myLogPath);

    myLogFile.setFileName(myLogPath + "/pub.txt");
    myLogFile.open(QIODevice::Append | QIODevice::Text);

    // wiadomosc poczatkowa
    QString welcome ("\nDarkChat 0.3  by mahho,  <a href=\"http://mahho.net/darkchat\">http://mahho.net/darkchat</a> \n");
    if (sett->value("archiwizuj").toBool())
        myLogFile.write(welcome.toUtf8());

    ui->chat->append("<br><font color=\"grey\">" + welcome.toUtf8() + "</font><br>");
    ui->linia->setFocus();

    QString connecting = "Lacze do serwera: " + myIP + "...\n";
    ui->chat->append("<font color =\"grey\">" + connecting + "</font>");
    if (sett->value("archiwizuj").toBool())
        myLogFile.write(connecting.toUtf8());

    // otwieramy socketa
    socket = new QTcpSocket(this);
    socket->connectToHost(myIP, 9753);
//    socket->waitForConnec ted(1000);

    connect(socket, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(socket, SIGNAL( readyRead() ), this, SLOT( clientReadWrite() ) );
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconclicked(QSystemTrayIcon::ActivationReason)));
}

darkchat::~darkchat()
{
    socket->disconnectFromHost();
    delete ui;
}

void darkchat::clientReadWrite() {
    while (socket->canReadLine()) {
        // wiadomosc z serwera
        QString message = QString::fromUtf8(socket->readLine()).trimmed();

        // czy ma byc pokazany zegarek?
        QString clock;
        if (show_clock)
            clock = "[" + QTime::currentTime().toString() + "] ";

        // regexpy
        QRegExp users("^(.*)<users>(.*)</users>(.*)$");
        QRegExp grey("^<info>(.*)</info>$");
        QRegExp ping("^/ping$");

        if (users.indexIn(message) != -1) {
            // parsujemy liste userow sciagnieta z serwera
            QStringList userslist = users.cap(2).split(",");
            ui->users->clear();
            foreach (QString user, userslist) {
                ui->users->addItem(user);
            }

        } else if (grey.indexIn(message) != -1) {
            // dostajemy informacje od serwera
            ui->chat->append(QString("<font color=\"grey\">" + clock + grey.cap(1) + "</font>"));
        } else if (ping.indexIn(message) != -1) {
            // dostajemy pinga, na razie nic z tym nie robimy :)
        } else {
            // dostajemy wiadomosc, dekodujemy ja i wyswietlamy w oknie chata
            QRegExp mymsg("^&lt;(.*)&gt; (.*)$");
            mymsg.indexIn(message);
            QString decipheredmsg = QString(QByteArray::fromHex(QByteArray::fromBase64(QByteArray::fromHex(mymsg.cap(2).toUtf8()))));

            // wyciagamy czcionke z rozsszyfrowanej wiadomosci i kolorujemy wszystko
            QRegExp decoded("^<font color=\"(.*)\">(.*)</font>$");
            if (decoded.indexIn(decipheredmsg) != -1) {
                QString message = "<font color=\""+decoded.cap(1)+"\">" + clock + "&lt;"+ mymsg.cap(1) + "&gt;" + decoded.cap(2) + "</font>";
                ui->chat->append(message);

                // logujemy do pliku?
                if (sett->value("archiwizuj").toBool()) {
                    myLogFile.write(QString(clock +  "<" + mymsg.cap(1) + ">" + decoded.cap(2) + "\n").toUtf8());
                }
            }
            if (!visible && !busy) {
                changeIcon(RED);
                QApplication::beep();
            }
        }
    }
}

void darkchat::on_linia_returnPressed()
{
    QRegExp help("/help");
    QRegExp nickreg("^/nick (.*)$");
    QRegExp quit("/quit");

    if (help.indexIn(ui->linia->text().toUtf8()) != -1) {
        ui->chat->append("<font color=\"grey\">/help - pomoc</font>");
        ui->chat->append("<font color=\"grey\">/nick ksywa - ustawia ksywe</font>");
        ui->chat->append("<font color=\"grey\">/quit - wyjscie</font>");
    } else if (nickreg.indexIn(ui->linia->text()) != -1) {
        QString send = "/nick " + myNick + " " + nickreg.cap(1) + "\n";
        socket->write(send.toUtf8());
        sett->setValue("ksywka", nickreg.cap(1));
    } else if (quit.indexIn(ui->linia->text()) != -1) {
        this->close();
    } else {
        sendMessage(ui->linia->text().toUtf8());
    }
    ui->linia->clear();
}

void darkchat::sendMessage(QString message) {
    QString formattedline = "<font color=\"" + myColor + "\"> " + message + "</font>";
    QString tosend = "/msg " + QByteArray(formattedline.toUtf8()).toHex().toBase64().toHex() + "\n";
    socket->write(tosend.toUtf8());
}

// nasz klient podlaczyl sie do serwera
void darkchat::clientConnected() {
    ui->chat->append("<font color=\"grey\">Polaczony...</font>");

    if (sett->value("archiwizuj").toBool())
        myLogFile.write("Polaczony... \n");

    QString join = "/join " + myNick + "\n";
    socket->write(join.toUtf8());
}

// klient rozlaczony
void darkchat::disconnected() {
    ui->chat->append("<B>Rozlaczony z serwerem...</B>");
}

// klikniecie w liste userow
void darkchat::on_users_clicked(QModelIndex index)
{
    QString who = index.data().toString();
//    qDebug() << index.internalPointer();
    if (who == "(/)") {
        ui->info->setText(">Wszyscy<");
    }
    else {
        ui->info->setText(QString(who + "\nAdres IP"));
    }
}

// mamy konczyc?
void darkchat::on_actionZakoncz_activated()
{
    int question = QMessageBox::question(this, "DarkChat", "Na pewno chcesz wyjsc?", QMessageBox::Yes, QMessageBox::No);
    if (question == QMessageBox::Yes)
       QApplication::quit();
}

// o programie
void darkchat::on_actionOProgramie_activated()
{
    QMessageBox::about(this, tr("DarkChat"),
                tr("DarkChat v 0.1\n"
                   "Created by mahho\n"
                   "http://mahho.net/darkchat (soon)\n"
                   "E-mail: mahho@mahho.net"));
}

// event zamkniecia, pytamy czy na pewno wyjsc
void darkchat::closeEvent(QCloseEvent *event) {
//    int question = QMessageBox::question(this, "DarkChat", "Na pewno chcesz wyjsc?", QMessageBox::Yes, QMessageBox::No);
 //   if (question == QMessageBox::No)
    visible = false;
    this->hide();
    event->ignore();
}

// wcisniety klawisz (escape)
void darkchat::keyPressEvent ( QKeyEvent * e )  {
    if (e->key() == Qt::Key_Escape) {
        visible = false;
        this->hide();
    }
}

// event - minimalizacja okna, zamiast do taskbara, to do traya
void darkchat::changeEvent ( QEvent *event )
{
    if( event->type() == QEvent::WindowStateChange ) {
        if( isMinimized() ) {
            visible = false;
            hide();
            setWindowFlags(Qt::FramelessWindowHint);
            event->ignore();
        }
    }
}

// odpalamy konfiguracje
void darkchat::on_actionKonfiguracja_activated()
{
    konfig = new konfiguracja();
    konfig->configread();
    konfig->show();
    connect(konfig, SIGNAL(konfupdated()), this, SLOT(konfigupdated()));
}

// emit z konfiguracji, nick do zmiany.
void darkchat::konfigupdated() {
    QString newNick = sett->value("ksywka").toString();
    if (myNick != newNick) {
        socket->write(QString("/nick " + myNick + " " + newNick + "\n").toUtf8());
        setWindowTitle("Darkchat - " + myNick);
    }

    myColor = sett->value("kolor").toString();
    show_clock = sett->value("print_time").toBool();
}

// tworzymy akcje dla trayikonki
void darkchat::createActions()
{
    /*
     <b>DarkChat</b>
     ---------------
     [ ] Czuwanie
     [ ] Zajêty
         Konfiguracja
         O Programie
     ---------------
         Zakoñcz
     */

    restoreAction = new QAction(tr("DarkChat"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

//    minimizeAction = new QAction(tr("Mi&nimize"), this);
//    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    quitAction = new QAction(tr("&Zakoncz"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(on_actionZakoncz_activated()));

}

// a tu tworzymy trayikonke // TODO
void darkchat::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    changeIcon(NORMAL);
}

// mamy klikniecie na ikonke w trayu,
// jesli darkchat schowany, przywracamy go
// jesli na wierzchu, chowamy
void darkchat::iconclicked(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (visible == true) {
            this->hide();
            visible = false;
        } else {
            setWindowFlags(Qt::Window);
            showNormal();
            this->show();
            this->activateWindow();
            visible = true;
            changeIcon(NORMAL);
        }
    }
}

// zmieniamy ikonke po otrzymaniu wiadomosci
// czerwona, gdy darkchat siedzi w trayu i jest nowa wiadomosc
// biala, gdy byla ikonka czerwona i darkchat zostal przywrocony
void darkchat::changeIcon(bool type) {

    if (!busy) {
        switch (type) {
            case NORMAL:
                trayIcon->setIcon(ico);
                break;
            case RED:
                trayIcon->setIcon(icored);
                break;
        }
    }
}

void darkchat::on_actionZajety_changed()
{
    if (ui->actionZajety->isChecked()) {
        socket->write(QString("/nick " + myNick + " " + myNick+"|x|\n").toUtf8());
        setWindowTitle("Darkchat - " + myNick + "|x|");
        trayIcon->setIcon(icogreen);
        busy = true;
        sett->setValue("zajety", true);
    } else {
        socket->write(QString("/nick " + myNick+"|x| " + myNick + "\n").toUtf8());
        setWindowTitle("Darkchat - " + myNick);
        trayIcon->setIcon(ico);
        busy = false;
        sett->setValue("zajety", false);
    }
}

// snejk
void darkchat::on_actionGra_activated() {
    gra = new game();
    gra->show();
    connect(gra, SIGNAL(endgame()), this, SLOT(snejkkoniec()));
}

void darkchat::snejkkoniec() {
    sendMessage(QString("<font color=\"grey\"><b>--== Zdobylem w snejka " + QString().setNum(gra->getPoints()) + " punktow ==--</b></font>").toUtf8());
}
