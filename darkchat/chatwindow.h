#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>

namespace Ui {
    class chatwindow;
}

class chatwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit chatwindow(QWidget *parent = 0);
    ~chatwindow();

private:
    Ui::chatwindow *ui;
};

#endif // CHATWINDOW_H
