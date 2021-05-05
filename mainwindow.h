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

private:
    //初始化
    void init();

private slots:

};

#endif // MAINWINDOW_H
