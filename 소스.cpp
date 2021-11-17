#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include <stdio.h>
#include <cstdlib>
#include <ctime>

ScenePtr scene1, scene2;
ObjectPtr mycard[14];
ObjectPtr start, back, randomcard;

void random(int card[14]); // 시작할 때 카드섞기
void getNewcard(int card[14]); // 게임하다 카드가 필요할때

void play_game();

int main()
{
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	scene1 = Scene::create(" ", "images/start.png"); // 시작화면
	start = Object::create("images/startbtn.png", scene1, 600, 100); // 시작버튼
	start->setScale(0.5f);


	scene2 = Scene::create(" ", "images/play.png");

	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene2->enter();
		return true;
		});

	back = Object::create("images/0.png", scene2, 600, 300); // 처음에 카드 뒷면 누르면 카드 나눠줌
	randomcard = Object::create("images/0.png", scene2, 1100, 300, false); // 이걸 누르면 카드를 줌

	back->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		play_game();
		back->hide();
		randomcard->show();
		return true;
		});

	startGame(scene1);

	return 0;
}

int index_to_x(int who, int index) { //카드판 x 위치 지정 함수
	if (who == 1) {//나라면,
		return 150 + 150 * index;
	}
	else return 200 + 100 * index; //컴퓨터면,
}

int index_to_y(int who, int index) { //카드판 y 위치 지정 함수
	if (who == 1) {
		return 403 - 90 * index; // 나라면
	}
	else return 45 + 89.5 * index; // 컴퓨터면
}

int myCard[14]; // 카드 개수가 14개가 넘어가면 패배처리
int comCard[14]; // 카드 개수가 14개가 넘어가면 패배처리
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
void random(int card[14]) { // 첫 카드 섞을때
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 20; i++) {
		//랜덤한 수 생성
		card[i] = rand() % 40+1;
	}

}
