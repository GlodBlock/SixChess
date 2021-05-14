#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "util.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    //画棋盘
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //落子
    void mouseReleaseEvent(QMouseEvent *event);

private:
    //初始化
    void initPVP();
    void initPVE();
    void initEVE();

    //游戏进程
    void GameOver(Player player);

    //鼠标悬浮位置
    int clickX, clickY;

    //是否为人
    bool isHuman;

    Util *game; //游戏本体指针

private slots:
    void on_pushButton_clicked(bool checked);

    void on_pushButton_2_clicked(bool checked);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
