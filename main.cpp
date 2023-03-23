#include<iostream>
using namespace std;	//��ʱ��֪����ô�޸�
#include<graphics.h>
#include"tools.h"
#include<ctime>
#include<time.h>

#include<mmsystem.h>	//����
#pragma comment(lib,"winmm.lib") 



enum {WAN_DAO,XIANG_RI_KUI,SHI_REN_HUA,ZHI_WU_COUNT};

IMAGE imgBg;	//��Ϸ����ͼ
IMAGE imgBar;	//״̬������ֲ��ı�����
IMAGE imgCards[ZHI_WU_COUNT];	//ֲ�￨������
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//ֲ������ 

int curX, curY;	//��ǰѡ��ֲ�����ƶ��е�����
int curZhiWu;	//��ǰѡ�е�ֲ��	0-û��ѡ�У�1-ѡ�е�һ��ֲ��

//�궨����Ϸ���ڴ�С
#define	WIN_WIDTH 900
#define	WIN_HEIGHT 600

struct zhiWu {	//ֲ��ṹ��
	int type;		//0-û��ֲ�1-��һ��ֲ��
	int frameIndex;	//����֡�����
};

struct sunShineBall {	//������ṹ��
	int	x, y;//�������x��y����
	int	frameIndex;	//����������֡�����
	int	destY;	//������ֹͣ��y����
	bool used;	//�������Ƿ���ʹ��
	int timer;	//��ʱ����������������������ͣ��ʱ��
};

struct sunShineBall	balls[10];	//������أ��������ȴ洢����
IMAGE	imgSunShineBall[29];	//��������֡����	-	���Զ���һ���꣬������ڹ���

struct zm {		//��ʬ�ṹ��	-���ڻ�����Ҫ��ֲ��һ�����ö��,���㴴����ͬ���͵Ľ�ʬ
	int x, y;
	int frameIndex;
	bool used;
	int speed;	//��ʬǰ���ٶ�
};

struct zm zms[10];	//��ʬ��,�������ȴ洢��ʬ
IMAGE	imgZm[22];	

int ballMax = sizeof(balls) / sizeof(balls[0]);	//����������������	-�ǲ��Ǻ���֣����������Ѿ�������10��
												//���ﻹҪ����һ�飬���ʣ��ʾ��Ǹ߿���(���˽����10����һ���궨�壬���Ӹ߿���)
int zmMax = sizeof(zms) / sizeof(zms[0]);	//��ʬ���н�ʬ������

struct zhiWu map[3][9];	//��ͼ���飬����洢ֲ��

int sunShine;	//����ֵ

//������Ϸ��ʼ��
void gameInit();

//������Ϸ��ʼ����
void startUI();

//������Ⱦ��Ϸ����(��ȾͼƬ��������)
void updateWindow();

//�����û��������������
void userClick();

//�����ж��ļ��Ƿ����
bool fileExist(const char* name);

//�������¶���
void updateGame();

//������������
void createSunShine();

//�����������⶯��
void updateSunShine();

//�����ռ�����
void collectSunshine(ExMessage* msg);

int main() {
		
	gameInit();	//��Ϸ��ʼ��

	startUI();	//������Ϸ��ʼ����

	//˫���壬�Ƚ�Ҫ���Ƶ�����һ���Ի�����ͼƬ�ϣ��ٰ�ͼƬ��������ⲻ�ϴ��ڴ��ȡ���ݶ����µ���Ļ��˸
	//��Ҫ��BeginBatchDraw()��EndBatchDraw()��FlushBatchDraw()���
	BeginBatchDraw();

	int timer=0;
	bool flag = true;

	while (1)
	{
		userClick();	//��ȡ�û�����¼�

		timer += getDelay();	//��ȡ���ʱ��
		if (timer > 20) {	//��������ֲ����Ⱦʱ��
			timer = 0;
			flag = true;
		}
		if (flag) {

			flag = false;

			updateWindow();	//������Ϸ���ڣ���Ⱦ��

			updateGame();	//���¶���
		}
		//ˢ��ͼƬ���������
		FlushBatchDraw();
	}
	
	EndBatchDraw();

	system("pause");

	return 0;
}

//��Ϸ��ʼ��ʵ��
void gameInit() {
	//������Ϸ����ͼƬ
	loadimage(&imgBg, "res/bg.jpg");

	//����״̬��
	loadimage(&imgBar, "res/bar5.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));	//��ָ�븳��ֵ

	memset(map, 0, sizeof(map));	//��ʼ����ͼ����

	memset(balls, 0, sizeof(balls));	//��ʼ�������


	//����ֲ�￨��
	char	name[64];
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i+1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {	//20�ǹ̶�ֵ������Ѱ���ƥ��ķ�ʽ
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//�ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}		
	}

	//��ʼ��ѡ��ֲ��
	curZhiWu = 0;

	//��ʼ������ֵ
	sunShine = 50;

	//��������
	for (int i = 0; i < 29; i++) {	//29�ǹ̶�ֵ������Ѱ���ƥ��ķ�ʽ
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png",i+1);
		loadimage(&imgSunShineBall[i],name);
	}

	srand(time(NULL));

	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT,1/*,1*/);	//����1��ʾ�ٿ�һ������̨����	

	//��������
	LOGFONT	f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");	//��������Ч��
	f.lfQuality = ANTIALIASED_QUALITY;		//�����
	settextstyle(&f);
	setbkmode(TRANSPARENT);					//����ģʽ������͸��
	setcolor(BLACK);						//������ɫ����ɫ

}

//��Ϸ��ʼ����ʵ��
void startUI() {
	IMAGE imgMenu,imgMenu1,imgMenu2;
	int	flag = 0;
	loadimage(&imgMenu, "res/menu.png");	//���ؿ�ʼ����ͼ
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");

	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgMenu);	//��Ⱦ��ʼ����ͼ��������
		putimagePNG(474, 75, flag == 0 ? &imgMenu1 : &imgMenu2);

		ExMessage	msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN &&	//����������		��չ������꾭��ʱҲ���Ը���
				msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215) {
				flag = 1;
			}
		}
		else if (msg.message == WM_LBUTTONUP && flag == 1) {	//������̧��
			return;
		}
		EndBatchDraw();
	}
}

//��ͼƬ���ص�������(��Ⱦ)
void updateWindow() {
	putimage(0, 0, &imgBg);	//����(��Ⱦ)������
	putimagePNG(255, 0, &imgBar);	//����(��Ⱦ)״̬��
	for (int i = 0; i < ZHI_WU_COUNT; i++) {	//����(��Ⱦ)ֲ�￨��
		int x = 343 + i * 65;
		int y = 6;
		putimagePNG(x, y, &imgCards[i]);
	}

	for (int i = 0; i < 3; i++) {	//�ڵ�ͼ�ϼ���(��Ⱦ)ֲ��
		for (int j = 0; j < 9; j++){
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 179 + i * 102 + 14;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
			}
		}
	}

	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];	//����(��Ⱦ)ֲ��
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {	//����(��Ⱦ)����
			IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x, balls[i].y, img);
		}
	}

	//����(��Ⱦ)����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunShine);	//������ֵת�����ַ�����
	outtextxy(283, 67, scoreText);			//��Ⱦ���	λ�ÿɵ����ɾ���,����ʹ�ù̶�ֵy		

}

//�û����ʵ��
void userClick() {
	static	int status = 0;
	ExMessage	msg;
	if (peekmessage(&msg)) {	//�ж��û��Ƿ��в���
		if (msg.message == WM_LBUTTONDOWN) {	//����������
			if (msg.x > 343 && msg.x < 343 + 65 * ZHI_WU_COUNT && msg.y < 96) {	//������Ƶ��¼�
				int index = (msg.x - 343) / 65;
				status = 1;
				curZhiWu = index + 1;
				//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
				curX = msg.x;
				curY = msg.y;
			}
			else {	//�ռ������¼�
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {	//����ƶ�
			curX = msg.x;
			curY = msg.y;
		}else if (msg.message == WM_RBUTTONDOWN) {	//����Ҽ�����
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int	row = (msg.y - 179) / 102;	//��ȡ��
				int	col = (msg.x - 256) / 81;	//��ȡ��
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;	//����굱ǰ������ֲ��
					map[row][col].frameIndex = 0;	//��Ⱦֲ���һ֡
				}
			}
			//ʹֲ���ͷ���ʧ
			curZhiWu = 0;
			status = 0;
			//����ֲ�������
			//curX = 1000;
			//curY = 1000;
		}
	}
}

//�ж��ļ��Ƿ����ʵ��
bool fileExist(const char* name) {
	FILE* fp=fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

//���¶���ʵ��
void updateGame() {

	//����ֲ�ﶯ��
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int	zhiWuType = map[i][j].type - 1;
				int	index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
	
	//��������
	createSunShine();

	//�������⶯��
	updateSunShine();
}

//��������ʵ��
void createSunShine() {

	static	int	count = 0;
	static	int	fre = 300;
	count++;
	
	if (count >= fre) {	//�����������ɵ��ٶ�

		fre = 100 + rand() % 200;	//�ڶ������������ʱ�����
		count = 0;

		int i;

		//���������ȡ�����õ�����
		for (i = 0; i < ballMax && balls[i].used; i++);	//���ʣ��ʾ���һ���¶��巽ʽ����{}һ����
		if (i >= ballMax)return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 320);	//������
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90;	//���ֹͣλ��
	}
}

//�������⶯��ʵ��
void updateSunShine() {
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;	//��������֡
			if (balls[i].y < balls[i].destY) {
				balls[i].y += 2;
			}
			if (balls[i].y >= balls[i].destY) {
				balls[i].timer++;
				//cout << "i=" << i << ":" << balls[i].timer << endl;
				if (balls[i].timer > 100) {
					balls[i].timer = 0;	//���ö�ʱ����������һ��ȡ��ͬ����������һ����ֹͣλ�þ���ʧ
					balls[i].used = false;
					//printf((balls[i].timer));
					//cout << "i=" << i << ":" << balls[i].timer << endl;
				}
			}
		}
	}
}

//�ռ�����ʵ��
void collectSunshine(ExMessage* msg) {
	int w = imgSunShineBall[0].getwidth();	//����������Ŀ��
	int h = imgSunShineBall[0].getheight();	//����������ĸ߶�
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {	//������ʹ���˲Ž��в���
			if (msg->x > balls[i].x && msg->x<balls[i].x + w &&		//ֻ�е���������ⷶΧ�ڲŽ��в���
				msg->y>balls[i].y && msg->y < balls[i].y + h) {
				balls[i].used = false;	//��������ʧ
				sunShine += 25;			//����ֵ��25
				mciSendString("play res/sunshine.mp3", 0, 0, 0);
			}
		}
	}
}

