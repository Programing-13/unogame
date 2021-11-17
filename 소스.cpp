#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include <stdio.h>
#include <cstdlib>
#include <ctime>

ScenePtr scene1, scene2;
ObjectPtr mycard[14];
ObjectPtr start, back, randomcard;

void random(int card[14]); // ������ �� ī�弯��
void getNewcard(int card[14]); // �����ϴ� ī�尡 �ʿ��Ҷ�

void play_game();

int main()
{
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	scene1 = Scene::create(" ", "images/start.png"); // ����ȭ��
	start = Object::create("images/startbtn.png", scene1, 600, 100); // ���۹�ư
	start->setScale(0.5f);


	scene2 = Scene::create(" ", "images/play.png");

	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene2->enter();
		return true;
		});

	back = Object::create("images/0.png", scene2, 600, 300); // ó���� ī�� �޸� ������ ī�� ������
	randomcard = Object::create("images/0.png", scene2, 1100, 300, false); // �̰� ������ ī�带 ��

	back->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		play_game();
		back->hide();
		randomcard->show();
		return true;
		});

	startGame(scene1);

	return 0;
}

int index_to_x(int who, int index) { //ī���� x ��ġ ���� �Լ�
	if (who == 1) {//�����,
		return 150 + 150 * index;
	}
	else return 200 + 100 * index; //��ǻ�͸�,
}

int index_to_y(int who, int index) { //ī���� y ��ġ ���� �Լ�
	if (who == 1) {
		return 403 - 90 * index; // �����
	}
	else return 45 + 89.5 * index; // ��ǻ�͸�
}

int myCard[14]; // ī�� ������ 14���� �Ѿ�� �й�ó��
int comCard[14]; // ī�� ������ 14���� �Ѿ�� �й�ó��
int myplay;

void play_game()
{
	random(myCard);
	char path[40];

	for (int i = 0; i < 7; i++)
	{
		printf("%d ", myCard[i]);
		myplay = myCard[i];
		sprintf(path, "Images/%d.png", myplay);
		mycard[i] = Object::create(path, scene2, index_to_x(1,i), 100);
	}

	randomcard->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		for (int i = 7; i < 14; i++)
		{
			myplay = myCard[i];
			sprintf(path, "Images/%d.png", myplay);
			mycard[i] = Object::create(path, scene2, index_to_x(1, i-7), 20);
		}
		return true;
		});
}
void random(int card[14]) { // ù ī�� ������
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 20; i++) {
		//������ �� ����
		card[i] = rand() % 40+1;
	}

}
