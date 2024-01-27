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

// 创建一张游戏图
GameMap creatGameMap(int width, int height, int n);

// 创建一个隐藏的游戏图
GameMap creatMaskMap(GameMap sourceMap);

// 在屏幕上绘制一张游戏图
int flashGameScreen(Screen * screen, GameMap * map);

// 打开一个网格
int openGrid(GameMap sourceMap, GameMap * showMap, int x, int y);

// // 初始化游戏屏幕中的光标
// struct touchPoint * initCursor(Screen screen, struct touchPoint * point);

// 根据坐标获取网格
int getTouchGrid(Screen * view, GameMap * map, Point * point, int * res);

// 选择一个网格
int selectGrid(GameMap *, int, int, Screen *);

#endif