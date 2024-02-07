#ifndef __SAOLEI_H__
#define __SAOLEI_H__

#include "main.h"

typedef struct{
    int width;  // 宽度
    int height; // 高度
    int sx;
    int sy;
    int diffic; // 地雷的数量
    char * p;   // 存储地图的数组
}GameMap;

typedef struct{
    View * gameView;
    GameMap * sourceMap;
    GameMap * showMap;
    Timer * timer; // 游戏中嵌入一个计时器
    char state;
}SaoleiGame;

// 初始化一个扫雷游戏
SaoleiGame * creatSaolei();

// 游戏重新开始
int restartSaolei();

/**
 * 选择一个格子: 光标移动到选中格子
*/

int selectGrid();

/**
 * 打开被选中的格子
*/
int openSelectedGrid();

/**
 * 被选中的格子插旗
*/
int flagSelectedGrid();

#endif