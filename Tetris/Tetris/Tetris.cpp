#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<Windows.h>
#include<cstdlib>
#include<ctime>
#include<cstdio>
#include<cstring>

using namespace std;


const int FLD_WIDTH = 20;
const int FLD_HEIGHT = 30;
const int SCR_WIDTH = FLD_WIDTH * 2;
const int SCR_HEIGHT = FLD_HEIGHT;

const char C_FIG = 219;
const char C_FIELD = 176;
const char C_FIG_DOWN = 178;

typedef char TScreenMap[SCR_HEIGHT][SCR_WIDTH];
typedef char TFieldMap[FLD_HEIGHT][FLD_WIDTH];


const int SHP_WIDTH = 4;
const int SHP_HEIGHT = 4;
typedef char TShape[SHP_HEIGHT][SHP_WIDTH];

char* shpArr[]{
	(char*)".....**..**.....",
	(char*)"....****........",
	(char*)"....***..*......",
	(char*)".....***.*.....",
	(char*)".....**.***.....",};
const int SHP_ARR_CNT = sizeof(shpArr) / sizeof(shpArr[0]);


void SetCurPos(int x, int y) {

	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

class TScreen {
	void SetEnd() { scr[SCR_HEIGHT - 1][SCR_WIDTH - 1] = '\0'; }

public:
	TScreenMap scr;
	TScreen() { Clear(); }
	void Clear() { memset(scr, '.', sizeof(scr)); }
	void Show() { SetCurPos(0, 0); SetEnd(); cout << scr[0]; }
};

class TField {

public:
	TFieldMap fld;
	TField() { Clear(); }
	void Clear() { memset(fld, C_FIELD, sizeof(fld)); }
	void Put(TScreenMap& scr);
	void Burning();
};

class TFigure {
	int x, y;
	TShape vid; 
	char turn;
	COORD coord[SHP_WIDTH * SHP_HEIGHT];
	int coordCnt;
	TField *field = 0;

public: 
	TFigure() { memset(this, 0, sizeof(*this)); }
	void FieldSet(TField *_field) { field = _field; }
	void Shape(char* _vid) { memcpy(vid, _vid, sizeof(vid)); }
	void Pos(int _x, int _y) { x = _x; y = _y; CalcCoord(); }
	char TurnGet() { return turn; }
	void TurnSet(char _turn);
	void Put(TScreenMap& scr);
	void Put(TFieldMap& fld);
	bool Move(int dx, int dy);
	int Check();

private: 
	void CalcCoord();
};



class TGame {
	
	TScreen screen;
	TField field;
	TFigure figure;

public:
	TGame();
	void PlayerControl();
	void Move();
	void Show();

};

void TGame::PlayerControl() {

	static int trn = 0;
	if (GetKeyState('W') < 0) trn += 1;
	if (trn == 1) figure.TurnSet(figure.TurnGet() + 1), trn++;
	if (GetKeyState('W') >= 0) trn = 0;



	if (GetKeyState('S') < 0)  figure.Move(0, 1);
	if (GetKeyState('A') < 0)  figure.Move(-1, 0);
	if (GetKeyState('D') < 0)  figure.Move(1, 0);
}

void TGame::Show() {

	screen.Clear();
	field.Put(screen.scr);
	figure.Put(screen.scr);
	screen.Show();
}

void TGame::Move() {

	static int tick = 0;
	tick++;
	if (tick >= 5) {
		
		if (!figure.Move(0, 1))
		{
			figure.Put(field.fld);
			figure.Shape(shpArr[rand() % SHP_ARR_CNT]);
			figure.Pos(FLD_WIDTH / 2 - SHP_WIDTH / 2, 0);
		}

		field.Burning();
		tick = 0;
	}

}

TGame::TGame() {

	figure.FieldSet(&field);
	figure.Shape(shpArr[rand()% SHP_ARR_CNT]);
	figure.Pos(FLD_WIDTH / 2 - SHP_WIDTH / 2, 0);
}


void TFigure::Put(TScreenMap& scr) {

	for (int i = 0; i < coordCnt; i++)
		scr[coord[i].Y][coord[i].X * 2] = scr[coord[i].Y][coord[i].X * 2 + 1] = C_FIG;
}


void TFigure::Put(TFieldMap& fld) {

	for (int i = 0; i < coordCnt; i++)
		fld[coord[i].Y][coord[i].X] = C_FIG_DOWN;
}


void TFigure::TurnSet(char _turn){

	int oldTurn = turn;
	turn = (_turn > 3 ? 0 : (_turn < 0 ? 3 : _turn));
	int chk = Check();
	
	if (chk == 0) 
		return;

	if (chk == 1) {

		int xx = x;
		int k = (x > (FLD_WIDTH / 2) ? -1 : +1);

		for (int i = 1; i < 3; i++) {

			x += k;

			if (Check() == 0) return; 
		}

		x = xx;
	}

	turn = oldTurn;
	CalcCoord();

}

bool TFigure::Move(int dx, int dy) {

	int oldX = x, oldY = y;
	Pos(x + dx, y + dy);
	int chk = Check();

	if (chk >= 1) {

		Pos(oldX, oldY);
		if (chk == 2)
			return false;
	}
	return true;
}

int TFigure::Check()
{
	CalcCoord();
	for (int i = 0; i < coordCnt; i++)
		if (coord[i].X  < 0 || coord[i].X >= FLD_WIDTH)
			return 1;

	for (int i = 0; i < coordCnt; i++)
		if (coord[i].Y >= FLD_HEIGHT || field->fld[coord[i].Y][coord[i].X] == C_FIG_DOWN)
			return 2;

	return 0;
}
void TFigure::CalcCoord() {

	int xx, yy;
	coordCnt = 0;

	for(int i = 0; i < SHP_WIDTH; i++)
		for(int j = 0; j < SHP_HEIGHT; j++)
			if (vid[j][i] == '*') {

				if (turn == 0) xx = x + i, yy = y + j;
				if (turn == 1) xx = x + (SHP_HEIGHT - j - 1), yy = y + i;
				if (turn == 2) xx = x + (SHP_WIDTH - i - 1), yy = y + (SHP_HEIGHT - j - 1);
				if (turn == 3) xx = x + j, yy = y + (SHP_HEIGHT - i - 1) + (SHP_WIDTH - SHP_HEIGHT);

				COORD cC = { (short)xx, (short)yy };
				coord[coordCnt] = cC;
				coordCnt++;
			}
}


void TField::Put(TScreenMap& scr) {

	for (int i = 0; i < FLD_WIDTH; i++)
		for (int j = 0; j < FLD_HEIGHT; j++)
			scr[j][i * 2] = scr[j][i * 2 + 1] = fld[j][i];
}

void TField::Burning() {

	for (int j = FLD_HEIGHT - 1; j >= 0; j--) {

		static bool fillLine;
		fillLine = true;

		for (int i = 0; i < FLD_WIDTH; i++) {

			if (fld[j][i] != C_FIG_DOWN)
				fillLine = false;

			if (fillLine) {

				for (int y = j; y >= 1; y--) {
					memcpy(fld[y], fld[y - 1], sizeof(fld[y]));
					return;
				}
			}
		}
	}
}

int main()
{
	char command[1000];
	sprintf(command, "mode con cols=%d lines=%d", SCR_WIDTH, SCR_HEIGHT);
	system(command);

	srand(time(0));
	TGame game;

	while (1) {

		game.PlayerControl();
		game.Move();
		game.Show();

		if (GetKeyState(VK_ESCAPE) < 0) break;
		Sleep(50);
	}
  
}
