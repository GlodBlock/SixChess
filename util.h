#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <qtCore>

//游戏模式
enum GameModel{
    PVE,
    PVP,
    EVE     //测试用
};

//游戏状态
enum GameStatus{
    RUNNING,
    END
};

//下棋方
enum Player{
    BLACK,
    WHITE,
    FAKE
};

//难度
enum Tier{
    PSAT,
    PRESENT,
    FUTURE
};

//方向
enum Dirc{
    LEFT_UP,
    UP,
    RIGHT_UP,
    RIGHT,
    RIGHT_DOWN,
    DOWN,
    LEFT_DOWN,
    LEFT
};

//棋盘大小
const int BoardSize = 19;

const int PORT = 4567;

class Util
{
public:
    Util();

    int ChessStatus[BoardSize+1][BoardSize+1];  //棋盘状态
    int BotRefer[BoardSize+1][BoardSize+1];      //bot参考权重

    Player player;          //下棋人状态
    GameModel gameModel;    //游戏模式
    GameStatus gameStatus;  //游戏状态

    void initGame(GameModel gm);    //初始化
    void calWeight(QPoint pos);     //bot计算权重
    void update(QPoint pos);        //落子后更新棋盘
    void turnHuman(QPoint pos);     //人类下棋
    void turnRobot();     //bot下棋

    Player isWin();       //判断胜利
    bool _dfsConnect(int rx, int ry, int col, int dep, int type, QString des);  //dfs搜索
    int antiDirc(int x);            //获取反向

    bool warning(QPoint pos);       //检测五子
    bool balanceBreaker(QPoint pos);//检测禁手
    bool isFull();                  //平局
};

#endif // UTIL_H
