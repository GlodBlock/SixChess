#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <qtCore>

//游戏模式
enum GameModel{
    PVE,
    PVP
};

//游戏状态
enum GameStatus{
    RUNNING,
    END
};

//下棋方
enum Player{
    BLACK,
    WHITE
};

//棋盘大小
const int BoardSize = 15;

class Util
{
public:
    Util();

    std::vector<std::vector<int>> ChessStatus;  //棋盘状态
    std::vector<std::vector<int>> BotRefer;      //bot参考权重

    Player player;          //下棋人状态
    GameModel gameModel;    //游戏模式
    GameStatus gameStatus;  //游戏状态

    void initGame(GameModel gm);    //初始化
    void calWeight();               //bot计算权重
    void update(QPoint pos);        //落子后更新棋盘
    void turnHuman(QPoint pos);     //人类下棋
    void turnRobot(QPoint pos);     //bot下棋
    Player isWin(QPoint pos);       //判断胜利

    bool isFull();                  //平局
};

#endif // UTIL_H