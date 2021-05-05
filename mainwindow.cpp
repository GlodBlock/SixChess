#include <QMessageBox>
#include <QPainter>
#include "mainwindow.h"
#include "ui_mainwindow.h"

const int BoardMargin = 16;
const int BlockSize = 32;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //开始绘制棋盘
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

//    QPixmap pixmap;
//    painter.drawPixmap(0,0,544,544,pixmap);

    for(int i = 0; i <= BoardSize; i ++)
    {
        painter.drawLine(BoardMargin + BlockSize * i, BoardMargin, BoardMargin + BlockSize * i, BlockSize * ( BoardSize + 1 ) - BoardMargin);
        painter.drawLine(BoardMargin, BoardMargin + BlockSize * i, BlockSize * ( BoardSize + 1 ) - BoardMargin, BoardMargin + BlockSize * i);
    }
}
