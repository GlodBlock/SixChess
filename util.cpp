#include <utility>
#include <stdlib.h>
#include <time.h>
#include "util.h"

Util::Util()
{
//我蒙古里
}

bool operator <(QPoint pos1,QPoint pos2)
{
    return pos1.x() == pos2.x() ? pos1.y() < pos2.y() : pos1.x() < pos2.x();
}

void Util::initGame(GameModel _gameModel)
{
    //棋盘初始化
    gameModel = _gameModel;
    player = BLACK;
    ChessStatus.clear();
    for (int i = 1; i <= BoardSize; i ++)
        for (int j = 1; j <= BoardSize; j++)
        {
            ChessStatus.insert(QPoint(i, j), 0);
        }
    //PVE模式下，棋盘权重初始化
    if (gameModel == PVE)
    {
        BotRefer.clear();
        for (int i = 1; i <= BoardSize; i ++)
            for (int j = 1; j <= BoardSize; j++)
            {
                BotRefer.insert(QPoint(i, j), 0);
            }
    }
}

void Util::update(QPoint pos)
{
    //修改棋盘状态
    if (player == BLACK)
        ChessStatus.insert(pos, 1);
    else
        ChessStatus.insert(pos, -1);
    //换人
    if (player == BLACK) player = WHITE;
        else player = BLACK;
}

void Util::turnHuman(QPoint pos)
{
    //人类下棋
    update(pos);
}

void Util::turnRobot(QPoint pos)
{
    //计算棋盘权重
    calWeight();
    //to-do
    update(pos);
}

void Util::calWeight()
{
    //to-do
}

bool Util::balanceBreaker()
{
    //to-do
    return false;
}

bool Util::warning()
{
    //to-do
    return false;
}

bool Util::isFull()
{
    for (QMap<QPoint, int>::iterator it = ChessStatus.begin(); it != ChessStatus.end(); it ++)
        if (!it.value()) return false;
    return true;
}
