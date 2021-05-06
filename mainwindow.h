#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "util.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;

protected:
    //画棋盘
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    //初始化
    void initPVP();
    void initPVE();
    void initEVE();

    //鼠标悬浮位置
    int clickX, clickY;

    Util *game; //游戏本体指针


private slots:

    void on_pushButton_clicked(bool checked);
};

#endif // MAINWINDOW_H
