#include "game.h"
#include "ui_game.h"

game::game(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::game)
{
    ui->setupUi(this);

    myHead.x = 320;
    myHead.y = 240;
    myDirection = 0;
    gameended = false;
    myPoints = 0;
    myApple.x = qrand() % 80*8;
    myApple.y = qrand() % 60*8;
    newapple = true;

    myDirection = UP;

    body tail;
    tail.x = myHead.x;
    tail.y = myHead.y+8;
    myBody.append(tail);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
}

game::~game()
{
    delete ui;
}

void game::paintEvent(QPaintEvent *event) {
    if (!gameended) {
        // malujemy "jablko"
        myGraphicApple = new QPainter(this);
        myGraphicApple->fillRect(myApple.x, myApple.y, 8, 8, QBrush(Qt::green));
        delete myGraphicApple;

        // glowa robaczka
        myGraphicHead = new QPainter(this);
        myGraphicHead->fillRect(myHead.x, myHead.y, 8, 8, QBrush(Qt::red));
        delete myGraphicHead;

        foreach (body part, myBody) {
            myGraphicBody = new QPainter(this);
            myGraphicBody->fillRect(part.x, part.y, 8, 8, QBrush(Qt::blue));
            delete myGraphicBody;
        }

    } else {
        ui->label->setText(QString(" K O N I E C \n Suma punktow: " + QString().setNum(myPoints) + " ..."));
    }

}

void game::closeEvent(QCloseEvent *event) {
    if (timer) {
        timer->stop();
        delete timer;
    }
}


void game::keyPressEvent ( QKeyEvent * e )  {
    if (e->key() == Qt::Key_Left)
        myDirection = LEFT;
    else if (e->key() == Qt::Key_Right)
        myDirection = RIGHT;
    else if (e->key() == Qt::Key_Up)
        myDirection = UP;
    else if (e->key() == Qt::Key_Down)
        myDirection = DOWN;

}

// 80x60
void game::update() {
    if (myHead.x == myApple.x && myHead.y == myApple.y) {
        myPoints++;
        myApple.x = qrand() % 80*8;
        myApple.y = qrand() % 60*8;

        // dodajemy cialka
        body tail;
        tail.x = myBody.last().x;
        tail.y = myBody.last().y;
        myBody.append(tail);

        ui->counter->display(myPoints);
    }

    int bodysize = myBody.size();
    if (bodysize > 0) {
        for (int i = bodysize - 1 ; i > 0; i--) {
//            qDebug() << "i: " << i;
//            qDebug() << myBody.at(i).x << " " << myBody.at(i).y;

            myBody[i] = myBody[i-1];
        }
    }
    myBody[0] = myHead;

    switch (myDirection) {
        case LEFT: myHead.x -= 8; break;
        case RIGHT: myHead.x += 8; break;
        case UP: myHead.y -= 8; break;
        case DOWN: myHead.y += 8; break;
    }


//    int bodysize = myBody.size();
//    for int (i = 1; i < bodysize; i++) {
//    }

    if (myHead.x < 0 || myHead.x >= 640 || myHead.y < 0 || myHead.y >= 480)
        koniec();

    foreach (body part, myBody) {
        if (myHead.x == part.x && myHead.y == part.y)
            koniec();
    }

    repaint();
}

void game::koniec() {
    gameended = true;
    timer->stop();
    repaint();
    emit endgame();
}

int game::getPoints() {
    return myPoints;
}

