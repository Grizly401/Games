#include <iostream>
#include <windows.h> // Необходим для работы с цветом в консоли Windows
#include <conio.h>

using namespace std;

const int WIDTH = 25;
const int HEIGHT = 12;
typedef char mapHW[HEIGHT][WIDTH];


enum ConsoleColors {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_YELLOW = 6,
    COLOR_WHITE = 7,
    COLOR_GRAY = 8,
    COLOR_BRIGHT_BLUE = 9,
    COLOR_BRIGHT_GREEN = 10,
    COLOR_BRIGHT_CYAN = 11,
    COLOR_BRIGHT_RED = 12,
    COLOR_BRIGHT_MAGENTA = 13,
    COLOR_BRIGHT_YELLOW = 14,
    COLOR_BRIGHT_WHITE = 15
};

// Константы для элементов карты
const char WALL = '#';
const char FLOOR = ' ';
const char PLAYER = 'P';
const char ENEMY = 'E';
const char TREE = 'T';
const char WATER = 'W';
const char CHEST = 'C';

// Функция для установки цвета текста и фона в консоли
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

struct Position {
    int x;
    int y;
};


enum Tdir { Rup = 0, Rdowm, Rleft, Rright };

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
    void Move(char w, char s, char a, char d, char fire);
    void SetToStart() { x = sX; y = sY; }
    bool IsHoriz() { return (dir == Rright || dir == Rleft); }
    RECT GetRect() { RECT r = { x - 1, y - 1, x + 1, y + 1 }; return r; }
};




int main() {
    // Размеры карты
    char map[HEIGHT][WIDTH];


    Position playerPos = { 12, 6 }; // Начальная позиция игрока
    map[playerPos.y][playerPos.x] = PLAYER;

    bool gameRunning = true;
    while (gameRunning) {
        system("cls"); // Очищаем консоль

    // Заполнение карты базовыми элементами
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) {
                map[y][x] = '#'; // Символ стены
            }
            else {
                map[y][x] = ' '; // Символ земли/пустого пространства
            }
        }
    }

    // Добавление объектов на карту
    map[3][7] = 'T';   // Дерево
    map[6][12] = 'P';  // Игрок
    map[9][4] = 'E';   // Враг
    map[5][18] = 'W';  // Вода
    map[7][5] = 'C';   // Сундук

    // Отрисовка карты с цветами
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char c = map[y][x];

            // Выбор цвета в зависимости от символа
            switch (c) {
            case WALL: setColor(COLOR_GRAY); break;
            case PLAYER: setColor(COLOR_BRIGHT_BLUE); break;
            case 'T': // Деревья - зеленый
                setColor(10);
                break;
            case 'E': // Враги - красный
                setColor(12);
                break;
            case 'W': // Вода - голубой
                setColor(11);
                break;
            case 'C': // Сундуки - желтый
                setColor(14);
                break;
            default:  // Остальное - белый
                setColor(7);
            }

            cout << c; // Вывод символа с выбранным цветом
        }
        cout << endl; // Переход на новую строку после каждой строки карты
    }

    // Возвращаем стандартный цвет консоли
    setColor(7);

    }

    return 0;
}