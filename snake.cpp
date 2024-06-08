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
#define LEFT -1 // ����
#define RIGHT 1 // �ҷ���
#define UP 2    // �Ϸ���
#define DOWN -2 // �·���

#define BGR(r, g, b)      RGB(b, g, r) // ��ɫת������

#define GAME_WINDOW_Width  640 // ��Ϸ���ڿ��
#define GAME_WINDOW_Height 480 // ��Ϸ���ڸ߶�
#define GAME_WINDOW_Title  "Snake Game" // ��Ϸ���ڱ���

#define SNAKE_MAX_LEN      500          // ̰���ߵ���󳤶�
#define SNAKE_LEN          20                // ̰���ߵĳ�ʼ����
#define SNAKE_SPEED        1             // ̰���ߵ��ٶ�һ����Ϊ��λ
#define SNAKE_WIDTH        20             // ̰���ߵĿ��
#define SNAKE_COLOR        BGR(255, 0, 0) // ̰���ߵ���ɫ
#define SNAKE_DIR          DOWN             // ̰���ߵķ���
#define SNAKE_HEAD_X       100           // ̰���ߵ�ͷ��x����
#define SNAKE_HEAD_Y       100           // ̰���ߵ�ͷ��y����
#define SNAKE_MOVE_LEFT    'A'   // ����A
#define SNAKE_MOVE_RIGHT   'D'   // ����D
#define SNAKE_MOVE_UP      'W'   // ����W
#define SNAKE_MOVE_DOWN    'S'   // ����S
#define SNAKE_HP           100.0  // ̰���ߵ�����ֵ

#define FOOD_COLOR         BGR(0, 255, 0) // ʳ�����ɫ
#define FOOD_WIDTH         20             // ʳ��Ŀ��

typedef struct Snake_Node
{ // ̰���ߵĽڵ�ṹ��
    float x;
    float y;
};
typedef struct Food{  //ʳ��Ľṹ��
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
int random(int min, int max){  //���ڲ���ʳ��������
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
}  //����������ľ���
void draw_food(float x, float y){
     for(int tx = x; tx < x + FOOD_WIDTH; tx++){
            for(int ty = y; ty < y + FOOD_WIDTH; ty++){
                putpixel(tx, ty, FOOD_COLOR);
            }
        }
}  //��ʳ��

// ������һЩ��ѧ����
double pg_math_angle_rad(double angle){    //�Ƕ�ת����
    return angle * M_PI / 180.0;
}
double pg_math_rad_angle(double rad){    //����ת�Ƕ�
    return rad * 180.0 / M_PI;
}
double pg_math_cal_point_angle(int x1, int y1, int x2, int y2){ 
    double angle = atan2(y2 - y1, x2 - x1) * 180.0 / M_PI;
    if(angle < 0){
        angle += 360;
    }
    return angle;
} //��������֮��ĽǶ�
double pg_math_PointLen(int x1, int y1, int x2, int y2){  //��������֮��ľ���
    double len = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return len;
}
double pg_math_In_Angle_sin(double angle){
    return sin(pg_math_angle_rad(angle));
}   //����ֵ
double pg_math_In_Angle_cos(double angle){
    return cos(pg_math_angle_rad(angle));
}   //����ֵ
void DrawLine(int x1, int y1, int x2, int y2, int color){  //Bresenham�㷨
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
//�洫������һ����Ŀʣ�µģ�ûʱ�������ˣ��ȷ��Ű�
class SNAKE
{
public:
    Snake_Node sNode[SNAKE_MAX_LEN];
    Snake_Node *head; // ̰���ߵ�ͷ��ָ��
    int len;// ̰���ߵĳ���
    int dir;// ̰���ߵķ���
    int speed;// ̰���ߵ��ٶ�
    int width;// ̰���ߵĿ��
    int color;// ̰���ߵ���ɫ
    int score;// ̰���ߵĵ÷�
    float HP;// ̰���ߵ�����ֵ
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
    void DrawNode(int x, int y){  //���ߵ�ÿһ���ڵ�
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
    }  //����
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

        // �ƶ��ߵ�ͷ��
        int i = 1;
        while(i < len){
            swap_node(&sNode[i], &Temp);
            i++;
        }
    }
     void GameOver(){
        outtextxy(GAME_WINDOW_Width / 2 - 100, 
        GAME_WINDOW_Height / 2 - 50, "Game Over!"); // �������

        outtextxy(GAME_WINDOW_Width / 2 - 100, 
        GAME_WINDOW_Height / 2 + 50, "Press any key to exit."); // �������

        char score_str[20];
        sprintf(score_str, "%d", score);
        string str = "Your score: ";//����÷�
        str += score_str;
        outtextxy(GAME_WINDOW_Width / 2 - 100, GAME_WINDOW_Height / 2, str.c_str()); // �������

        const char* AgainGame;  //����һ����Ϸ
        AgainGame = "Press Enter key to play again."; // �������
        outtextxy(GAME_WINDOW_Width / 2 - 100, 
        GAME_WINDOW_Height / 2 + 100, AgainGame); // �������

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
    void Collision(){  //��ײ���
        if(head->x < 0 || head->x > GAME_WINDOW_Width || 
            head->y < 0 || head->y > GAME_WINDOW_Height){
            HP -= 10.0f;  // ����ֵ����
        }
    }
    void PrintScore(){
        char score_str[20];
        sprintf(score_str, "%d", score);
        //ƴ���ַ���
        string str = "Score: ";
        str += score_str;
        outtextxy(GAME_WINDOW_Width - 100, 30, str.c_str()); // �������
    }  //�������
    void FastMove(void){
        speed = 2;
    }
    void DrawHP(){
        const int HP_len = 70;
        const int HP_height = 10;
        //��Ѫ���߿�begin
        line(GAME_WINDOW_Width - 100 + 2, 50, 
        GAME_WINDOW_Width - 100 + HP_len, 50);  

        line(GAME_WINDOW_Width - 100 + 2, 50 + HP_height, GAME_WINDOW_Width - 100 + HP_len, 
        50 + HP_height);

        line(GAME_WINDOW_Width - 100 + 2, 50, 
        GAME_WINDOW_Width - 100 + 2, 50 + HP_height);

        line(GAME_WINDOW_Width - 100 + HP_len, 50, 
        GAME_WINDOW_Width - 100 + HP_len, 50 + HP_height);
        //��Ѫ���߿�end

        float HP_rate = HP / SNAKE_HP;  // ����ֵ����
        for(int y = 50; y < 50 + HP_height; y++){
            DrawLine(GAME_WINDOW_Width - 100, y, GAME_WINDOW_Width - 100 + HP_rate * HP_len,y,BGR(0, 0, 255));  //��Ѫ��
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
            Sleep(2000); // �ȴ�2��
            #endif

            cout << "Game Start!" << endl;
            initgraph(GAME_WINDOW_Width, GAME_WINDOW_Height);// ��ʼ��ͼ�ν���
            setcaption(GAME_WINDOW_Title);// ���ô��ڱ���
            bool food_quite = true;  // ʳ���Ƿ񱻳Ե�(true��ʾδ���Ե�)
            food.x = random(0, GAME_WINDOW_Width - FOOD_WIDTH);  // �������ʳ�������
            food.y = random(0, GAME_WINDOW_Height - FOOD_WIDTH);
            //food.quite = food_quite;
            while(true){
                if(GetKey(VK_ESCAPE)){
                     ExitGame();
                }
                if(!food_quite){
                    food_quite = true;  // ʳ���ѱ��Ե���Ҫ��������
                }
                if(food_quite)
                {
                    draw_food(food.x, food.y);  // ��ʳ��
                }
                if(distance_len(snake.head->x, snake.head->y, food.x, food.y) < SNAKE_WIDTH){
                   snake.score++;  // �Ե�ʳ��ӷ�
                   food_quite = false;  // ʳ�ﱻ�Ե�
                   snake.len++;  // �߳���1
                   food.x = random(0, GAME_WINDOW_Width - FOOD_WIDTH);  // ��������ʳ��
                   food.y = random(0, GAME_WINDOW_Height - FOOD_WIDTH);
                } // ʳ������ͷ�ľ���С���ߵĿ��)
                snake.Move();
                snake.Draw();
                snake.Collision();  //��ײ���
                snake.PrintScore();//�����Ͻ���ʾ����
                snake.DrawHP();  //������ֵ
                if(snake.HP <= 0){
                    snake.GameOver();
                }
                if(GetKey(' ')){
                    snake.FastMove();
                }
                else{
                    snake.speed = 1;
                }//���ո���ӿ���Ϸ�ٶ�
                delay(100); // ��ʱ100����
                cleanwindow(); // �������4a5eb4deb426e3344b1ce400fedf8e1c77abe6f7
            }
        }
};
int main()
{
    GAME game;
    game.game();
    
    return 0;
}