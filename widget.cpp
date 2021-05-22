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
    //初始化获胜计数
    blackWin = 0;
    whiteWin = 0;
    //开启鼠标悬浮追踪
    setMouseTracking(true);
    QString localIP = getLocalIP();
    this->setWindowTitle("SixChess--IP:" + localIP);
    tcpServer = new QTcpServer(this);
    tcpSocket = new QTcpSocket(this);
    tcpClient = new QTcpSocket(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnection()));
    connect(tcpClient,SIGNAL(connected()),this,SLOT(onClientConnect()));
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(getClientData()));
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
    game->initGame(PVP);
    clickX = -1;
    clickY = -1;
    QMessageBox chosCol(QMessageBox::Information, tr(""), tr("选择匹配/创建"));
    QPushButton *button1= (chosCol.addButton(tr("创建"), QMessageBox::AcceptRole));
    QPushButton *button2= (chosCol.addButton(tr("匹配"), QMessageBox::YesRole));
    chosCol.exec();
    if(chosCol.clickedButton() == button1) isHost = true;
    if(chosCol.clickedButton() == button2) isHost = false;
    if(isHost){
        tcpServer->listen(QHostAddress::LocalHost,PORT);
        isHuman = true;
    }
    else{
        tcpClient->connectToHost(QHostAddress::LocalHost,PORT);
        isHuman = false;
    }
    update();
}

void Widget::initPVE()
{
    //PVE初始化
    game->gameModel = PVE;
    game->gameStatus = RUNNING;
    game->initGame(PVE);
    clickX = -1;
    clickY = -1;
    QMessageBox chosCol(QMessageBox::Information, tr(""), tr("选择颜色"));
    QPushButton *button1= (chosCol.addButton(tr("黑色"), QMessageBox::AcceptRole));
    QPushButton *button2= (chosCol.addButton(tr("白色"), QMessageBox::YesRole));
    chosCol.exec();
    if(chosCol.clickedButton() == button1) isHuman = true;
    if(chosCol.clickedButton() == button2) isHuman = false;
    update();
}

void Widget::initEVE()
{
    //EVE初始化
    game->gameModel = PVE;
    game->gameStatus = RUNNING;
    game->initGame(EVE);
    clickX = -1;
    clickY = -1;
    update();
}

void Widget::paintEvent(QPaintEvent *event)
{
    //开始绘制棋盘
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    QFont font("宋体", 15, true);

    for(int i = 0; i <= BoardSize; i ++)
    {
        painter.drawLine(BoardMargin + BlockSize * i, BoardMargin, BoardMargin + BlockSize * i, BlockSize * ( BoardSize + 1 ) - BoardMargin);
        painter.drawLine(BoardMargin, BoardMargin + BlockSize * i, BlockSize * ( BoardSize + 1 ) - BoardMargin, BoardMargin + BlockSize * i);
    }

    //绘制获胜计数
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(680, 60), 15, 15);
    brush.setColor(Qt::white);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(680, 110), 15, 15);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(710, 70, ": " + QString::number(blackWin));
    painter.drawText(710, 120, ": " + QString::number(whiteWin));


    //游戏未开始时拒绝响应
    if(game->gameStatus == END) return;

    //绘制棋子
    for(int i = 0; i <= BoardSize; i++ )
        for(int j = 0; j <= BoardSize; j++){
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

    //绘制警告点
    if(game->gameModel == PVP || isHuman)
        for(int i = 0; i <= BoardSize; i++ )
            for(int j = 0; j <= BoardSize; j++){
                if(game->warning(QPoint(i ,j))){
                painter.setBrush(QColor(0xb2,0x22,0x22,100));
                painter.drawEllipse(BoardMargin + BlockSize * i - 15, BoardMargin + BlockSize * j - 15, 30, 30);
                }
            }

    //绘制预计落点
    if(clickX != -1 && clickY != -1 && game->gameStatus == RUNNING && isHuman)
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

void Widget::onNewConnection()
{
    QMessageBox::StandardButton btnValue = QMessageBox::information(this, "", "有人加入，可以开始游戏");
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(getData()));
    game->gameStatus = RUNNING;
}

void Widget::onClientConnect()
{
    QMessageBox::StandardButton btnValue = QMessageBox::information(this, "", "成功匹配，可以开始游戏");
    game->gameStatus = RUNNING;
}

void Widget::getData()
{
    isHuman = true;
    char buff[10];
    tcpSocket->read(buff,2);
    if(game->balanceBreaker(QPoint(buff[0],buff[1]))) GameOver(WHITE);
    game->turnHuman(QPoint(buff[0],buff[1]));
    if(game->isWin() != FAKE)
        GameOver(game->isWin());
    if(game->isFull())
        GameOver(FAKE);

}

void Widget::getClientData()
{
    isHuman = true;
    char buff[10];
    tcpClient->read(buff,2);
    if(game->balanceBreaker(QPoint(buff[0],buff[1]))) GameOver(WHITE);
    game->turnHuman(QPoint(buff[0],buff[1]));
    if(game->isWin() != FAKE)
        GameOver(game->isWin());
    if(game->isFull())
        GameOver(FAKE);

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
        for(int i = 0; i <= BoardSize && flag; i ++)
            for(int j = 0; j <= BoardSize && flag; j ++)
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

    //bot下棋
    if(game->gameModel == PVE && !isHuman && game->gameStatus == RUNNING){
        isHuman = true;
        game->turnRobot();
        //获胜
        if(game->isWin() != FAKE)
            GameOver(game->isWin());
        if(game->isFull())
            GameOver(FAKE);
    }

    if(game->gameModel == EVE && game->gameStatus == RUNNING){
        game->turnRobot();
        if(game->isWin() != FAKE)
            GameOver(game->isWin());
        if(game->isFull())
            GameOver(FAKE);
    }
    update();
}

void Widget::mouseReleaseEvent(QMouseEvent *event){
    //游戏未开始时拒绝响应
    if(game->gameStatus == END) return;
    //平局判定
    if(game->isFull()) GameOver(FAKE);
    //PVP模式
    if(game->gameModel == PVP && isHuman){
        if(clickX != -1 && clickY != -1 && game->ChessStatus[clickX][clickY] == 0){
             if(game->balanceBreaker(QPoint(clickX, clickY))) GameOver(WHITE);
             game->turnHuman(QPoint(clickX, clickY));
             isHuman = false;
             char str[2];
             str[0] = clickX;
             str[1] = clickY;
             if(isHost) tcpSocket->write(str,2);
             else tcpClient->write(str,2);
             //获胜
             if(game->isWin() != FAKE)
                 GameOver(game->isWin());
             if(game->isFull())
                 GameOver(FAKE);
        }
    }
    //PVE模式
    if(game->gameModel == PVE){
        if(clickX != -1 && clickY != -1 && game->ChessStatus[clickX][clickY] == 0){
            if(isHuman) game->turnHuman(QPoint(clickX, clickY)), isHuman = false;
             //获胜
             if(game->isWin() != FAKE)
                 GameOver(game->isWin());
             if(game->isFull())
                 GameOver(FAKE);
        }
    }
}

void Widget::GameOver(Player player){
    //游戏结束
    if(player == WHITE){
        whiteWin ++;
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "恭喜", "白方获胜!");
    }
    if(player == BLACK){
        blackWin ++;
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "恭喜", "黑方获胜!");
    }
    if(player == FAKE)
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "", "平局");
    game->gameStatus = END;
    if(!isHost) tcpClient->disconnectFromHost();
    else tcpServer->close();
}

QString Widget::getLocalIP()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QString localIP = "";
    QList<QHostAddress> addList = hostInfo.addresses();

    if(!addList.isEmpty())
        for(int i = 0; i < addList.count(); i ++){
            QHostAddress aHost = addList.at(i);
            if(QAbstractSocket::IPv4Protocol == aHost.protocol())
            {
                localIP = aHost.toString();
                break;
            }
        }
    return localIP;
}

void Widget::on_pushButton_clicked(bool checked)
{
    //开始PVP模式
    if(game->gameStatus == END)
        initPVP();
    else
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "WARN", "当前游戏未结束");
}


void Widget::on_pushButton_2_clicked(bool checked)
{
    //开始PVE模式
    if(game->gameStatus == END)
        initPVE();
    else
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "WARN", "当前游戏未结束");
}

void Widget::on_pushButton_4_clicked(bool checked)
{
    if(game->gameStatus == END)
        initEVE();
    else
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "WARN", "当前游戏未结束");
}
