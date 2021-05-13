#include <utility>
#include <stdlib.h>
#include <time.h>
#include "util.h"

const int dx8[]{-1, 0, 1, 1, 1, 0,-1,-1};
const int dy8[]{-1,-1,-1, 0, 1, 1, 1, 0};

Util::Util()
{
//我蒙古里
}

void Util::initGame(GameModel _gameModel)
{
    //棋盘初始化
    gameModel = _gameModel;
    player = BLACK;
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++)
        {
            ChessStatus[i][j] = 0;
        }
    //PVE模式下，棋盘权重初始化
    if (gameModel == PVE)
    {
        for (int i = 0; i <= BoardSize; i ++)
            for (int j = 0; j <= BoardSize; j++)
            {
                BotRefer[i][j] = 0;
            }
    }
}

void Util::update(QPoint pos)
{
    //修改棋盘状态
    if (player == BLACK)
        ChessStatus[pos.x()][pos.y()] = 1;
    else
        ChessStatus[pos.x()][pos.y()] = -1;
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

Player Util::isWin(){
    //用dfs寻找连成6子的一方
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++){
            if(ChessStatus[i][j] != 0)
                for(int k = 0; k < 8; k ++){
                    if(_dfsConnect(i, j, ChessStatus[i][j], 6, k)){
                       if(ChessStatus[i][j] == 1) return BLACK;
                       if(ChessStatus[i][j] == -1) return WHITE;
                    }
                }
        }
    return FAKE;
}

bool Util::_dfsConnect(int rx, int ry, int col, int dep, int type){
    //搜索
    if(dep == 1) return true;
    int x = rx + dx8[type];
    int y = ry + dy8[type];
    if(x >= 0 && x <= BoardSize && y >= 0 && y <= BoardSize){
        if(col == ChessStatus[x][y]){
            if(_dfsConnect(x, y, col, dep - 1, type)) return true;
        }
    }
    return false;
}

bool Util::warning()
{
    //to-do
    return false;
}

bool Util::isFull()
{
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++){
            if(ChessStatus[i][j] == 0) return false;
        }
    return true;
}
