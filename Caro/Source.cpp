#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>

using namespace std;

#pragma comment (lib, "Winmm.lib")

#define BOARD_SIZE 10
#define LEFT 5
#define TOP 4
#define Continue 2

class _Common {
public:
	static void fixConsoleWindow();
	static void gotoXY(int, int);
};
void _Common::gotoXY(int pX, int pY) {
	COORD coord;
	coord.X = pX;
	coord.Y = pY;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void _Common::fixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}
class _Point {
	int _x, _y, _check;
public:
	_Point();
	_Point(int, int);
	bool setCheck(int);
	int getX();
	int getY();
	int getCheck();
	void setX(int);
	void setY(int);
};
_Point::_Point() { _x = _y = _check = 0; }
_Point::_Point(int pX, int pY) {
	_x = pX; _y = pY;
	_check = 0;
}
int _Point::getX() { return _x; }
int _Point::getY() { return _y; }
int _Point::getCheck() { return _check; }
void _Point::setX(int pX) { _x = pX; }
void _Point::setY(int pY) { _y = pY; }
bool _Point::setCheck(int pCheck) {
	if (pCheck == -1 || pCheck == 1 || pCheck == 0) {
		_check = pCheck;
		return true;
	}
	return false;
};

class _Board {
private:
	int _size;
	int _left, _top;
	_Point** _pArr;
public:
	_Board(int, int, int);
	~_Board();
	int getSize();
	int getLeft();
	int getTop();
	int getXAt(int, int);
	int getYAt(int, int);
	void resetData();
	void drawBoard();
	int checkBoard(int, int, bool);
	int testBoard(int, int); // sua lai tra ve continue
	_Point getPoint(int, int);
	int checkColumn(int, int);
	int checkRow(int, int);
	int leftDiagonal(int, int);
	int rightDiagonal(int, int);

	void setPoint(int, int, int); // moi them
	//Ai
	int checkBoardWith_Ai(int, int);
};
void _Board::setPoint(int x, int y, int data) { _pArr[x][y].setCheck(data); }
_Point _Board::getPoint(int x, int y) {
	return _pArr[x][y];
}
int _Board::getSize() { return _size; }
int _Board::getLeft() { return _left; }
int _Board::getTop() { return _top; }
int _Board::getXAt(int i, int j) {
	return _pArr[i][j].getX();
}
int _Board::getYAt(int i, int j) {
	return _pArr[i][j].getY();
}

_Board::_Board(int pSize, int pX, int pY) {
	_size = pSize;
	_left = pX;
	_top = pY;
	_pArr = new _Point * [pSize];
	for (int i = 0; i < pSize; i++) _pArr[i] = new _Point[pSize];
}

_Board::~_Board() {
	for (int i = 0; i < _size; i++) delete[] _pArr[i];
	delete[] _pArr;
}

void _Board::resetData() {
	if (_size == 0) return; // Firstly calling constructor before calling resetData
	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < _size; j++) {
			_pArr[i][j].setX(4 * j + _left + 2); // x-value of boardgame
			_pArr[i][j].setY(2 * i + _top + 1); // y-value of boardgame
			_pArr[i][j].setCheck(0);
		}
	}
}
void _Board::drawBoard() {
	if (_pArr == NULL) return; // firstly call constructor
	HANDLE hConsoleColor;
	hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleColor, 3);

	for (int i = 1; i <= _size; i++) {
		_Common::gotoXY(_left, _top + 2 * i);
		for (int j = 0; j <= 4 * _size; j++) {
			cout << char(196);
		}
	}
	for (int i = 0; i <= _size; i++) {
		for (int j = 0; j <= 2 * _size; j++) {
			_Common::gotoXY(_left + 4 * i, _top + j);
			cout << char(179);
		}
	}

	//khung
	_Common::gotoXY(_left - 3, _top);
	for (int i = 0; i <= 4 * _size + 6; i++)
		cout << char(219);
	_Common::gotoXY(_left - 3, _top + 2 * _size);
	for (int i = 0; i <= 4 * _size + 4; i++)
		cout << char(219);

	for (int i = 0; i <= 2 * _size; i++) {
		_Common::gotoXY(_left - 3, _top + i);
		cout << char(219) << char(219);
	}

	for (int i = 0; i <= 2 * _size; i++) {
		_Common::gotoXY(_left + 4 * _size + 2, _top + i);
		cout << char(219) << char(219);
	}
	_Common::gotoXY(_pArr[0][0].getX(), _pArr[0][0].getY());//move to the 1st cell
}
int _Board::checkBoard(int pX, int pY, bool pTurn) {
	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < _size; j++) {
			if (_pArr[i][j].getX() == pX && _pArr[i][j].getY() == pY && _pArr[i][j].getCheck() == 0) {
				if (pTurn) _pArr[i][j].setCheck(-1);// If current turn is true: c = -1
				else _pArr[i][j].setCheck(1);// If current turn is false: c = 1
				return _pArr[i][j].getCheck();
			}
		}
	}
	return 0;
}

int _Board::checkColumn(int pX, int pY) {
	int enemy = 0;
	int ally = 0;
	int row = (pY - 5) / 2;
	int column = (pX - 7) / 4;
	int check = getPoint(row, column).getCheck();

	for (int count = 1; count <= 5 && count + row < _size; count++) {
		if (check == getPoint(row + count, column).getCheck())
			ally++;
		else if (getPoint(row + count, column).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && row - count >= 0; count++) {
		if (check == getPoint(row - count, column).getCheck())
			ally++;
		else if (getPoint(row - count, column).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	if (ally == 4 && enemy < 2)
		return check;
	return false;

}


//
int _Board::checkRow(int pX, int pY) {
	int enemy = 0;
	int ally = 0;
	int row = (pY - 5) / 2;
	int column = (pX - 7) / 4;
	int check = getPoint(row, column).getCheck();

	for (int count = 1; count <= 5 && count + column < _size; count++) {
		if (check == getPoint(row, column + count).getCheck())
			ally++;
		else if (getPoint(row, column + count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0; count++) {
		if (check == getPoint(row, column - count).getCheck())
			ally++;
		else if (getPoint(row, column - count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	if (ally == 4 && enemy < 2)
		return check;
	return false;

}
//

int _Board::rightDiagonal(int pX, int pY) {
	int enemy = 0;
	int ally = 0;
	int row = (pY - 5) / 2;
	int column = (pX - 7) / 4;
	int check = getPoint(row, column).getCheck();

	for (int count = 1; count <= 5 && count + column < _size && row + count < _size; count++) {
		if (check == getPoint(row + count, column + count).getCheck())
			ally++;
		else if (getPoint(row + count, column + count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0 && row - count >= 0; count++) {
		if (check == getPoint(row - count, column - count).getCheck())
			ally++;
		else if (getPoint(row - count, column - count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	if (ally == 4 && enemy < 2)
		return check;
	return false;

}

int _Board::leftDiagonal(int pX, int pY) {
	int enemy = 0;
	int ally = 0;
	int row = (pY - 5) / 2;
	int column = (pX - 7) / 4;
	int check = getPoint(row, column).getCheck();

	for (int count = 1; count <= 5 && count + column < _size && row - count >= 0; count++) {
		if (check == getPoint(row - count, column + count).getCheck())
			ally++;
		else if (getPoint(row - count, column + count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0 && row + count < _size; count++) {
		if (check == getPoint(row + count, column - count).getCheck())
			ally++;
		else if (getPoint(row + count, column - count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	if (ally == 4 && enemy < 2)
		return check;
	return false;

}

int _Board::testBoard(int pX, int pY) {
	if (checkColumn(pX, pY) != 0)
		return checkColumn(pX, pY);
	if (checkRow(pX, pY) != 0)
		return checkRow(pX, pY);
	if (rightDiagonal(pX, pY) != 0)
		return rightDiagonal(pX, pY);
	if (leftDiagonal(pX, pY) != 0)
		return leftDiagonal(pX, pY);
	return Continue;
}

int _Board::checkBoardWith_Ai(int pX, int pY) {
	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < _size; j++) {
			if (_pArr[i][j].getX() == pX && _pArr[i][j].getY() == pY && _pArr[i][j].getCheck() == 0) {
				_pArr[i][j].setCheck(-1);// Nếu lượt hiện hành là người
				return _pArr[i][j].getCheck();
			}
		}
	}
	return 0;
}

//
class _Game {
	_Board* _b;// a board game
	bool _turn;// turn: true for the 1st player and false for the 2nd player
	int _x, _y; // current position of cursor
	int _command; // input-key from the players
	bool _loop; // decision bool variable to exit game or not
	long* attackPoint, * deffensePoint;
public:
	_Game(int, int, int);
	~_Game();
	int getCommand();
	bool isContinue();
	char waitKeyBoard(); // Receiving keyboard from players
	char askContinue();
	void startGame(); // Function to start the game
	void exitGame(); // Function to exit the game
	int processFinish(); // sua case 2 sang continue
	bool processCheckBoard();
	void moveRight();
	void moveLeft();
	void moveUp();
	void moveDown();

	//Ai
	_Board* get_b() { return _b; }
	_Game(int, int, int, int);
	long aPointCol(int, int);
	long aPointRow(int, int);
	long aPointLDiag(int, int);
	long aPointRDiag(int, int);
	long dPointCol(int, int);
	long dPointRow(int, int);
	long dPointLDiag(int, int);
	long dPointRDiag(int, int);
	bool processCheckBoard_Ai();
	int processFinishWith_Ai();
	_Point* findingPos_Ai();
};
_Game::_Game(int pSize, int pLeft, int pTop) {
	_b = new _Board(pSize, pLeft, pTop);
	_loop = _turn = true;
	_command = -1; // Assign turn and default key
	_x = pLeft; _y = pTop;
	attackPoint = deffensePoint = NULL;
}

_Game::_Game(int pSize, int pLeft, int pTop, int modeAi) {
	_b = new _Board(pSize, pLeft, pTop);
	_loop = _turn = true;
	_command = -1; // Assign turn and default key
	_x = pLeft; _y = pTop;
	attackPoint = deffensePoint = NULL;

	if (modeAi == 1) {
		long aPoint_Easy[] = { 4, 5, 6, 6, 8, 9, 10 };
		long dPoint_Easy[] = { 3, 4, 5, 2, 2, 2, 2 };
		attackPoint = new long[7];
		for (int i = 0; i < 7; i++)
			attackPoint[i] = aPoint_Easy[i];
		deffensePoint = new long[7];
		for (int i = 0; i < 7; i++)
			deffensePoint[i] = dPoint_Easy[i];
	}
	else if (modeAi == 2) {
		long dPoint_Difficult[] = { 0, 5, 45, 405, 3645, 32805, 295245 };
		long aPoint_Difficult[] = { 0, 8, 72, 648, 5832, 52488, 472392 };
		attackPoint = new long[7];
		for (int i = 0; i < 7; i++)
			attackPoint[i] = aPoint_Difficult[i];
		deffensePoint = new long[7];
		for (int i = 0; i < 7; i++)
			deffensePoint[i] = dPoint_Difficult[i];
	}
}

long _Game::aPointCol(int row, int column) {
	int enemy = 0;
	int ally = 0;

	int minus = 0;
	if (row * column == 0 || (row || column) == BOARD_SIZE) minus = 9; // tranh th may duoc 4 con ma danh vo may cai bien

	for (int count = 1; count <= 5 && count + row < _b->getSize(); count++) {
		if (_b->getPoint(row + count, column).getCheck() == 1)
			ally++;
		else if (_b->getPoint(row + count, column).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && row - count >= 0; count++) {
		if (_b->getPoint(row - count, column).getCheck() == 1)
			ally++;
		else if (_b->getPoint(row - count, column).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}
	if (enemy == 2 && ally == 4) return 0;
	return attackPoint[ally] - deffensePoint[enemy + 1];
}

long _Game::aPointRow(int row, int column) {
	int enemy = 0;
	int ally = 0;

	int minus = 0;
	if (row * column == 0 || (row || column) == BOARD_SIZE) minus = 9; // tranh th may duoc 4 con ma danh vo may cai bien

	for (int count = 1; count <= 5 && count + column < _b->getSize(); count++) {
		if (1 == _b->getPoint(row, column + count).getCheck())
			ally++;
		else if (_b->getPoint(row, column + count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0; count++) {
		if (1 == _b->getPoint(row, column - count).getCheck())
			ally++;
		else if (_b->getPoint(row, column - count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}
	if (enemy == 2 && ally == 4) return 0;
	return attackPoint[ally] - deffensePoint[enemy + 1];
}

long _Game::aPointLDiag(int row, int column) {
	int enemy = 0;
	int ally = 0;

	int minus = 0;
	if (row * column == 0 || (row || column) == BOARD_SIZE) minus = 9; // tranh th may duoc 4 con ma danh vo may cai bien

	for (int count = 1; count <= 5 && count + column < _b->getSize() && row - count >= 0; count++) {
		if (1 == _b->getPoint(row - count, column + count).getCheck())
			ally++;
		else if (_b->getPoint(row - count, column + count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0 && row + count < _b->getSize(); count++) {
		if (1 == _b->getPoint(row + count, column - count).getCheck())
			ally++;
		else if (_b->getPoint(row + count, column - count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}
	if (enemy == 2 && ally == 4) return 0;
	return attackPoint[ally] - deffensePoint[enemy + 1];
}

long _Game::aPointRDiag(int row, int column) {
	int enemy = 0;
	int ally = 0;

	int minus = 0;
	if (row * column == 0 || (row || column) == BOARD_SIZE) minus = 9; // tranh th may duoc 4 con ma danh vo may cai bien

	for (int count = 1; count <= 5 && count + column < _b->getSize() && row + count < _b->getSize(); count++) {
		if (1 == _b->getPoint(row + count, column + count).getCheck())
			ally++;
		else if (_b->getPoint(row + count, column + count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0 && row - count >= 0; count++) {
		if (1 == _b->getPoint(row - count, column - count).getCheck())
			ally++;
		else if (_b->getPoint(row - count, column - count).getCheck() != 0) {
			enemy++;
			break;
		}
		else
			break;
	}
	if (enemy == 2 && ally == 4) return 0;
	return attackPoint[ally] - deffensePoint[enemy + 1];
}

long _Game::dPointCol(int row, int column) {
	int enemy = 0;
	int ally = 0;

	for (int count = 1; count <= 5 && count + row < _b->getSize(); count++) {
		if (1 == _b->getPoint(row + count, column).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row + count, column).getCheck() != 0)
			enemy++;
		else
			break;
	}

	for (int count = 1; count <= 5 && row - count >= 0; count++) {
		if (1 == _b->getPoint(row - count, column).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row - count, column).getCheck() != 0)
			enemy++;
		else
			break;
	}
	if (ally == 2) return 0;
	return deffensePoint[enemy];
}

long _Game::dPointRow(int row, int column) {
	int enemy = 0;
	int ally = 0;

	for (int count = 1; count <= 5 && count + column < _b->getSize(); count++) {
		if (1 == _b->getPoint(row, column + count).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row, column + count).getCheck() != 0)
			enemy++;
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0; count++) {
		if (1 == _b->getPoint(row, column - count).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row, column - count).getCheck() != 0)
			enemy++;
		else
			break;
	}
	if (ally == 2) return 0;
	return deffensePoint[enemy];
}

long _Game::dPointLDiag(int row, int column) {
	int enemy = 0;
	int ally = 0;

	for (int count = 1; count <= 5 && count + column < _b->getSize() && row - count >= 0; count++) {
		if (1 == _b->getPoint(row - count, column + count).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row - count, column + count).getCheck() != 0)
			enemy++;
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0 && row + count < _b->getSize(); count++) {
		if (1 == _b->getPoint(row + count, column - count).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row + count, column - count).getCheck() != 0)
			enemy++;
		else
			break;
	}
	if (ally == 2) return 0;
	return deffensePoint[enemy];
}

long _Game::dPointRDiag(int row, int column) {
	int enemy = 0;
	int ally = 0;

	for (int count = 1; count <= 5 && count + column < _b->getSize() && row + count < _b->getSize(); count++) {
		if (1 == _b->getPoint(row + count, column + count).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row + count, column + count).getCheck() != 0)
			enemy++;
		else
			break;
	}

	for (int count = 1; count <= 5 && column - count >= 0 && row - count >= 0; count++) {
		if (1 == _b->getPoint(row - count, column - count).getCheck())
		{
			ally++;
			break;
		}
		else if (_b->getPoint(row - count, column - count).getCheck() != 0)
			enemy++;
		else
			break;
	}
	if (ally == 2) return 0;
	return deffensePoint[enemy];
}

_Point* _Game::findingPos_Ai() {
	_Point* posResult = new _Point();
	long max = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (_b->getPoint(i, j).getCheck() == 0)
			{
				long tempPoint;
				long attackPoint = aPointCol(i, j) + aPointRow(i, j) + aPointLDiag(i, j) + aPointRDiag(i, j);
				long deffensePoint = dPointCol(i, j) + dPointRow(i, j) + dPointLDiag(i, j) + dPointRDiag(i, j);
				if (attackPoint > deffensePoint)
					tempPoint = attackPoint;
				else
					tempPoint = deffensePoint;

				if (tempPoint > max)
				{
					max = tempPoint;
					posResult->setX(_b->getXAt(i, j));
					posResult->setY(_b->getYAt(i, j));
				}
			}
		}
	return posResult;
}

_Game::~_Game() { 
	delete _b;
	delete[] attackPoint;
	delete[] deffensePoint;
}

int _Game::getCommand() { return _command; }
bool _Game::isContinue() { return _loop; }
char _Game::waitKeyBoard() {
	_command = toupper(_getch());
	return _command;
}
char _Game::askContinue() {
	_Common::gotoXY(0, _b->getYAt(_b->getSize() - 1, _b->getSize() - 1) + 4);
	return waitKeyBoard();
}

void _Game::startGame() {
	system("cls");
	PlaySound("bgSound", NULL, SND_ASYNC | SND_LOOP);
	_b->resetData(); // Setting the original data
	_b->drawBoard(); // Draw boad
	_x = _b->getXAt(0, 0);
	_y = _b->getYAt(0, 0);
}
void _Game::exitGame() {
	system("cls");
	//Maybe save game before stopping
	_loop = false;
}

bool _Game::processCheckBoard() {
	switch (_b->checkBoard(_x, _y, _turn)) {
	case -1:
		printf("X");
		break;
	case 1:
		printf("O");
		break;
	case 0: return false; // Tick the cell marked
	}
	_turn = !_turn;
	return true;
}
int _Game::processFinish() {
	// Move to the reasonable place to print string win/lose/draw
	_Common::gotoXY(0, _b->getYAt(_b->getSize() - 1, _b->getSize() - 1) + 2);
	int pWhoWin = _b->testBoard(_x, _y);
	switch (pWhoWin) {
	case -1:
		printf("The player %d won and the player %d lost\n", true, false);
		break;
	case 1:
		printf("The player %d won and the player %d lost\n", false, true);
		break;
	case 0:
		printf("The player %d draw with the player %d\n", false, true);
		break;
	case Continue:
		_turn = !_turn; // change turn if nothing happen
	}
	_Common::gotoXY(_x, _y);// Return the current position of cursor
	return pWhoWin;
}

bool _Game::processCheckBoard_Ai()	{
	switch (_b->checkBoardWith_Ai(_x, _y)) {
	case -1: {
		cout << "X";
		_Common::gotoXY(_x, _y);
		break;
	}	
	case 0: return false;
	}
	return true;
}

int _Game::processFinishWith_Ai()	{
 	int pWhoWin = _b->testBoard(_x, _y);
	switch (pWhoWin) {
	case -1:
		printf("The player %d won and  the machine lost\n", true);
		break;
	case 1:
		printf("The machine won and the player %d lost\n", true);
		break;
	case 0:
		printf("The machine and the player are draw\n");
		break;
	case Continue: {
		_Point* square = findingPos_Ai();
		_x = square->getX();
		_y = square->getY();
		if (processCheckBoard_Ai() != 0) {
			_b->setPoint((_y - 5) / 2, (_x - 7) / 4, 1);
			_Common::gotoXY(_x, _y);
			cout << "O";
		}
		int check = _b->testBoard(_x, _y);
		if (check != Continue) {
			if (check == 1)
				cout << "*****Machine win!*****\n";
			else if(check == 0)
				cout << "*****Draw*****\n";
			return check;
		}
		else
			_turn = !_turn;
		break;
	}
	}
	_Common::gotoXY(_x, _y); // vi tri hien hanh
	return pWhoWin;
}

void _Game::moveRight() {
	if (_x < _b->getXAt(_b->getSize() - 1, _b->getSize() - 1)) {
		_x += 4;
		_Common::gotoXY(_x, _y);
	}
}
void _Game::moveLeft() {
	if (_x > _b->getXAt(0, 0)) {
		_x -= 4;
		_Common::gotoXY(_x, _y);
	}
}
void _Game::moveDown() {
	if (_y < _b->getYAt(_b->getSize() - 1, _b->getSize() - 1)) {
		_y += 2;
		_Common::gotoXY(_x, _y);
	}
}
void _Game::moveUp() {
	if (_y > _b->getYAt(0, 0)) {
		_y -= 2;
		_Common::gotoXY(_x, _y);
	}
}

void newGame_Ai(){ // lam menu nho chia lam 2 che do roi sua o duoi
	_Game g(BOARD_SIZE, LEFT, TOP, 2); // 1: de     2: kho
	g.startGame();
	cout << "PLAYER 1 VS MACHINE";
	_Common::gotoXY(g.get_b()->getXAt(0, 0), g.get_b()->getYAt(0, 0));
	while (g.isContinue()) {
		g.waitKeyBoard();		
		if (g.getCommand() == 27) g.exitGame();
		else {
			if (g.getCommand() == 'A' || g.getCommand() == 75)
				g.moveLeft();
			if (g.getCommand() == 'W' || g.getCommand() == 72)
				g.moveUp();
			if (g.getCommand() == 'S' || g.getCommand() == 80)
				g.moveDown();
			if (g.getCommand() == 'D' || g.getCommand() == 77)
				g.moveRight();
			if (g.getCommand() == 13 || g.getCommand() == 32)
			{
				if (g.processCheckBoard_Ai())
				{
					switch (g.processFinishWith_Ai())
					{
					case -1: case 1: case 0:

						if (g.askContinue() != 'Y')
						{
							_Common::gotoXY(100, 105);
							g.exitGame();
						}
						else
						{
							g.startGame();
							_Common::gotoXY(g.get_b()->getXAt(0, 0), g.get_b()->getYAt(0, 0));
						}
					}
				}
			}
		}
	}
}

int main() {
	//system("color fa");
	_Common::fixConsoleWindow();
	newGame_Ai();
	return 0;
}