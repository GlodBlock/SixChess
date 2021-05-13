#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include "widget.h"
#include "ui_widget.h"

const int BoardMargin = 16;
const int BlockSize = 32;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //固定窗口大小
    setFixedSize(1025,686);
    //建立游戏实例
    game = new Util;
    game->gameStatus = END;
    //开启鼠标悬浮追踪
    setMouseTracking(true);
}

Widget::~Widget()
{
    delete ui;
    //结束时释放内存
    delete game;
}

void Widget::initPVP()
{
    //PVP初始化
    game->gameModel = PVP;
    game->gameStatus = RUNNING;
    game->initGame(PVP);
    clickX = -1;
    clickY = -1;
    update();
}


void Widget::paintEvent(QPaintEvent *event)
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

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    //绘制棋子
    for(int i = 0; i <= 15; i++ )
        for(int j = 0; j <= 15; j++){
            if(game->ChessStatus[i][j] == 1){
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(BoardMargin + BlockSize * i - 15, BoardMargin + BlockSize * j - 15, 30, 30);
            }
            if(game->ChessStatus[i][j] == -1){
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(BoardMargin + BlockSize * i - 15, BoardMargin + BlockSize * j - 15, 30, 30);
            }
        }

    //绘制预计落点
    if(clickX != -1 && clickY != -1 && game->gameStatus == RUNNING)
    {
        if(game->ChessStatus[clickX][clickY] != 0) return;
        if(game->player == BLACK)
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawRect(BoardMargin + BlockSize * clickX - 3, BoardMargin + BlockSize * clickY - 3, 6, 6);
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //获取鼠标坐标
    int PosX = event->x();
    int PosY = event->y();

    clickX = -1;
    clickY = -1;

    //判断鼠标是否在棋盘内
    if(PosX >= BoardMargin && PosY >= BoardMargin && PosX <= BlockSize * ( BoardSize + 1 ) - BoardMargin && PosY <= BlockSize * ( BoardSize + 1 ) - BoardMargin)
    {
        float esp = 15;
        bool flag = true;
        for(int i = 0; i <= 15 && flag; i ++)
            for(int j = 0; j <= 15 && flag; j ++)
            {
                float x = BoardMargin + BlockSize * i;
                float y = BoardMargin + BlockSize * j;
                if(sqrt((x - PosX) * (x - PosX) + (y - PosY) * (y - PosY)) < esp)
                {
                    flag = false;
                    clickX = i;
                    clickY = j;
                }
            }
    }
    update();
}

void Widget::mouseReleaseEvent(QMouseEvent *event){
    //游戏未开始时拒绝响应
    if(game->gameStatus == END) return;
    //平局判定
    if(game->isFull()) GameOver(FAKE);
    //PVP模式
    if(game->gameModel == PVP){
        if(clickX != -1 && clickY != -1 && game->ChessStatus[clickX][clickY] == 0){
             game->turnHuman(QPoint(clickX, clickY));
             //获胜
             if(game->isWin() != FAKE)
                 GameOver(game->isWin());
        }
    }
}

void Widget::GameOver(Player player){
    //游戏结束
    if(player == WHITE)
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "恭喜", "白方获胜!");
    if(player == BLACK)
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "恭喜", "黑方获胜!");
    if(player == FAKE)
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "", "平局");
    game->gameStatus = END;
}

void Widget::on_pushButton_clicked(bool checked)
{
    //开始PVP模式
    if(game->gameStatus == END)
        initPVP();
    else
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "WARN", "当前游戏未结束");
}

