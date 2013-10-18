#ifndef KONFIGURACJA_H
#define KONFIGURACJA_H

#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QColorDialog>
#include <QDir>
#include <QTreeView>
#include <QFileDialog>

namespace Ui {
    class konfiguracja;
}

class konfiguracja : public QWidget
{
    Q_OBJECT

    QSettings *settings;

public:
    explicit konfiguracja(QWidget *parent = 0);
    ~konfiguracja();
    void myupdate();
    void checkconfig();
    void configread();

private:
    Ui::konfiguracja *ui;
    QColorDialog *colordialog;

    QString newcolor;
    bool colorchanged;
    QString nick;

public slots:
    void on_ZapiszB_clicked();

signals:
    void konfupdated();

private slots:
    void on_sciezka_clicked();
    void on_kolor_clicked();
    void colorupdated();
};

#endif // KONFIGURACJA_H
