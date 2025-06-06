﻿#include <iostream>
#include <cstring>
#include <Windows.h>
#include <ctime> 

using namespace std;

#define width 80 
#define height 25

#define field ' '
#define fbrick 176
#define fstone 206

#define duloV 179
#define duloH 205 
#define tankC 219
#define catter '#'

typedef char mapHW[height][width];


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

    mapHW map;
    void Clear() { memset(map, field, sizeof(map) - 1); }
    void SetEnd() { map[height - 1][width - 1] = '\0'; }
    void Show() { SetCurPos(0, 0); SetEnd(); cout << map[0]; }

};

enum Tdir { Rup = 0, Rdowm, Rleft, Rright };
POINT dirInc[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

class Ttank {
    
    int x, y;
    int sX, sY;

public :
    Tdir dir;
    Ttank(int startX, int startY)
    {
        dir = Rup; sX = startX; sY = startY; SetToStart();
    }
    void Show(mapHW& map);
    void Move(char w, char s, char a, char d, char fire);
    void SetToStart() { x = sX; y = sY; }
    bool IsHoriz() { return (dir == Rright || dir == Rleft); }
    RECT GetRect() { RECT r = { x - 1, y - 1, x + 1, y + 1 }; return r; }
};

enum Tmatter {ttStone, ttBrick};

class Tbrick {
  
    RECT rct;

public:
    bool use; 
    Tmatter tp;
    Tbrick() { use = 0l, tp = ttBrick; }
    void Show(mapHW& map);
    void SetPos(int px, int py) { RECT r = { px - 1,py - 1,px + 1,py + 1 }; rct = r; use = 1; }
    RECT GetRect() { return rct; }

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


Tmap scr;
#define tankCnt 2
Ttank tank[tankCnt] = { Ttank(1,11), Ttank(78,11)};

#define brickCnt 300
Tbrick brick[brickCnt];

#define pulaCnt 100 
Tpula pula[pulaCnt];


Tpula& GetFreePula() {

    for (int i = 0; i < pulaCnt; i++)
        if (!pula[i].use)
            return pula[i];

    return pula[0];
}


Ttank* CheckCrossAnyTank(RECT rct, Ttank* eccept) {

    for (int i = 0; i < tankCnt; i++)
        if ((eccept != tank + i) &&
            (IsCross(rct, tank[i].GetRect())))
                return tank + i;

    return 0;
}

Tbrick* CheckCrossAnyBrick(RECT rct) {

    for (int i = 0; i < brickCnt; i++)
        if (brick[i].use &&
            IsCross(rct, brick[i].GetRect()))
            return brick + i;

    return 0;
}


void Ttank::Show(mapHW& map) {

    if (IsHoriz())
        for (int i = -1; i < 2; map[y - 1][x + i] = map[y + 1][x + i] = catter, i++);
    else
        for (int i = -1; i < 2; map[y + i][x - 1] = map[y + i][x + 1] = catter, i++);

    map[y][x] = tankC;
    POINT dt = dirInc[dir];
    map[y + dt.y][x + dt.x] = IsHoriz() ? duloH : duloV;
}

RECT area = { 2,2, width - 3,height - 3 };

void Ttank::Move(char w, char s, char a, char d, char fire) {

    char wasd[4] = { w,a,s,d };

    for (int i = 0; i < 4; i++)
        if (GetKeyState(wasd[i]) < 0) dir = (Tdir)i;

    POINT pt = dirInc[dir];
    Ttank old = *this;
    x += pt.x;
    y += pt.y;

    if (!IsCross(area, GetRect()) || 
        (CheckCrossAnyTank(GetRect(), this) != 0) ||
        (CheckCrossAnyBrick(GetRect()) != 0 ))
    {
        *this = old;
    }

    if (GetKeyState(fire) < 0)
        GetFreePula().SetPula(x + pt.x * 2, y + pt.y * 2, dir);
}

void Tbrick::Show(mapHW& map) {

    if (!use) return;

    for (int i = rct.left; i <= rct.right; i++)
        for (int j = rct.top; j <= rct.bottom; j++)
            if (tp == ttBrick)
                map[j][i] = fbrick;
            else
                map[j][i] = fstone;
}


RECT areaPula = { 0,0, width - 1, height - 1 };

void Tpula::Move() {

    if (!use) return;

    for (int i = 1; i < speed; i++) {

        x += dirInc[dir].x;
        y += dirInc[dir].y;
        RECT rct = { x,y,x,y };

        if (!IsCross(rct, areaPula)) use = 0;

        Tbrick* brick = CheckCrossAnyBrick(rct);

        if (brick) use = 0, brick->use = (brick->tp == ttStone);

        Ttank* tank = CheckCrossAnyTank(rct, 0);

        if (tank) use = 0, tank->SetToStart();

        if (!use) return;
    }
}


void CreateBattleField() {

    int pos = 0;

    for( int i = 5; i < width-5; i +=3)
        for (int j = 1; j < height - 1; j += 3) {

            brick[pos].SetPos(i, j);
            if (rand() % 5 == 0) brick[pos].tp == ttStone;
            if (rand() % 5 == 0) brick[pos].use = 0;
            pos++;
        }
}


int main()
{
    srand(time(0));
    CreateBattleField();
    do
    {
        tank[0].Move('W', 'A', 'S', 'D', VK_SPACE);
        tank[1].Move(38, 40, 37, 39, 13);
        for (int i = 0; i < pulaCnt; pula[i++].Move());

        scr.Clear();
        for (int i = 0; i < brickCnt; brick[i++].Show(scr.map));
        for (int i = 0; i < tankCnt; tank[i++].Show(scr.map));
        for (int i = 0; i < pulaCnt; pula[i++].Show(scr.map));
        scr.Show();

        Sleep(100);
    }
    while (GetKeyState(VK_ESCAPE) >= 0);

}
