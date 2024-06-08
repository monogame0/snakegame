#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <graphics.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;
using namespace ege;
//#define HELP 
#define LEFT -1 // 左方向
#define RIGHT 1 // 右方向
#define UP 2    // 上方向
#define DOWN -2 // 下方向

#define BGR(r, g, b)      RGB(b, g, r) // 颜色转换函数

#define GAME_WINDOW_Width  640 // 游戏窗口宽度
#define GAME_WINDOW_Height 480 // 游戏窗口高度
#define GAME_WINDOW_Title  "Snake Game" // 游戏窗口标题

#define SNAKE_MAX_LEN      500          // 贪吃蛇的最大长度
#define SNAKE_LEN          20                // 贪吃蛇的初始长度
#define SNAKE_SPEED        1             // 贪吃蛇的速度一像素为单位
#define SNAKE_WIDTH        20             // 贪吃蛇的宽度
#define SNAKE_COLOR        BGR(255, 0, 0) // 贪吃蛇的颜色
#define SNAKE_DIR          DOWN             // 贪吃蛇的方向
#define SNAKE_HEAD_X       100           // 贪吃蛇的头部x坐标
#define SNAKE_HEAD_Y       100           // 贪吃蛇的头部y坐标
#define SNAKE_MOVE_LEFT    'A'   // 按键A
#define SNAKE_MOVE_RIGHT   'D'   // 按键D
#define SNAKE_MOVE_UP      'W'   // 按键W
#define SNAKE_MOVE_DOWN    'S'   // 按键S
#define SNAKE_HP           100.0  // 贪吃蛇的生命值

#define FOOD_COLOR         BGR(0, 255, 0) // 食物的颜色
#define FOOD_WIDTH         20             // 食物的宽度

typedef struct Snake_Node
{ // 贪吃蛇的节点结构体
    float x;
    float y;
};
typedef struct Food{  //食物的结构体
    float x;
    float y;
    bool quite;
};

void swap_node(Snake_Node *a, Snake_Node *b){
    Snake_Node temp = *a;
    *a = *b;
    *b = temp;
}

bool GetKey(int Key){
    if(GetKeyState(Key) & 0x8000)
        return true;
    else
        return false;
}
void cleanwindow(void){
    cleardevice();
}
int random(int min, int max){  //用于产生食物的随机数
    return rand() % max + min;
}
void ExitGame(void){
    if(MessageBoxA(NULL, "Are you sure to exit?", "Exit", MB_OKCANCEL) == IDOK){
        exit(0);
    }
    else{
        return;
    }
}
float distance_len(float x1, float y1, float x2, float y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}  //计算两个点的距离
void draw_food(float x, float y){
     for(int tx = x; tx < x + FOOD_WIDTH; tx++){
            for(int ty = y; ty < y + FOOD_WIDTH; ty++){
                putpixel(tx, ty, FOOD_COLOR);
            }
        }
}  //画食物

// 以下是一些数学函数
double pg_math_angle_rad(double angle){    //角度转弧度
    return angle * M_PI / 180.0;
}
double pg_math_rad_angle(double rad){    //弧度转角度
    return rad * 180.0 / M_PI;
}
double pg_math_cal_point_angle(int x1, int y1, int x2, int y2){ 
    double angle = atan2(y2 - y1, x2 - x1) * 180.0 / M_PI;
    if(angle < 0){
        angle += 360;
    }
    return angle;
} //计算两点之间的角度
double pg_math_PointLen(int x1, int y1, int x2, int y2){  //计算两点之间的距离
    double len = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return len;
}
double pg_math_In_Angle_sin(double angle){
    return sin(pg_math_angle_rad(angle));
}   //正弦值
double pg_math_In_Angle_cos(double angle){
    return cos(pg_math_angle_rad(angle));
}   //余弦值
void DrawLine(int x1, int y1, int x2, int y2, int color){  //Bresenham算法
    double angle = pg_math_cal_point_angle(x1, y1, x2, y2);
    double LengthEnd = pg_math_PointLen(x1, y1, x2, y2);
    double Length = 0.0;
    while (Length < LengthEnd){
        int x = x1 + Length * pg_math_In_Angle_cos(angle);
        int y = y1 + Length * pg_math_In_Angle_sin(angle);
        ege::putpixel(x, y, color);
        Length += 0.1;
    }
}
//祖传代码上一个项目剩下的，没时间整理了，先放着吧
class SNAKE
{
public:
    Snake_Node sNode[SNAKE_MAX_LEN];
    Snake_Node *head; // 贪吃蛇的头部指针
    int len;// 贪吃蛇的长度
    int dir;// 贪吃蛇的方向
    int speed;// 贪吃蛇的速度
    int width;// 贪吃蛇的宽度
    int color;// 贪吃蛇的颜色
    int score;// 贪吃蛇的得分
    float HP;// 贪吃蛇的生命值
    SNAKE()
    {
        len = SNAKE_LEN;
        dir = SNAKE_DIR;
        speed = SNAKE_SPEED;
        width = SNAKE_WIDTH;
        color = SNAKE_COLOR;
        score = 0;
        head = &sNode[0];
        head->x = SNAKE_HEAD_X;
        head->y = SNAKE_HEAD_Y;
        HP = SNAKE_HP;
        switch (dir)
        {
        case RIGHT:
        case LEFT:
            for (int i = 0; i < len; i++)
            {
                sNode[i].y = head->y;
                sNode[i].x = SNAKE_HEAD_X + i * SNAKE_WIDTH;
            }
            break;
        case UP:
        case DOWN:
            for (int i = 0; i < len; i++)
            {
                sNode[i].x = head->x;
                sNode[i].y = SNAKE_HEAD_Y - i * SNAKE_WIDTH / 2.0f;
            }
            break;
        }
    }
    void DrawNode(int x, int y){  //画蛇的每一个节点
        for(int tx = x; tx < x + width; tx++){
            for(int ty = y; ty < y + width; ty++){
                putpixel(tx, ty, color);
            }
        }
    }
    void Draw(){
        for (int i = 0; i < len; i++)
        {
            DrawNode(sNode[i].x, sNode[i].y);
        }
    }  //画蛇
    void Move(){
        if(GetKey(SNAKE_MOVE_LEFT) && dir != RIGHT){
            dir = LEFT;
        }
        if(GetKey(SNAKE_MOVE_RIGHT) && dir != LEFT){
            dir = RIGHT;
        }
        if(GetKey(SNAKE_MOVE_UP) && dir != DOWN){
            dir = UP;
        }
        if(GetKey(SNAKE_MOVE_DOWN) && dir != UP){
            dir = DOWN;
        }
        Snake_Node Temp;
        Temp = sNode[0];
        switch (dir){
            case LEFT:
                head->x -= speed * SNAKE_WIDTH / 2.0f;
                break;
            case RIGHT:
                head->x += speed * SNAKE_WIDTH / 2.0f;
                break;
            case UP:
                head->y -= speed * SNAKE_WIDTH / 2.0f;
                break;
            case DOWN:
                head->y += speed * SNAKE_WIDTH / 2.0f;
                break;
        }

        // 移动蛇的头部
        int i = 1;
        while(i < len){
            swap_node(&sNode[i], &Temp);
            i++;
        }
    }
     void GameOver(){
        outtextxy(GAME_WINDOW_Width / 2 - 100, 
        GAME_WINDOW_Height / 2 - 50, "Game Over!"); // 输出文字

        outtextxy(GAME_WINDOW_Width / 2 - 100, 
        GAME_WINDOW_Height / 2 + 50, "Press any key to exit."); // 输出文字

        char score_str[20];
        sprintf(score_str, "%d", score);
        string str = "Your score: ";//输出得分
        str += score_str;
        outtextxy(GAME_WINDOW_Width / 2 - 100, GAME_WINDOW_Height / 2, str.c_str()); // 输出文字

        const char* AgainGame;  //再来一次游戏
        AgainGame = "Press Enter key to play again."; // 输出文字
        outtextxy(GAME_WINDOW_Width / 2 - 100, 
        GAME_WINDOW_Height / 2 + 100, AgainGame); // 输出文字

        while(true){
            if(GetKey(VK_ESCAPE)){
                exit(0);
            }
            if(GetKey(VK_RETURN)){
                head->x = SNAKE_HEAD_X;
                head->y = SNAKE_HEAD_Y;
                len = SNAKE_LEN;
                dir = SNAKE_DIR;
                speed = SNAKE_SPEED;
                score = 0;
                break;
            }
        }
     }
    void Collision(){  //碰撞检测
        if(head->x < 0 || head->x > GAME_WINDOW_Width || 
            head->y < 0 || head->y > GAME_WINDOW_Height){
            HP -= 10.0f;  // 生命值减少
        }
    }
    void PrintScore(){
        char score_str[20];
        sprintf(score_str, "%d", score);
        //拼接字符串
        string str = "Score: ";
        str += score_str;
        outtextxy(GAME_WINDOW_Width - 100, 30, str.c_str()); // 输出文字
    }  //输出分数
    void FastMove(void){
        speed = 2;
    }
    void DrawHP(){
        const int HP_len = 70;
        const int HP_height = 10;
        //画血条边框begin
        line(GAME_WINDOW_Width - 100 + 2, 50, 
        GAME_WINDOW_Width - 100 + HP_len, 50);  

        line(GAME_WINDOW_Width - 100 + 2, 50 + HP_height, GAME_WINDOW_Width - 100 + HP_len, 
        50 + HP_height);

        line(GAME_WINDOW_Width - 100 + 2, 50, 
        GAME_WINDOW_Width - 100 + 2, 50 + HP_height);

        line(GAME_WINDOW_Width - 100 + HP_len, 50, 
        GAME_WINDOW_Width - 100 + HP_len, 50 + HP_height);
        //画血条边框end

        float HP_rate = HP / SNAKE_HP;  // 生命值比率
        for(int y = 50; y < 50 + HP_height; y++){
            DrawLine(GAME_WINDOW_Width - 100, y, GAME_WINDOW_Width - 100 + HP_rate * HP_len,y,BGR(0, 0, 255));  //画血条
        }
        
    }
};
class GAME{
    private:
        SNAKE snake;
        Food food;
    public:
        void game(void){
            #ifdef HELP
            cout << "Welcome to Snake Game!" << endl;
            cout << "Press 'A' to move left, 'D' to move right, 'W' to move up, 'S' to move down, 'Space' to speed up." << endl;
            cout << "Press 'Esc' to exit." << endl;
            Sleep(2000); // 等待2秒
            #endif

            cout << "Game Start!" << endl;
            initgraph(GAME_WINDOW_Width, GAME_WINDOW_Height);// 初始化图形界面
            setcaption(GAME_WINDOW_Title);// 设置窗口标题
            bool food_quite = true;  // 食物是否被吃掉(true表示未被吃掉)
            food.x = random(0, GAME_WINDOW_Width - FOOD_WIDTH);  // 随机生成食物的坐标
            food.y = random(0, GAME_WINDOW_Height - FOOD_WIDTH);
            //food.quite = food_quite;
            while(true){
                if(GetKey(VK_ESCAPE)){
                     ExitGame();
                }
                if(!food_quite){
                    food_quite = true;  // 食物已被吃掉需要重新生成
                }
                if(food_quite)
                {
                    draw_food(food.x, food.y);  // 画食物
                }
                if(distance_len(snake.head->x, snake.head->y, food.x, food.y) < SNAKE_WIDTH){
                   snake.score++;  // 吃到食物加分
                   food_quite = false;  // 食物被吃掉
                   snake.len++;  // 蛇长加1
                   food.x = random(0, GAME_WINDOW_Width - FOOD_WIDTH);  // 重新生成食物
                   food.y = random(0, GAME_WINDOW_Height - FOOD_WIDTH);
                } // 食物与蛇头的距离小于蛇的宽度)
                snake.Move();
                snake.Draw();
                snake.Collision();  //碰撞检测
                snake.PrintScore();//在右上角显示分数
                snake.DrawHP();  //画生命值
                if(snake.HP <= 0){
                    snake.GameOver();
                }
                if(GetKey(' ')){
                    snake.FastMove();
                }
                else{
                    snake.speed = 1;
                }//按空格键加快游戏速度
                delay(100); // 延时100毫秒
                cleanwindow(); // 清除窗口4a5eb4deb426e3344b1ce400fedf8e1c77abe6f7
            }
        }
};
int main()
{
    GAME game;
    game.game();
    
    return 0;
}