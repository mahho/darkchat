#ifndef DARKCHAT_H
#define DARKCHAT_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QtGui/QCloseEvent>
#include <QSystemTrayIcon>

#include "konfiguracja.h"
#include "game.h"

#define NORMAL 0
#define RED 1

namespace Ui {
    class darkchat;
}
class QCloseEvent;//forward declaration

class darkchat : public QMainWindow
{
    Q_OBJECT

    QWidget *opcje;
    konfiguracja *konfig;

public:
    explicit darkchat(QWidget *parent = 0);
    ~darkchat();

protected:
        void closeEvent(QCloseEvent *event);
        void keyPressEvent ( QKeyEvent * e );
        void changeEvent(QEvent *event);

private:
    Ui::darkchat *ui;
    void createTrayIcon();
    void createActions();
    void sendMessage(QString message);

    QString myNick, myIP, myColor, myLogPath;
    QTcpSocket *socket;
    QSettings *sett;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QIcon ico, icored, icogreen;
    QFile myLogFile;
    QTextStream toLogFile;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    bool visible, show_clock, busy;

    game *gra;

private slots:
    void on_actionGra_activated();
    void on_actionZajety_changed();
    void on_actionKonfiguracja_activated();
    void on_actionOProgramie_activated();
    void on_actionZakoncz_activated();
    void on_users_clicked(QModelIndex index);
    void on_linia_returnPressed();
    void clientConnected();
    void disconnected();
    void clientReadWrite();
    void konfigupdated();
    void iconclicked(QSystemTrayIcon::ActivationReason reason);
    void changeIcon(bool type);
    void snejkkoniec();
};

#endif // DARKCHAT_H
