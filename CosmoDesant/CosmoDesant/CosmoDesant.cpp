#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h> 
#include <conio.h>
#include <cstdlib>
#include "Enemy.h"
#include "GameManager.h"

using namespace std;

#define field ' '
#define fbrick 176
#define fstone 206

//Обозначение символов для танка 
#define duloV 179
#define duloH 205 
#define tankC 219
#define catter '#'

typedef char mapHW[GameConstants::height][GameConstants::width];
GameManager game;

void SetCurPos(int x, int y) {

    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

bool IsCross(RECT a, RECT b) {

    return (a.right >= b.left) && (a.left <= b.right)
        && (a.bottom >= b.top) && (a.top <= b.bottom);
}

struct Tmap {
    //Сама карта 
    mapHW map;
    void Clear() { memset(map, field, sizeof(map) - 1); }
    void SetEnd() { map[GameConstants::height - 1][GameConstants::width - 1] = '\0'; }
    void Show() { SetCurPos(0, 0); SetEnd(); cout << map[0]; }

};

enum Tdir { Rup = 0, Rleft, Rright };
POINT dirInc[] = { {3, 0},{-3, 0} };
POINT dirUp = { 0, -1 };

class Ship {

    int x, y;
    int sX, sY;

public:
    Tdir dir;
    Ship(int startX, int startY)
    {
        dir = Rup; sX = startX; sY = startY; SetToStart();
    }
    void Show(mapHW& map);
    void Move(char s, char a, char fire);
    void SetToStart() { x = sX; y = sY; }
    RECT GetRect() { RECT r = { x - 1, y - 1, x + 1, y + 1 }; return r; }
};



class Tpula {

    int x, y;
    int speed;
    Tdir dir;

public:
    bool use;
    Tpula() { use = 0; speed = 5; }

    void SetPula(int px, int py, Tdir pdir)
    {
        x = px;
        y = py;
        dir = pdir;
        use = 1;
    }

    void Move();
    void Show(mapHW& map) { if (!use) return; map[y][x] = '*'; }

};


void Ship::Show(mapHW& map) {

    for (int i = -1; i < 2; i++)
        map[y + 1][x + i] = catter;

    map[y][x] = tankC;
    POINT dt = dirUp;
    map[y + dt.y][x + dt.x] = duloV;
}


Ship ship(13,19);

Tmap scr;

RECT area = { 2,2, GameConstants::width - 3,GameConstants::height - 3 };

#define pulaCnt 100
Tpula pula[pulaCnt];

Tpula& GetFreePula() {

    for (int i = 0; i < pulaCnt; i++)
        if (!pula[i].use)
            return pula[i];

    return pula[0];
}

void Ship::Move( char d, char a, char fire) {

    char wasd[4] = {a,d};

    bool keyPressed = false; 
    POINT pt = { 0, 0 };

    for (int i = 0; i < 2; i++) {
        if (GetKeyState(wasd[i]) & 0x8000) {  // Если клавиша нажата
            dir = (Tdir)i;                    // Обновляем направление
            pt = dirInc[dir];                // Получаем вектор движения
            keyPressed = true;               // Запоминаем, что клавиша нажата
            break;                           // Выходим (если нужно только одно направление)
        }
    }

    // Двигаемся только если клавиша нажата (keyPressed == true)
    if (keyPressed) {
        Ship old = *this;  // Сохраняем старую позицию
        x += pt.x;         // Обновляем координаты
        y += pt.y;

        // Проверяем столкновения
        if (!IsCross(area, GetRect()) /*|| другие проверки столкновений*/) {
            *this = old;  // Откатываем, если столкновение
        }
    }

    if (GetKeyState(fire) < 0)
        GetFreePula().SetPula(x , y, Rup);
}

RECT areaPula = { 0,0, GameConstants::width - 1, GameConstants::height - 1 };


Enemy* CheckCrossAnyEnemy(RECT rct) {

    for (Enemy* enemy : game.GetEnemies()){
        if (!enemy->IsActive())
            continue;

        if (IsCross(rct, enemy->GetRect())) {

            return enemy;
       
        }

    }
        return nullptr;
}


void Tpula::Move() {

    if (!use) return;

    for (int i = 1; i < speed; i++) {

        x += dirUp.x;
        y += dirUp.y;
        RECT rct = { x,y,x,y };

        if (!IsCross(rct, areaPula))
            use = 0;

        Enemy* enemy = CheckCrossAnyEnemy(rct);

        if (enemy) {

            enemy->TakeDamage(1);
            game.AddScore();
        }

        if (!use)
            return;
    } 
}



int main() {
 
    char command[1000];
    sprintf(command, "mode con cols=%d lines=%d", GameConstants::width, GameConstants::height);
    system(command);

    do
    {
        ship.Move('A', 'D', VK_SPACE);
        for (int i = 0; i < pulaCnt; pula[i++].Move());

        scr.Clear();
        game.Update();
        game.Render(scr.map);
        for (int i = 0; i < pulaCnt; pula[i++].Show(scr.map));
        ship.Show(scr.map);
        scr.Show();

        Sleep(100);
    } 
    while (GetKeyState(VK_ESCAPE) >= 0);

    return 0;
}