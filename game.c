#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <WinUser.h>

typedef struct ball{
	int x, y;
	int dx, dy;
}ball;
typedef struct paddle{
	int x,y;
	int dy;
	int size;
	int score;
}paddle;
const int width = 30;
const int height = 70;
int gravity = 10;
void gotoxy(int ,int );
void init(ball *, paddle *);
void ballUpdate(ball *b, int *);
void paddleUpdate(paddle *);
int getCollision(ball , paddle *);
void onCollisionUpdate(ball *, paddle *, int );

int main()
{
	srand(time(NULL));
	HWND hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT windowSize = {0, 0, height-1, width-1};
	SetConsoleWindowInfo(hOut, TRUE, &windowSize);	//set custom size

	//setting current screen buffer size same with current window size
	CONSOLE_SCREEN_BUFFER_INFO scrBuffInfo;
	GetConsoleScreenBufferInfo(hOut, &scrBuffInfo);
	int BuffWidth = scrBuffInfo.dwSize.X;
	int BuffHeight = scrBuffInfo.dwSize.Y;
	COORD newBuffSize;
	newBuffSize.X = scrBuffInfo.srWindow.Right - scrBuffInfo.srWindow.Left + 1;
	newBuffSize.Y = scrBuffInfo.srWindow.Bottom - scrBuffInfo.srWindow.Top + 1;
	SetConsoleScreenBufferSize(hOut, newBuffSize);

	hOut = GetConsoleWindow();
	SetWindowLong(hOut, GWL_STYLE, GetWindowLong(hOut, GWL_STYLE) & ~WS_MINIMIZEBOX & ~WS_SIZEBOX);

	//disabling quick edit mode to disable mouse input
	//and resorted at the end
	HWND hIn = GetStdHandle(STD_INPUT_HANDLE);
	DWORD previous_mode;
	GetConsoleMode(hIn, &previous_mode);
	SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS|(previous_mode & ~ENABLE_QUICK_EDIT_MODE));
	
	//now the console is ready for pong :-)
	///////////////////////////////////////////////
	ball b;
	paddle p[2];
	int gameOver = 0, CollisionCode = 0;
	init(&b, p);

	printf("Press Any Key To Start PONG\n");
	getch();


	while(gameOver == 0)
	{
		system("cls");

		ballUpdate(&b, &gameOver);

		CollisionCode = getCollision(b, p);
		if(CollisionCode != -1)
			onCollisionUpdate(&b, p, CollisionCode);
		
		paddleUpdate(p);
		Sleep(20);
	}

	Sleep(1500);
	system("cls");
	printf("That was a good game");
	printf("\nPlayer 0 scored: %d", p[0].score);
	printf("\nPlayer 1 scored: %d", p[1].score);

	hIn = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hIn, &previous_mode);
	SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS|(previous_mode & ~ENABLE_QUICK_EDIT_MODE));
	
}

int getCollision(ball b, paddle *p)
{
	int i;
	for(i=0;i<p[0].size;i++){
		if(b.x <= p[0].x && b.y >= p[0].y && b.y <= p[0].y+p[0].size)
			return 0;
	}
	for(i=0;i<p[1].size;i++){
		if(b.x >= p[1].x && b.y >= p[1].y && b.y <= p[1].y+p[1].size)
			return 1;
	}
	return -1;
}

void onCollisionUpdate(ball *b, paddle *p, int CollisionCode)
{
	b->dx = -b->dx;
	if(CollisionCode == 0)
		p[0].score++;
	else if(CollisionCode == 1)
		p[1].score++;
}

void paddleUpdate(paddle *p)
{
	char c;
	if(kbhit())
		c = getch();

	switch (c)
	{
	case 'w':
		if(p[0].y > 0)
			p[0].y-=4;
		break;
	case 's':
		if(p[0].y + p[0].size < width-1)
			p[0].y+=4;
		break;
	case 72:	//up
		if(p[1].y > 0)
			p[1].y-=4;
		break;
	case 80:	//down
		if(p[1].y + p[1].size < width-1)
			p[1].y+=4;
		break;
	default:
		break;
	}

	int offset = 0, i, j;
	offset = p[0].y;
	for(i=0;i<p[0].size;i++,offset++){
		gotoxy(p[0].x, offset);
		printf("|");
	}
	offset = p[1].y;
	for(i=0;i<p[1].size;i++,offset++){
		gotoxy(p[1].x, offset);
		printf("|");
	}
}

void ballUpdate(ball *b, int *gameover)
{
	b->x += b->dx;
	b->y += b->dy;

	if(b->x < 0){
		*gameover = 1;
	}	
	else if (b->x > height-1){
		*gameover = 1;
	}
	if(b->y < 0){
		b->y = 0;
		b->dy = -b->dy;
	}
	else if(b->y > width-1){
		b->y = width-1;
		b->dy = -b->dy;
	}


	gotoxy(b->x,b->y);
		printf("O");	
}

void init(ball *b, paddle *p)
{
	b->x = rand()%(height-1);
	b->y = rand()%(width-1);
	b->dx = (1 + rand()%2/5) * (rand()%2 == 0 ? -1 : 1);
	b->dy = (1 + rand()%2/5) * (rand()%2 == 0 ? -1 : 1);

	int i=0;
	for(i=0;i<2;i++)
	{
		p[i].size = 6;
		p[i].x = i*(height-10) + 5;
		p[i].y = 0;
		p[i].score = 0;
	}
}

void gotoxy(int x, int y)
{
	static HANDLE hstdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	
	if(!hstdout)
		hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hstdout, coord);
}
