#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <graphics.h>
using namespace std;
using namespace ege;

#define LEFT -1 // ����
#define RIGHT 1 // �ҷ���
#define UP 2    // �Ϸ���
#define DOWN -2 // �·���

#define BGR(r, g, b)      RGB(b, g, r) // ��ɫת������

#define GAME_WINDOW_Width  640 // ��Ϸ���ڿ��
#define GAME_WINDOW_Height 480 // ��Ϸ���ڸ߶�
#define GAME_WINDOW_Title  "Snake Game" // ��Ϸ���ڱ���

#define SNAKE_MAX_LEN      100          // ̰���ߵ���󳤶�
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

typedef struct Snake_Node
{ // ̰���ߵĽڵ�ṹ��
    float x;
    float y;
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
void ExitGame(void){
    if(MessageBoxA(NULL, "Are you sure to exit?", "Exit", MB_OKCANCEL) == IDOK){
        exit(0);
    }
    else{
        return;
    }
}
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
        outtextxy(GAME_WINDOW_Width / 2 - 100, GAME_WINDOW_Height / 2 - 50, "Game Over!"); // �������
        outtextxy(GAME_WINDOW_Width / 2 - 100, GAME_WINDOW_Height / 2 + 50, "Press any key to exit."); // �������
        getch(); // �ȴ��û�����
        exit(0); // �˳�����
    }
    void Collision(){  //��ײ���
        if(head->x < 0 || head->x > GAME_WINDOW_Width || head->y < 0 || head->y > GAME_WINDOW_Height){
            GameOver();
        }
    }
};
class GAME{
    private:
        SNAKE snake;
    public:
        void game(void){
            initgraph(GAME_WINDOW_Width, GAME_WINDOW_Height);// ��ʼ��ͼ�ν���
            setcaption(GAME_WINDOW_Title);// ���ô��ڱ���

            while(true){
                if(GetKey(VK_ESCAPE)){
                     ExitGame();
                }
                snake.Move();
                snake.Draw();
                //snake.Collision();
                delay(100); // ��ʱ100����
                cleanwindow(); // �������
                
            }
        }
};
int main()
{
    GAME game;
    game.game();
   return 0;
}