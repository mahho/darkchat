#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <QFrame>
#include <QPainter>
#include <QLabel>

#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

struct body {
    int x;
    int y;
};

namespace Ui {
    class game;
}

class game : public QWidget
{
    Q_OBJECT

public:
    explicit game(QWidget *parent = 0);
    ~game();
    int getPoints();

protected:
    void keyPressEvent ( QKeyEvent * e );
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Ui::game *ui;
    QList<body> myBody;
    QList<body> lastpos;
    QPainter *myGraphicHead;
    QPainter *myGraphicBody;
    body myHead;
    body myHeadsLastPos;

    short int myDirection;
    QTimer *timer;
    void koniec();
    bool gameended;
    bool newapple;
    QPainter *myGraphicApple;
    body myApple;
    int myPoints;

private slots:
    void update();

signals:
    void endgame();

};

#endif // GAME_H
