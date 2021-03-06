#include <stdlib.h>
#include <time.h>
#include <queue>
#include <ctime>
#include <algorithm>
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
    //PVE以及EVE模式下，棋盘权重初始化
    if (gameModel == PVE || gameModel == EVE)
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

void Util::turnRobot()
{
    //计算棋盘权重
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++){
            calWeight(QPoint(i, j));
        }
    std::queue<QPoint> q;
    //找出最大值
    int Max = 0;
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++){
            Max = std::max(Max, BotRefer[i][j]);
        }
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++){
            if(Max == BotRefer[i][j]) q.push(QPoint(i, j));
        }

    //随机取最大值中的一个
    qsrand(time(NULL));
    int x = qrand() % q.size();
    while(x){
        x --;
        q.pop();
    }
    QPoint pos = q.front();
    while(!q.empty()) q.pop();

    //bot首步优化
    if(Max) update(pos);
    else update(QPoint(10,10));
}

void Util::calWeight(QPoint pos)
{
    int me, opp;
    //确定bot棋子颜色
    if(player == BLACK) me = 1, opp = -1;
    else me = -1, opp = 1;
    //权值计算
    /* 010 10
     * 0110 100
     * 01110 300
     * 011110 800
     * 011111 inf
     * 0110110 500
     * 01 5
     * 011 80
     * 0111 280
     * 01111 780
     */
    int x = pos.x();
    int y = pos.y();
    BotRefer[x][y] = 0;
    for(int k = 0; k < 8; k ++){
        if(_dfsConnect(x, y, me, 3, k, "010")) BotRefer[x][y] += 10;
        if(_dfsConnect(x, y, me, 4, k, "0110")) BotRefer[x][y] += 100;
        if(_dfsConnect(x, y, me, 5, k, "01110")) BotRefer[x][y] += 300;
        if(_dfsConnect(x, y, me, 6, k, "011110")) BotRefer[x][y] += 800;
        if(_dfsConnect(x, y, me, 7, k, "0110110")) BotRefer[x][y] += 500;
        if(_dfsConnect(x, y, me, 6, k, "111110") ||
          (_dfsConnect(x, y, me, 6, k, "011110") && _dfsConnect(x, y, me, 2, antiDirc(k), "00")) ||
          (_dfsConnect(x, y, me, 5, k, "01110") && _dfsConnect(x, y, me, 3, antiDirc(k), "010")) ||
          (_dfsConnect(x, y, me, 5, k, "01110") && _dfsConnect(x, y, me, 3, antiDirc(k), "010")) ||
          (_dfsConnect(x, y, me, 4, k, "0110") && _dfsConnect(x, y, me, 4, antiDirc(k), "0110"))
           ) BotRefer[x][y] += 1<<20;
        if((_dfsConnect(x, y, me, 5, k, "11110") && _dfsConnect(x, y, me, 2, antiDirc(k), "00")) ||
           (_dfsConnect(x, y, me, 4, k, "1110") && _dfsConnect(x, y, me, 3, antiDirc(k), "010")) ||
           (_dfsConnect(x, y, me, 5, k, "01110") && _dfsConnect(x, y, me, 2, antiDirc(k), "10")) ||
           (_dfsConnect(x, y, me, 4, k, "0110") && _dfsConnect(x, y, me, 3, antiDirc(k), "110"))
           ) BotRefer[x][y] += 1800;
        if((_dfsConnect(x, y, me, 5, k, "01110") && _dfsConnect(x, y, me, 2, antiDirc(k), "00")) ||
           (_dfsConnect(x, y, me, 4, k, "0110") && _dfsConnect(x, y, me, 3, antiDirc(k), "010")) ||
           (_dfsConnect(x, y, me, 4, k, "0110") && _dfsConnect(x, y, me, 4, antiDirc(k), "0100")) ||
           (_dfsConnect(x, y, me, 5, k, "01100") && _dfsConnect(x, y, me, 3, antiDirc(k), "010"))
           ) BotRefer[x][y] += 2500;
        if(_dfsConnect(x, y, opp, 3, k, "010")) BotRefer[x][y] += 11;
        if(_dfsConnect(x, y, opp, 4, k, "0110")) BotRefer[x][y] += 110;
        if(_dfsConnect(x, y, opp, 5, k, "01110")) BotRefer[x][y] += 330;
        if(_dfsConnect(x, y, opp, 6, k, "011110")) BotRefer[x][y] += 880;
        if(_dfsConnect(x, y, opp, 7, k, "0110110")) BotRefer[x][y] += 550;
        if(_dfsConnect(x, y, opp, 6, k, "111110") ||
          (_dfsConnect(x, y, opp, 6, k, "011110") && _dfsConnect(x, y, opp, 2, antiDirc(k), "00")) ||
          (_dfsConnect(x, y, opp, 5, k, "01110") && _dfsConnect(x, y, opp, 3, antiDirc(k), "010")) ||
          (_dfsConnect(x, y, opp, 5, k, "01110") && _dfsConnect(x, y, opp, 3, antiDirc(k), "010")) ||
          (_dfsConnect(x, y, opp, 4, k, "0110") && _dfsConnect(x, y, opp, 4, antiDirc(k), "0110"))
           ) BotRefer[x][y] += 1<<22;
        if((_dfsConnect(x, y, opp, 5, k, "11110") && _dfsConnect(x, y, opp, 2, antiDirc(k), "00")) ||
           (_dfsConnect(x, y, opp, 4, k, "1110") && _dfsConnect(x, y, opp, 3, antiDirc(k), "010")) ||
           (_dfsConnect(x, y, opp, 5, k, "01110") && _dfsConnect(x, y, opp, 2, antiDirc(k), "10")) ||
           (_dfsConnect(x, y, opp, 4, k, "0110") && _dfsConnect(x, y, opp, 3, antiDirc(k), "110"))
           ) BotRefer[x][y] += 2000;
        if((_dfsConnect(x, y, opp, 5, k, "01110") && _dfsConnect(x, y, opp, 2, antiDirc(k), "00")) ||
           (_dfsConnect(x, y, opp, 4, k, "0110") && _dfsConnect(x, y, opp, 3, antiDirc(k), "010")) ||
           (_dfsConnect(x, y, opp, 4, k, "0110") && _dfsConnect(x, y, opp, 4, antiDirc(k), "0100")) ||
           (_dfsConnect(x, y, opp, 5, k, "01100") && _dfsConnect(x, y, opp, 3, antiDirc(k), "010"))
            ) BotRefer[x][y] += 2800;
        if(_dfsConnect(x, y, me, 2, k, "10")) BotRefer[x][y] += 5;
        if(_dfsConnect(x, y, me, 3, k, "110")) BotRefer[x][y] += 80;
        if(_dfsConnect(x, y, me, 4, k, "1110")) BotRefer[x][y] += 280;
        if(_dfsConnect(x, y, opp, 5, k, "11110")) BotRefer[x][y] += 380;
        if(_dfsConnect(x, y, opp, 2, k, "10")) BotRefer[x][y] += 6;
        if(_dfsConnect(x, y, opp, 3, k, "110")) BotRefer[x][y] += 85;
        if(_dfsConnect(x, y, opp, 4, k, "1110")) BotRefer[x][y] += 285;
        if(_dfsConnect(x, y, opp, 5, k, "11110")) BotRefer[x][y] += 385;
    }
    int cnt1 = 0, cnt2 = 0;
    for(int k = 0; k < 8; k++){
        if(_dfsConnect(x, y, me, 4, k, "1110")) cnt1 ++;
        if(_dfsConnect(x, y, me, 3, k, "110")) cnt2 ++;
    }
    if(cnt1 > 1 || (cnt1 > 0 && cnt2 > 0)) BotRefer[x][y] += 1<<20;
    if(cnt2 > 1) BotRefer[x][y] += 700;
    cnt1 = 0, cnt2 = 0;
    for(int k = 0; k < 8; k++){
        if(_dfsConnect(x, y, opp, 4, k, "1110")) cnt1 ++;
        if(_dfsConnect(x, y, opp, 3, k, "110")) cnt2 ++;
    }
    if(cnt1 > 1 || (cnt1 > 0 && cnt2 > 0)) BotRefer[x][y] += 1<<22;
    if(cnt2 > 1) BotRefer[x][y] += 780;
    if(warning(QPoint(x, y))) BotRefer[x][y] = 1<<30;
    if(balanceBreaker(pos) && player == BLACK) BotRefer[x][y] = 0;
}

Player Util::isWin(){
    //用dfs寻找连成6子的一方
    for (int i = 0; i <= BoardSize; i ++)
        for (int j = 0; j <= BoardSize; j++){
            if(ChessStatus[i][j] != 0)
                for(int k = 0; k < 8; k ++){
                    if(_dfsConnect(i, j, ChessStatus[i][j], 6, k, "111111")){
                       if(ChessStatus[i][j] == 1) return BLACK;
                       if(ChessStatus[i][j] == -1) return WHITE;
                    }
                }
        }
    return FAKE;
}

bool Util::_dfsConnect(int rx, int ry, int col, int dep, int type, QString des){
    //通用搜索
    //‘1’代表相同棋子，‘0’代表空位
    if(dep == 0) return true;
    if(!(rx >= 0 && rx <= BoardSize && ry >= 0 && ry <= BoardSize)) return false;
    if(!((col == ChessStatus[rx][ry] && des[dep - 1] == '1') || (ChessStatus[rx][ry] == 0 && des[dep - 1] == '0'))) return false;
    int x = rx + dx8[type];
    int y = ry + dy8[type];
    if(_dfsConnect(x, y, col, dep - 1, type, des)) return true;
    return false;
}

bool Util::warning(QPoint pos)
{
    //五子警告
    int opp;
    if(balanceBreaker(pos) && player == BLACK) return false;
    if(player == BLACK) opp = -1;
    if(player == WHITE) opp = 1;
    int x = pos.x();
    int y = pos.y();
    for(int k = 0; k < 8; k ++){
        if(ChessStatus[x][y] == 0){
            if(_dfsConnect(x, y, opp, 6, k, "111110")) return true;
        }
    }
    if(ChessStatus[x][y] == 0){
        if(_dfsConnect(x, y, opp, 3, UP, "110") && _dfsConnect(x, y, opp, 4, DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, DOWN, "110") && _dfsConnect(x, y, opp, 4, UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, LEFT, "110") && _dfsConnect(x, y, opp, 4, RIGHT, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, RIGHT, "110") && _dfsConnect(x, y, opp, 4, LEFT, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, LEFT_UP, "110") && _dfsConnect(x, y, opp, 4, RIGHT_DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, RIGHT_DOWN, "110") && _dfsConnect(x, y, opp, 4, LEFT_UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, RIGHT_UP, "110") && _dfsConnect(x, y, opp, 4, LEFT_DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 3, LEFT_DOWN, "110") && _dfsConnect(x, y, opp, 4, RIGHT_UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, UP, "1110") && _dfsConnect(x, y, opp, 4, DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, DOWN, "1110") && _dfsConnect(x, y, opp, 4, UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, LEFT, "1110") && _dfsConnect(x, y, opp, 4, RIGHT, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, RIGHT, "1110") && _dfsConnect(x, y, opp, 4, LEFT, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, LEFT_UP, "1110") && _dfsConnect(x, y, opp, 4, RIGHT_DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, RIGHT_DOWN, "1110") && _dfsConnect(x, y, opp, 4, LEFT_UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, RIGHT_UP, "1110") && _dfsConnect(x, y, opp, 4, LEFT_DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 4, LEFT_DOWN, "1110") && _dfsConnect(x, y, opp, 4, RIGHT_UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, UP, "11110") && _dfsConnect(x, y, opp, 2, DOWN, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, DOWN, "11110") && _dfsConnect(x, y, opp, 2, UP, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT, "11110") && _dfsConnect(x, y, opp, 2, RIGHT, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT, "11110") && _dfsConnect(x, y, opp, 2, LEFT, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT_UP, "11110") && _dfsConnect(x, y, opp, 2, RIGHT_DOWN, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT_DOWN, "11110") && _dfsConnect(x, y, opp, 2, LEFT_UP, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT_UP, "11110") && _dfsConnect(x, y, opp, 2, LEFT_DOWN, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT_DOWN, "11110") && _dfsConnect(x, y, opp, 2, RIGHT_UP, "10")) return true;
        if(_dfsConnect(x, y, opp, 5, UP, "11110") && _dfsConnect(x, y, opp, 3, DOWN, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, DOWN, "11110") && _dfsConnect(x, y, opp, 3, UP, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT, "11110") && _dfsConnect(x, y, opp, 3, RIGHT, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT, "11110") && _dfsConnect(x, y, opp, 3, LEFT, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT_UP, "11110") && _dfsConnect(x, y, opp, 3, RIGHT_DOWN, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT_DOWN, "11110") && _dfsConnect(x, y, opp, 3, LEFT_UP, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT_UP, "11110") && _dfsConnect(x, y, opp, 3, LEFT_DOWN, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT_DOWN, "11110") && _dfsConnect(x, y, opp, 3, RIGHT_UP, "110")) return true;
        if(_dfsConnect(x, y, opp, 5, UP, "11110") && _dfsConnect(x, y, opp, 4, DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, DOWN, "11110") && _dfsConnect(x, y, opp, 4, UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT, "11110") && _dfsConnect(x, y, opp, 4, RIGHT, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT, "11110") && _dfsConnect(x, y, opp, 4, LEFT, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT_UP, "11110") && _dfsConnect(x, y, opp, 4, RIGHT_DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT_DOWN, "11110") && _dfsConnect(x, y, opp, 4, LEFT_UP, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, RIGHT_UP, "11110") && _dfsConnect(x, y, opp, 4, LEFT_DOWN, "1110")) return true;
        if(_dfsConnect(x, y, opp, 5, LEFT_DOWN, "11110") && _dfsConnect(x, y, opp, 4, RIGHT_UP, "1110")) return true;
    }
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

int Util::antiDirc(int x)
{
    switch (x) {
    case UP:
        return DOWN;
        break;
    case DOWN:
        return UP;
        break;
    case LEFT:
        return RIGHT;
        break;
    case RIGHT:
        return LEFT;
        break;
    case RIGHT_UP:
        return LEFT_DOWN;
        break;
    case LEFT_DOWN:
        return RIGHT_UP;
        break;
    case RIGHT_DOWN:
        return LEFT_UP;
        break;
    case LEFT_UP:
        return RIGHT_DOWN;
        break;
    default:
        break;
    }
}

bool Util::balanceBreaker(QPoint pos)
{
    int x = pos.rx();
    int y = pos.ry();
    if(player == WHITE) return false;
    for (int i = 0; i < 8 ;i ++)
        for (int j = 0; j < 8; j ++)
        {
            if(i == j || antiDirc(i) == j) continue;
            //五五禁手
            if(_dfsConnect(x, y, 1, 5, i, "11110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 5, j, "11110") && _dfsConnect(x, y, 1, 2, antiDirc(j), "00")) return true;
            if(_dfsConnect(x, y, 1, 6, i, "011110") && _dfsConnect(x, y, 1, 6, j, "011110")) return true;
            if(_dfsConnect(x, y, 1, 5, i, "11110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 6, j, "011110")) return true;

            if(_dfsConnect(x, y, 1, 6, i, "011110") && _dfsConnect(x, y, 1, 5, j, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(j), "10")) return true;
            if(_dfsConnect(x, y, 1, 6, i, "011110") && _dfsConnect(x, y, 1, 4, j, "1110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;
            if(_dfsConnect(x, y, 1, 5, i, "11110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 5, j, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(j), "10")) return true;
            if(_dfsConnect(x, y, 1, 5, i, "11110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 4, j, "1110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;

            if(_dfsConnect(x, y, 1, 6, i, "011110") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "110")) return true;
            if(_dfsConnect(x, y, 1, 5, i, "11110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "110")) return true;

            if(_dfsConnect(x, y, 1, 5, i, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "10") && _dfsConnect(x, y, 1, 5, j, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(j), "10")) return true;
            if(_dfsConnect(x, y, 1, 5, i, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "10") && _dfsConnect(x, y, 1, 4, j, "1110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;
            if(_dfsConnect(x, y, 1, 4, i, "1110") && _dfsConnect(x, y, 1, 3, antiDirc(i), "010") && _dfsConnect(x, y, 1, 4, j, "1110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;

            if(_dfsConnect(x, y, 1, 5, i, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "10") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "110")) return true;
            if(_dfsConnect(x, y, 1, 4, i, "1110") && _dfsConnect(x, y, 1, 3, antiDirc(i), "010") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "110")) return true;

            if(_dfsConnect(x, y, 1, 4, i, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(i), "110") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "110")) return true;
            //四四禁手
            if(_dfsConnect(x, y, 1, 5, i, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 5, j, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(j), "00")) return true;

            if(_dfsConnect(x, y, 1, 5, i, "01110") && _dfsConnect(x, y, 1, 2, antiDirc(i), "00") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;

            if(_dfsConnect(x, y, 1, 4, i, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(i), "010") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;

            if(_dfsConnect(x, y, 1, 3, i, "010") && _dfsConnect(x, y, 1, 4, antiDirc(i), "0110") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 4, antiDirc(j), "0100")) return true;

            if(_dfsConnect(x, y, 1, 4, i, "0110") && _dfsConnect(x, y, 1, 4, antiDirc(i), "0100") && _dfsConnect(x, y, 1, 4, j, "0110") && _dfsConnect(x, y, 1, 4, antiDirc(j), "0100")) return true;

            if(_dfsConnect(x, y, 1, 5, i, "01100") && _dfsConnect(x, y, 1, 3, antiDirc(i), "010") && _dfsConnect(x, y, 1, 5, j, "01100") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;

            if(_dfsConnect(x, y, 1, 4, i, "0110") && _dfsConnect(x, y, 1, 3, antiDirc(i), "010") && _dfsConnect(x, y, 1, 5, j, "01100") && _dfsConnect(x, y, 1, 3, antiDirc(j), "010")) return true;
        }
    //长连禁手
    for (int  i = 0; i < 4; i ++)
    {
        if(_dfsConnect(x, y, 1, 5, i * 2 + 1, "11110") && _dfsConnect(x, y, 1, 4, antiDirc(i * 2 + 1), "110")) return true;
    }
    return false;
}
