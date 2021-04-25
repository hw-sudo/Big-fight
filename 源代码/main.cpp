#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<math.h>
#include<graphics.h>
#include<easyx.h>


#define FilePath1 "timeer.txt"
#define FilePath "counter.txt"
#define WIDTH 1024								// ��Ļ��
#define HEIGHT 576								// ��Ļ��
#define MAPW (WIDTH*4)								// ��ͼ��
#define MAPH (HEIGHT*4)							// ��ͼ��
#define AINUM 100 							    // AI ����
#define FNUM 2000									// FOOD ����
#define DISTANCE(x1,y1,x2,y2)	(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))		//�������

struct FOOD {
	bool eat;
	COLORREF color;								// ��ɫ
	int x, y;									// ����
	char type;
};

struct BALL {										//С��ṹ��
	bool life;									//����
	COLORREF color;								//��ɫ
	int x, y;									//����
	float r;										//�뾶
};

FOOD food[FNUM];									//ʳ��
BALL mover = { 1,RGB(0,0,0),0,0,0 };				//���
BALL ai[AINUM] = { 1,RGB(0,0,0),0,0,0 };			//AI

void ChooseSpeed();
void move(BALL* ball);								// ����ƶ�
void draw();										// ��ͼ
void start();										// ��Ϸ��ʼ
void setall();									// ��ʼ��
void AI();										// AI
void Food();										// ʳ��
void starttime();
int readCount();
void writeCount(int);
int readTime();
void writeTime(int);
void endtime();

clock_t start_t, end_t;
DWORD* pBuffer;									// �Դ�ָ��
int eaten = 0;									// �� AI ������
int ai_eaten = 0;									// AI �� AI������
float asp = 1;									// ��������
float Time = 0;									// ʱ��
int total_t;
int speed;

int main() {
	starttime();

	initgraph(WIDTH, HEIGHT);
	start();
	ChooseSpeed();
	setall();
	BeginBatchDraw();

	while (true) {
		move(&mover);
		AI();
		Food();
		draw();
		FlushBatchDraw();							// ��ʾ����Ļ�������
		Sleep(1);
	}
}

void ChooseSpeed() {
	switch (getch()) {
	case 1:speed = 4;
	case 2:speed = 3;
	case 3:speed = 2;
	default:speed = 4;
	}
}

int readCount() {
	FILE *fp;
	int count;
	if ((fp = fopen(FilePath, "r")) == NULL) {
		return 0;
	}
	else
		fscanf(fp, "%d", &count);
	fclose(fp);
	return count;
}

void writeCount(int count) {
	FILE *fp;
	if ((fp = fopen(FilePath, "w")) == NULL) {
		printf("�޷����������ļ�:counter.txt��\n");
		return;
	}
	else
		fprintf(fp, "%d", count);
	fclose(fp);
}

int readTime() {
	FILE *fp1;
	int time;
	if ((fp1 = fopen(FilePath1, "r")) == NULL) {
		return 0;
	}
	else
		fscanf(fp1, "%d", &time);
	fclose(fp1);
	return time;
}

void writeTime(int time) {
	FILE *fp1;
	if ((fp1 = fopen(FilePath1, "w")) == NULL) {
		printf("�޷����������ļ���timeer.txt��\n");
		return;
	}
	else
		fprintf(fp1, "%d", time);
	fclose(fp1);
}

void starttime() {
	start_t = clock();
	writeCount(readCount() + 1);
}

void endtime() {
	closegraph();
	initgraph(WIDTH, HEIGHT);
	cleardevice();
	BeginBatchDraw();
	setbkcolor(WHITE);							// ��ɫ����
	cleardevice();								// ��ʼ������
	settextcolor(BLACK);							// ������
	setbkmode(TRANSPARENT);
	end_t = clock();
	total_t = (end_t - start_t);
	IMAGE image;
	loadimage(&image, _T("../resourse/start.jpg"), WIDTH, HEIGHT);
	putimage(0, 0, &image);
	settextstyle(50, 0, _T("����"));
	setlinestyle(PS_NULL);
	TCHAR str[64];
	swprintf_s(str, _T("������Ϸʱ�䣺%d��%d��"), total_t / 60000, total_t / 1000 - total_t / 60000 * 60);
	settextcolor(WHITE);							// ������
	outtextxy(20, 100, str);
	total_t = readTime() + total_t;
	writeTime(total_t);
	TCHAR str1[64];
	swprintf_s(str1, _T("�����ۼ���Ϸ%d��%d��"), total_t / 60000, total_t / 1000 - total_t / 60000 * 60);
	settextcolor(WHITE);							// ������
	outtextxy(20, 20, str1);
	settextstyle(20, 0, _T("����"));
	outtextxy(384, 550, _T("��������˳���Ϸ"));
	FlushBatchDraw();
	getchar();
	closegraph();
	exit(1);		//�����������¿�ʼ��Ϸ
}



void move(BALL* ball) {
	if (ball->r <= 0)
		ball->life = false;
	if (ball->life == false) {						// �ж���Ϸ�Ƿ����
		HWND hwnd = GetHWnd();
		MessageBox(hwnd, _T("�㱻����"), _T("��Ϸ����"), MB_ICONEXCLAMATION);
		endtime();

	}

	if (eaten + ai_eaten == AINUM)					// �Ƿ�Ե����� AI
	{
		HWND hwnd = GetHWnd();
		MessageBox(hwnd, _T("��ϲ����"), _T("��Ϸ����"), MB_OK | MB_ICONEXCLAMATION);	// ����
		endtime();
	}

	for (int i = 0; i < AINUM; i++) {				// ��ҳ� AI �ж�
		if (ball->r >= ai[i].r) {
			if (ai[i].life == 0)	continue;
			if (DISTANCE(ball->x, ball->y, ai[i].x, ai[i].y) < (4 / 5.0 * (ball->r + ai[i].r))) {
				ai[i].life = 0;					//AI����
				ball->r = sqrt(ai[i].r*ai[i].r + ball->r*ball->r);
				eaten++;
			}
		}
	}

	for (int n = 0; n < FNUM; n++) {				// ��ҳ�ʳ��
		if (food[n].eat == 0)	continue;
		if (DISTANCE(ball->x, ball->y, food[n].x, food[n].y) < ball->r) {
			ball->r += 4 / ball->r;				// �������
			food[n].eat = 0;						// ʳ�ﱻ��
		}
	}

	static int mx = 0, my = 0;						// ��¼ƫ����

	if (GetAsyncKeyState(VK_UP) && (ball->y - ball->r > 0 && ball->y <= (MAPH - ball->r + 10))) {
		ball->y -= speed;
		my += speed;
	}
	if (GetAsyncKeyState(VK_DOWN) && (ball->y - ball->r >= -10 && ball->y < (MAPH - ball->r))) {
		ball->y += speed;
		my -= speed;
	}
	if (GetAsyncKeyState(VK_LEFT) && ball->x - ball->r > 0 && (ball->x <= (MAPW - ball->r + 10))) {
		ball->x -= speed;
		mx += speed;
	}
	if (GetAsyncKeyState(VK_RIGHT) && ball->x - ball->r >= -10 && (ball->x < (MAPW - ball->r))) {
		ball->x += speed;
		mx -= speed;
	}


	if (GetAsyncKeyState(VK_SPACE)) {
		settextcolor(WHITE);
		settextstyle(32, 0, _T("����"));
		outtextxy(384 - mx, 350 - my, _T("��Ϸ����ͣ��"));
		outtextxy(20 - mx, 500 - my, _T("��ESC���˳�"));
		outtextxy(780 - mx, 500 - my, _T("���س���������"));
		FlushBatchDraw();
		getch();
		if (GetAsyncKeyState(VK_ESCAPE))
			exit(0);
		else
			getch();
	}

	setorigin(mx, my);							//��������
}


void AI() {
	for (int i = 0; i < AINUM; i++) {				// AI �����
		if (ai[i].r > mover.r) {
			if (DISTANCE(mover.x, mover.y, ai[i].x, ai[i].y) < (ai[i].r + mover.r)) {
				ai[i].r = sqrt(ai[i].r*ai[i].r + mover.r*mover.r);
				mover.life = 0;
				mover.r = 0;
			}
		}
		for (int j = 0; j < AINUM; j++) {			// AI �� AI
			if (ai[i].r > ai[j].r) {
				if (ai[j].life == 0) continue;
				if (DISTANCE(ai[i].x, ai[i].y, ai[j].x, ai[j].y) < (ai[i].r + ai[j].r)) {
					ai[i].r = sqrt(ai[i].r*ai[i].r + ai[j].r*ai[j].r);
					ai[j].life = 0;
					ai[j].r = 0;
					ai_eaten++;
				}
			}
		}

		double min_DISTANCE = 100000;
		int min = -1;
		for (int k = 0; k < AINUM; k++) {			// AI ���� AI
			if (ai[i].r > ai[k].r&&ai[k].life != 0) {
				if (DISTANCE(ai[i].x, ai[i].y, ai[k].x, ai[k].y) < min_DISTANCE) {
					min_DISTANCE = DISTANCE(ai[i].x, ai[i].y, ai[k].x, ai[k].y);
					min = k;
				}
			}
		}
		if ((min != -1) && (rand() % 2 == 1)) {
			if (rand() % 2) {
				if (ai[i].x < ai[min].x)
					ai[i].x++;
				else
					ai[i].x--;
			}
			else {
				if (ai[i].y < ai[min].y)
					ai[i].y++;
				else
					ai[i].y--;
			}
		}
		for (int n = 0; n < FNUM; n++) {			// AI ��ʳ��
			if (food[n].eat == 0) continue;
			if (DISTANCE(ai[i].x, ai[i].y, food[n].x, food[n].y) < ai[i].r) {
				ai[i].r += 4 / ai[i].r;
				food[n].eat = 0;
			}
		}
	}
}

void Food() {
	for (int i = 0; i < FNUM; i++) {				// ʳ��ˢ��
		if (food[i].eat == 0) {
			food[i].eat = 1;
			food[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
			food[i].x = rand() % MAPW;
			food[i].y = rand() % MAPH;
			food[i].type = rand() % 10 + 1;
		}
	}
}

void draw() {
	clearcliprgn();
	IMAGE image;
	loadimage(&image, _T("../resourse/background.jpg"), WIDTH*4, HEIGHT*4);
	putimage(0, 0, &image);
	setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 20);		// �ı�ʵ���ɫ��״̬
	setlinecolor(RGB(0, 100, 0));
	line(-20, MAPH + 20, -20, -20);				// ����
	line(-20, MAPH + 20, MAPW + 20, MAPH + 20);		// �Ϻ�
	line(-20, -20, MAPW + 20, -20);				// �º�
	line(MAPW + 20, -20, MAPW + 20, MAPH + 20);		// ����
	setfillcolor(GREEN);

	if (mover.x - 0.5 * WIDTH / asp < -20)
		floodfill(-20 - 11, mover.y, RGB(0, 100, 0));
	if (mover.x + 0.5 * WIDTH / asp > MAPW + 20)
		floodfill(MAPW + 20 + 11, mover.y, RGB(0, 100, 0));
	if (mover.y - 0.5 * HEIGHT / asp < -20)
		floodfill(mover.x, -20 - 11, RGB(0, 100, 0));
	if (mover.y + 0.5 * HEIGHT / asp > MAPH + 20)
		floodfill(mover.x, MAPH + 20 + 11, RGB(0, 100, 0));

	setlinecolor(WHITE);
	setlinestyle(PS_NULL);

	for (int i = 0; i < FNUM; i++) {				// ����ʳ��
		if (food[i].eat == 0) continue;
		setfillcolor(food[i].color);
		switch (food[i].type) {					// ��״
		case 1:		solidellipse(food[i].x, food[i].y, food[i].x + 2, food[i].y + 4); break;
		case 2:		solidellipse(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2);	break;
		case 3:		solidrectangle(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2); break;
		case 4:		solidrectangle(food[i].x, food[i].y, food[i].x + 2, food[i].y + 4); break;
		case 5:		solidroundrect(food[i].x, food[i].y, food[i].x + 2, food[i].y + 4, 2, 2); break;
		case 6:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 2, 2); break;
		case 7:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 4, 2); break;
		case 8:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 2, 4); break;
		case 9:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 1, 1); break;
		case 10:	fillcircle(food[i].x, food[i].y, 4); break;
		}
	}

	for (int i = 0; i < AINUM; i++) {				// �� AI
		if (ai[i].life == 0) continue;
		setfillcolor(ai[i].color);
		fillcircle(ai[i].x, ai[i].y, int(ai[i].r + 0.5));
	}

	setfillcolor(mover.color);						// �����
	fillcircle(mover.x, mover.y, int(mover.r + 0.5));

	IMAGE map(150, 100);							// С��ͼ
	SetWorkingImage(&map);
	setbkcolor(RGB(120, 165, 209));					// ǳ��ɫ����
	cleardevice();
	for (int i = 0; i < AINUM; i++)				// �� AI��С��ͼ��
	{
		if (ai[i].life == 0) continue;
		setfillcolor(ai[i].color);
		fillcircle(ai[i].x * 150 / WIDTH / 4, ai[i].y * 100 / HEIGHT / 4, int(ai[i].r / 28 + 1.5));
	}

	setfillcolor(mover.color);						// ����ң�С��ͼ��
	fillcircle(mover.x * 150 / WIDTH / 4, mover.y * 100 / HEIGHT / 4, int(mover.r / 28 + 3.5));
	setlinecolor(RGB(0, 100, 0));

	SetWorkingImage();							// �ָ���ͼ����
	putimage(mover.x + int(0.5 * WIDTH) - 150, mover.y - int(0.5 * HEIGHT), 150, 100, &map, 0, 0);						// ����С��ͼ
	setlinecolor(LIGHTBLUE);
	setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 4);
	line(mover.x + int(0.5 * WIDTH) - 151, mover.y - int(0.5 * HEIGHT), mover.x + int(0.5 * WIDTH) - 151, mover.y - int(0.5 * HEIGHT) + 99);	// ��ͼ�߿���
	line(mover.x + int(0.5 * WIDTH) - 151, mover.y - int(0.5 * HEIGHT) + 99, mover.x + int(0.5 * WIDTH), mover.y - int(0.5 * HEIGHT) + 99);	// ��ͼ�߿���

	setlinestyle(PS_NULL);							// �ָ���
	TCHAR str[32];
	swprintf_s(str, _T("����:%.1fg  ��ɱ:%d"), mover.r, eaten);
	settextcolor(WHITE);							// ������
	outtextxy(mover.x - int(0.5 * WIDTH), mover.y - int(0.5 * HEIGHT), str);
	settextcolor(WHITE);
	outtextxy(mover.x-20, mover.y , _T("user"));
}

void setall() {
	srand((unsigned)time(NULL));					// �����
	mover.color = RGB(rand() % 256, rand() % 256, rand() % 256);		// �����ɫ
	mover.life = 1;								// ����ֵ1
	mover.x = int(WIDTH*0.5);
	mover.y = int(HEIGHT*0.5);
	mover.r = 20;

	for (int i = 0; i < AINUM; i++) {				// AI ������
		ai[i].life = 1;
		ai[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		ai[i].r = float(rand() % 10 + 10);
		ai[i].x = rand() % (MAPW - int(ai[i].r + 0.5)) + int(ai[i].r + 0.5);
		ai[i].y = rand() % (MAPH - int(ai[i].r + 0.5)) + int(ai[i].r + 0.5);
	}

	for (int i = 0; i < FNUM; i++) {				// ʳ�������
		food[i].eat = 1;
		food[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		food[i].x = rand() % MAPW;
		food[i].y = rand() % MAPH;
		food[i].type = rand() % 10 + 1;
	}

	pBuffer = GetImageBuffer(NULL);					// ��ȡ�Դ�ָ��
	setbkcolor(WHITE);							// ��ɫ����
	cleardevice();								// ��ʼ������
	settextcolor(LIGHTRED);						// ������
	setbkmode(TRANSPARENT);
	settextstyle(16, 0, _T("����"));

}

void start() {
	setbkcolor(WHITE);							// ��ɫ����
	cleardevice();								// ��ʼ������
	settextcolor(BLACK);							// ������
	setbkmode(TRANSPARENT);
	IMAGE image;
	loadimage(&image, _T("../resourse/start.jpg"), WIDTH, HEIGHT);
	putimage(0, 0, &image);
	settextcolor(WHITE);
	settextstyle(22, 0, _T("����"));
	//outtextxy(580, 380, _T("��ѡ��ؿ���"));
	outtextxy(50, 550, _T("1.С��ţ��"));
	outtextxy(200, 550, _T("2.¯����"));
	outtextxy(350, 550, _T("3.�Ƿ��켫"));
	settextstyle(15, 0, _T("����"));
	outtextxy(600, 550, _T("ע�������ѡ��Ĭ��ѡ��ؿ�1;��Ϸ�а��ո��������ͣ��"));
	settextcolor(BLACK);
	settextstyle(22, 0, _T("����"));
	TCHAR s[5];
	_stprintf(s, _T("%d"), readCount());
	outtextxy(810, 200, _T("��ӭ������Ϸ��"));
	outtextxy(810, 240, _T("��Ϸ������"));
	outtextxy(910, 240, s);
}