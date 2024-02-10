#include "main.h"

// 将ShowMap刷新到GameView中
static int flashGameView();

/**
 * 打开一个格子
 * 返回值:
 * -1 打开失败
 * 0 正常打开
 * 1 打开后游戏失败
 * 6 打开后游戏胜利
*/
static int openGrid(int x, int y);

// 扫雷游戏
SaoleiGame * game;

// 改变图中一个点的数据
static int setMapData(GameMap * map, int x, int y, char data){
    if(x < 0 | x >= game->width | y < 0 | y >= game->height){
        printf("setMapData: (%d, %d) beyond Map \n", x, y);
        return -1;
    }
    *(map->p + game->width * y + x) = data;
    return 1;
}

// 获取图中一个点的数据
static int getMapData(GameMap * map, int x, int y, char * data){
    if(x < 0 | x >= game->width | y < 0 | y >= game->height){
        printf("getMapData: (%d, %d) beyond Map \n", x, y);
        return -1;
    }
    *data = *(map->p + game->width * y + x);
    return 1;
}

// 计算某个格子周围地雷的数量
static int countMine(GameMap * map, int x, int y){
    int count = 0;
    int i,j;
    for(j=y-1; j<y+2; j++){
        if(j < 0) continue;
        else if(j >= game->height) break;
        for(i=x-1; i<x+2; i++){
            if(i < 0) continue;
            else if(i >= game->width) break;

            char data;
            if(getMapData(map, i, j, &data) == -1) continue;
            if(data == 'm') count++;
        }
    }
    return count;
}

static GameMap * creatSourceMap(){
    GameMap * map = malloc(sizeof(GameMap));
    map->p = malloc(game->width * game->height);

    // 初始化Map
    int x, y;
    for(y=0;y<game->height;y++){
        for(x=0;x<game->width;x++){
            setMapData(map, x, y, '0');
        }
    }

    // 生成地雷的位置
    int m = 0;
    while(1){
        if(m == game->diffic) break;
        // 计算地雷生成的坐标
        int mx = randuint(0, game->width);
        int my = randuint(0, game->height);
        if(mx == game->startPoint[0] && my == game->startPoint[1]) continue;
        char data;
        getMapData(map, mx, my, &data);
        if(data != 'm'){
            setMapData(map, mx, my, 'm');
            m++;
        }
    }

    // 计算数字
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            // 判断格子是不是地雷
            char data;
            getMapData(map, x, y, &data);
            if(data == 'm') continue;

            int count = countMine(map, x, y);
            // printf("(%d, %d) count: %d \n",x,y,count);

            data = '0' + count;
            setMapData(map, x, y, data);
        }
    }

    return map;
}

static GameMap * creatShowMap(){
    GameMap * showMap = malloc(sizeof(GameMap));
    showMap->p = malloc(game->width*game->height);

    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            setMapData(showMap, x, y, 'h');
        }
    }

    return showMap;
}

// 初始化
static void initSaolei(SaoleiGame * game){
    printf("initSaolei: SaoLei Init  \n");
    if(game->sourceMap){
        free(game->sourceMap);
    }
    if(game->showMap){
        free(game->showMap);
    }

    // 创建一个游戏的两张基础图
	GameMap * sourceMap = creatSourceMap();
    GameMap * showMap = creatShowMap();
    
    game->sourceMap = sourceMap;
    game->showMap = showMap;

    game->state = 1;
    game->timer->cmd[0] = 3;
    flashGameView();
}

SaoleiGame * creatSaolei(){
	game = malloc(sizeof(SaoleiGame)); //创建一个游戏
	
    // 设置游戏规格
    game->width = 8;
    game->height = 10;
    game->diffic = 10;

	// 将两张基础图添加到游戏中
	game->sourceMap = NULL;
	game->showMap = NULL;

	// 建立一个游戏View
	View * gameView = creatView(1, 600, 600, 0, 0);

	// 游戏View添加到游戏中
	game->gameView = gameView;

    // 为Game添加一个计时器
    game->timer = creaTimer(10, 150, 40, 920, 10, 0xffcc66);

    game->startPoint[0] = -1;
    game->startPoint[1] = -1;
    initSaolei(game);
    game->state = 7;

    return game;
}

static int flashGameView(){
    View * view = game->gameView;
    GameMap * map = game->showMap;

    if(game->state == 0){
        return -1;
    }

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->canvas->width / game->width; // 宽度能取得的最大像素
    int hpixSize = view->canvas->height / game->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->canvas->width - game->width * pixSize) / 2;
    int ymargin = (view->canvas->height - game->height * pixSize) / 2;

    // 绘制边框
    int x, y;
    for(y=1; y<game->height; y++){
        drawRect(view->canvas, xmargin, ymargin + pixSize * y, game->width*pixSize, 4, 0x0066ccff);
    }
    for(x=1; x<game->width; x++){
        drawRect(view->canvas, xmargin + pixSize * x, ymargin, 4, game->height*pixSize, 0x0066ccff);
    }

    // 绘制GameMap
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            // 获取数据
            char data;
            getMapData(map, x, y, &data);
            // printf("(%d, %d) : %c \t", x, y, data);
            int sx = xmargin + x*pixSize+4; // 绘制的格子x轴起始坐标
            int sy = ymargin + y*pixSize+4; // 绘制的格子y轴起始坐标
            /**
             * 判断数据的类型
             * 0-9: 该网格是一个数字或空
             * m: 该网格是一个地雷
             * h: 该网格是一个隐藏内容的格子
             * s: 该网格是一个被选中的格子
             * f: 插旗的格子
             * g: 被选中的旗子
            */
            if(data == 'h'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, HIDE_COLOR);
            }
            else if(data >= '0' && data <= '8'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, -1);
                if(data == '0') continue;
                int color;
                // 不同数字显示不同颜色
                switch (data)
                {
                case '1':
                    color = 0x00B5E61D;
                    break;

                case '2':
                    color = 0x0022b14c;
                    break;

                case '3':
                    color = 0x0000A2E8;
                    break;
                
                case '4':
                    color = 0x003F48CC;
                    break;

                case '5':
                    color = 0x00A349A4;
                    break;
                
                default:
                    color = 0x00222222;
                    break;
                }
                drawChar(view->canvas, sx, sy, data, pixSize-4, pixSize-4, color);
            }
            else if(data == 'm'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 's'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, 0x00ffcc66);
            }
            else if(data == 'f'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, HIDE_COLOR);
                drawChar(view->canvas, sx, sy, 'F', pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 'g'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, 0x00ffcc66);
                drawChar(view->canvas, sx, sy, 'F', pixSize-4, pixSize-4, 0x00ff0000);
            }
            else{
                continue;
            }
        }
    }
    return 0;
}

// 判断游戏胜利的条件
static int gameIsWin(){

    GameMap * showMap = game->showMap;

    int x, y;
    int hideNum = 0;
    for(y = 0; y<game->height; y++){
        for(x = 0; x<game->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 'h' || data == 's' || data == 'f' || data == 'g') hideNum++;
        }
    }
    printf("gameIsWin: hideNum %d diffic %d \n", hideNum, game->diffic);
    if(hideNum == game->diffic){
        return 1;
    }
    else{
        return 0;
    }
}


static int openGrid(int x, int y){
    // 判断是否是第一次点开，第一次点开重新布局并开始计时器
    if(game->state == 7){
        game->startPoint[0] = x;
        game->startPoint[1] = y;
        free(game->sourceMap);
        game->sourceMap = creatSourceMap();
        game->state = 1;
        game->timer->cmd[0] = 1;
    }

    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;
    // printf("openGrid: (%d, %d) \n", x, y);
    char data;
    // 判断要打开的格子的状态
    getMapData(showMap, x, y, &data);
    if(data != 'h' && data != 's'){
        return -1;
    }

    // 获取格子中的实际内容
    getMapData(sourceMap, x, y, &data);
    printf("openGrid: Opened(%d, %d): %c \n", x, y, data);
    if(data >= '0' && data <= '8'){ // 点到一个数字，揭开这一个格子
        setMapData(showMap, x, y, data);
        if(data == '0'){
            int i, j;
            for(j=y-1; j<y+2; j++){
                if(j < 0) continue;
                if(j >= game->height) break;
                for(i=x-1; i<x+2; i++){
                    if(i < 0) continue;
                    if(i >= game->width) break;
                    openGrid(i, j);
                }
            }
        }
        if(gameIsWin() == 1){
            return 6;
        }
        return 0;
    }

    else if(data == 'm'){ // 点到地雷游戏结束
        return 1;
    }
}

int getRelatPoints(View * view, Point * touchP, int * relatPoint){
    relatPoint[0] = touchP->x - view->marginsX;
    relatPoint[1] = touchP->y - view->marginsY;
    // 判断坐标是否在view内部
    if(relatPoint[0] < 0 || relatPoint[0] > view->canvas->width || relatPoint[1] < 0 || relatPoint[1] > view->canvas->height){
        return -1;
    }

    touchP->x = touchP->y = -1;
    return 1;
}

// 获取触摸到的网格并绘制光标
static int getTouchGrid(int * res){
    View * view = game->gameView;
    GameMap * map = game->showMap;

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->canvas->width / game->width; // 宽度能取得的最大像素
    int hpixSize = view->canvas->height / game->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->canvas->width - game->width * pixSize) / 2;
    int ymargin = (view->canvas->height - game->height * pixSize) / 2;

    // 相对坐标
    int relatP[] = {view->event.value[0], view->event.value[1]};

    if(relatP[0] < xmargin || relatP[1] < ymargin){
        return -1;
    }

    *res = (relatP[0] - xmargin) / pixSize;
    *(res + 1) = (relatP[1] - ymargin) / pixSize;

    // printf("TouchGrid: (%d, %d) \n", *res, *(res + 1));
    return 1;
}

int selectGrid(){
    GameMap * showMap = game->showMap;

    // 获取触摸的网格
    int grid[2];
    getTouchGrid(grid);

    char data;
    getMapData(showMap, grid[0], grid[1], &data);
    int i, j;
    
    if(data == 'h' || data == 'f'){  // 判断是否可以被选中
        // 清除上一次的选中
        for(j=0; j<game->height; j++){
            for(i=0; i<game->width; i++){
                getMapData(showMap, i, j, &data);
                if(data == 's'){
                    setMapData(showMap, i, j, 'h');
                }
                else if(data == 'g'){
                    setMapData(showMap, i, j, 'f');
                }
            }
        }
        
        // 选中指定的格子
        printf("selectGrid: Select(%d, %d) \n", grid[0], grid[1]);
        getMapData(showMap, grid[0], grid[1], &data);
        if(data == 'h'){
            setMapData(showMap, grid[0], grid[1], 's');
        }
        else if(data == 'f'){
            setMapData(showMap, grid[0], grid[1], 'g');
        }
    }

    flashGameView();
    return 1;
}

static int gameOver(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(sourceMap, x, y, &data);
            if(data == 'm'){
                setMapData(showMap, x, y, 'm');
            }
        }
    }
    flashGameView();
    game->state = 0;
    game->timer->cmd[0] = 2;

    return 1;
}

static int gameWin(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(sourceMap, x, y, &data);
            if(data == 'm'){
                setMapData(showMap, x, y, 'f');
            }
        }
    }
    flashGameView();
    game->state = 0;
    game->timer->cmd[0] = 2;

    return 1;
}

int openSelectedGrid(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    // 找到被选中的格子
    int x, y;
    int openRes;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 's'){
                openRes = openGrid(x, y);
            }
        }
    }

    if(openRes == 0){
        flashGameView();
    }
    else if (openRes == 1){
        gameOver();
    }
    else if(openRes == 6){
        gameWin();
    }
    return openRes;
}

// 将一个格子插旗
static int flagGrid(int x, int y){
    GameMap * showMap = game->showMap;

    char data;
    getMapData(showMap, x, y, &data);
    if(data == 's'){
        setMapData(showMap, x, y, 'f');
        return 1;
    }
    else if(data == 'g'){
        setMapData(showMap, x, y, 'h');
        return 0;
    }
    else{
        return -1;
    }
}

int flagSelectedGrid(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    // 找到被选中的格子
    int x, y;
    int flagRes;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 's' || data == 'g'){
                flagRes = flagGrid(x, y);
            }
        }
    }
    flashGameView();
}

int restartSaolei(){
    initSaolei(game);
    game->state = 7;
}