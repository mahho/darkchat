#include "konfiguracja.h"
#include "ui_konfiguracja.h"

konfiguracja::konfiguracja(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::konfiguracja)
{
    ui->setupUi(this);

    settings = new QSettings(QApplication::applicationDirPath() + "/darkchat.ini",QSettings::IniFormat);
    colordialog = new QColorDialog();

    // wczytywanie ustawien
}

konfiguracja::~konfiguracja()
{
    delete colordialog;
    delete settings;
    delete ui;
}

void konfiguracja::on_ZapiszB_clicked()
{
    // zapisywanie ustawien
    settings->setValue("ksywka", ui->ksywka->text());
    if (colorchanged)
        settings->setValue("kolor", newcolor);
    settings->setValue("serverIP", ui->adresip->text());

    settings->setValue("print_time", ui->czasnadejscia->isChecked());
// okna automatycznie    settings->setValue();

    settings->setValue("defdir", ui->sciezkazapisu->text());
    settings->setValue("zajecie", ui->nieprzeszkadzacbo->text());

    settings->setValue("archiwizuj", ui->archiwizacja->isChecked());
    settings->setValue("czuwajwglownym", ui->czuwanie->isChecked());
    settings->setValue("autouruchamianie", ui->autostart->isChecked());

    #ifdef Q_OS_WIN
         QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
         if (ui->autostart->isChecked()) {
            settings.setValue("Darkchat", QApplication::applicationDirPath() + "\\darkchat.exe");
         } else {
             settings.remove("Darkchat");
         }
    #endif


    emit konfupdated();
}

void konfiguracja::checkconfig() {
    QFile *konfig = new QFile(QApplication::applicationDirPath() + "/darkchat.ini");
    if (!konfig->exists()) {
        settings->setValue("ksywka", "Gosc");
        settings->setValue("serverIP", "94.42.146.26");
        settings->setValue("zajety", 0);
        settings->setValue("zajecie", "pracuje");
        settings->setValue("defdir", QApplication::applicationDirPath());
        settings->setValue("archiwizuj", 0);
        settings->setValue("czuwaj", 0);
        settings->setValue("czuwajwglownym", 0);
        settings->setValue("autouruchamianie", 1);
        settings->setValue("kolor", "");
        settings->setValue("print_time", 1);
        settings->setValue("pozx", 1);
        settings->setValue("pozy", 1);
        settings->setValue("szer", 488);
        settings->setValue("wys", 336);

        #ifdef  Q_OS_WIN
            QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            settings.setValue("Darkchat", QApplication::applicationDirPath() + "\\darkchat.exe");
        #endif
    }
}

void konfiguracja::configread() {
    ui->ksywka->setText(settings->value("ksywka").toString());
    ui->kolorfont->setText("<font color=\"" + settings->value("kolor").toString() + "\">AaBbCcDdEeFfGg</font>");
    ui->adresip->setText(settings->value("serverIP").toString());
    ui->archiwizacja->setChecked(settings->value("archiwizuj").toBool());
    ui->czuwanie->setChecked(settings->value("czuwaj").toBool());
    ui->autostart->setChecked(settings->value("autouruchamianie").toBool());
    ui->czasnadejscia->setChecked(settings->value("print_time").toBool());
    ui->nieprzeszkadzacbo->setText(settings->value("zajecie").toString());
    ui->sciezkazapisu->setText(settings->value("defdir").toString());

}

void konfiguracja::colorupdated() {
    QColor mycolor = colordialog->currentColor();
    newcolor = mycolor.name();
    colorchanged = true;
    ui->kolorfont->setText("<font color=\""+newcolor+"\">AaBbCcDdEeFfGg</font>");
}

void konfiguracja::on_kolor_clicked()
{
    QColor mycolor = settings->value("kolor").toString();
    colordialog->setCurrentColor(mycolor);
    colordialog->show();
    connect(colordialog, SIGNAL(accepted()), this, SLOT(colorupdated()));
}

void konfiguracja::on_sciezka_clicked()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Wybierz folder"),
                                                          "",
                                                          options);
    if (!directory.isEmpty())
        ui->sciezkazapisu->setText(directory);
}
