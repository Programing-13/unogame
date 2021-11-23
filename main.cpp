#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include <stdio.h>
#include <cstdlib>
#include <ctime>

#define numOfCard 40

ScenePtr scene1, scene2;
ObjectPtr start, back, randomcard, end, replay, banbtn;
TimerPtr timer1 = Timer::create(1.f);

ObjectPtr mycard[14], comcard[14];
ObjectPtr stdCard, randomCard[25];					//기준 카드, 랜덤카드더미
int stdnum, randomnum[25];							//기준 카드, 랜덤카드가 가리키는 클래스 배열의 넘버
int myCardnum[14], comCardnum[14];	//각자 카드가 가리키는 클래스 배열의 넘버
int comNull = 7, myNull = 7;		//각자 카드의 개수, mycard[]와 comcard[]의 첫 빈 공간 의미.
int mixCard[numOfCard];				//순서 섞은 카드 배열
int nextCard = 15;					//다음에 뒤집을 카드 넘버

int turn = 0;						//0:플레이어 차례, 1: 컴퓨터 차례

enum COL { red, green, blue, yellow };
class card {					//카드 클래스: 숫자, 색깔, 이미지 오브젝트포인터를 멤버로 가짐
public:
	int num;
	COL color;
	ObjectPtr cardObject;

	card() {};
};
card allCard[numOfCard];		//모든 카드 클래스 배열 생성

void setClassMem();				//클래스 멤버 설정함수

void random(int card[numOfCard]);		// 시작할 때 카드섞기
void getNewcard(int card[14]);	// 게임하다 카드가 필요할때

void play_game();
void com_play();				//컴퓨터 자동 플레이

int main()
{
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	scene1 = Scene::create(" ", "images/start.png"); // 시작화면
	start = Object::create("images/startbtn.png", scene1, 600, 100); // 시작버튼
	//replay = Object::create("images/.png", scene2, , 100, false); // 재도전버튼
	//end = Object::create("images/.png", scene2, , 100, false); // 끝내기버튼
	start->setScale(0.5f);


	scene2 = Scene::create(" ", "images/play.png");

	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene2->enter();
		return true;
		});

	setClassMem();

	back = Object::create("images/0.png", scene2, 600, 270); // 처음에 카드 뒷면 누르면 카드 나눠줌
	randomcard = Object::create("images/0.png", scene2, 1100, 270, false); // 이걸 누르면 카드를 줌

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
	return 150 + 150 * index;
}

void play_game()
{
	random(mixCard);			//mixCard[40]에 랜덤 숫자 40개 저장, 0~6:mycard, 7~13:comcard, 14:첫번째 stdcard
	//char path[40];

	for (int i = 0; i < 7; i++)
	{
		myCardnum[i] = mixCard[i];							//mixCard의 i번째 랜덤숫자를 myCardnum[i]에 대입
		mycard[i] = allCard[myCardnum[i]].cardObject;		//myplay번째 카드객체의 Object를 mycard배열 i번째에 저장
		mycard[i]->locate(scene2, index_to_x(1, i), 60);	//저장한 객체멤버의 위치 조정
		mycard[i]->show();									//객체멤버 보이기

		comCardnum[i] = mixCard[i + 7];						//mixCard의 i번째 랜덤숫자를 comCardnum[i]에 대입
		comcard[i] = allCard[comCardnum[i]].cardObject;
		comcard[i]->locate(scene2, index_to_x(0, i), 500);
		comcard[i]->setImage("images/0.png");
		comcard[i]->show();
	}

	stdnum = mixCard[14];							//첫번째 기준카드
	stdCard = allCard[stdnum].cardObject;
	stdCard->locate(scene2, 600, 270);
	stdCard->show();

	for (int i = 0; i < 25; i++) {					//랜덤카드더미
		randomnum[i] = mixCard[15 + i];
		randomCard[i] = allCard[randomnum[i]].cardObject;

	}

	randomcard->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		//		if (myNull == 14) endGame();
		mycard[myNull] = randomCard[nextCard - 15];

		if (myNull < 7) mycard[myNull]->locate(scene2, 150 + 150 * myNull, 60);
		else mycard[myNull]->locate(scene2, 225 + 150 * (myNull - 7), 20);
		mycard[myNull]->show();

		myNull++;
		nextCard++;

		return true;
		});
}
void random(int card[numOfCard]) { // 첫 카드 섞을때
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 40; i++) {
		//랜덤한 수 생성
		card[i] = rand() % numOfCard + 1;
	}

}
void setClassMem() {
	for (int i = 0; i < numOfCard; i++) {				//멤버 num 설정
		if (i < 10) allCard[i].color = red;
		else if (i > 9 && i < 20) allCard[i].color = yellow;
		else if (i > 19 && i < 30) allCard[i].color = green;
		else allCard[i].color = blue;
	}
	for (int i = 0; i < numOfCard; i++) {				//멤버 color 설정
		switch (i % 10) {
		case 0:
			allCard[i].num = 0;
			break;
		case 1:
			allCard[i].num = 1;
			break;
		case 2:
			allCard[i].num = 2;
			break;
		case 3:
			allCard[i].num = 3;
			break;
		case 4:
			allCard[i].num = 4;
			break;
		case 5:
			allCard[i].num = 5;
			break;
		case 6:
			allCard[i].num = 6;
			break;
		case 7:
			allCard[i].num = 7;
			break;
		case 8:
			allCard[i].num = 8;
			break;
		case 9:
			allCard[i].num = 9;
			break;
		}
	}
	char path[numOfCard];					//멤버 allcard 설정
	for (int i = 0; i < numOfCard; i++) {
		sprintf(path, "images/%d.png", i + 1);
		allCard[i].cardObject = Object::create(path, scene2, 0, 0, false);
	}
}
void com_play() {
	int T = 0;
	for (int i = 0; i < comNull; i++) {			//색과 숫자가 모두 다르면
		allCard[stdnum].num != allCard[comCardnum[i]].num && allCard[stdnum].color != allCard[comCardnum[i]].color;
		T++;
	}
	if (T == comNull) {							//한장 가져간다.
		if (comNull == 14) endGame();			//14장 넘어가면 게임종료
		else {
			comCardnum[comNull] = mixCard[nextCard];
			comcard[comNull] = allCard[comCardnum[comNull]].cardObject;		//카드 더미에서 한장 가져감
			comcard[comNull]->locate(scene2, index_to_x(0, comNull), 500);
			comcard[comNull]->setImage("images/0.png");
			comcard[comNull]->show();

			comNull++;
			nextCard++;							//comcard의 개수와 다음 뒤집을 카드 넘버 1씩 증가
		}
	}
	T = 0;

	for (int i = 0; i < comNull; i++) {				//색과 숫자 중 하나가 같으면
		if (allCard[stdnum].num == allCard[comCardnum[i]].num || allCard[stdnum].color == allCard[comCardnum[i]].color) {
			stdnum = comCardnum[i];
			stdCard = allCard[stdnum].cardObject;	//기준카드를 해당 카드로 변경

			for (int j = 0; j < comNull - i; j++) {	//갖고 있던 카드들 배열 앞으로 땡기기
				comCardnum[i] = comCardnum[i + 1];
				comcard[i] = allCard[comCardnum[i]].cardObject;
			}

			comcard[comNull - 1]->hide();			//마지막 카드자리 숨김
			comNull--;								//comcard의 개수 1 감소

			if (comNull == 1) showMessage("UNO!!");	//한장 남으면 우노 외침
			if (comNull == 0) {
				showMessage("You Lose..");
				//replay->show();
				//end->show();
			}
		}
	}

	turn = 0;										//플레이어에게 턴을 넘김
}

void ban() {
	timer1->setOnTimerCallback([&](TimerPtr)->bool {
		banbtn->hide();
		return true;
		});

	for (int i = 0; i < myNull; i++) {
		mycard[i]->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
			if (allCard[myCardnum[i]].num != allCard[stdnum].num || allCard[myCardnum[i]].color != allCard[stdnum].color) {
				timer1->start();
				banbtn = Object::create("images/ban.png", scene2, 600, 200);
			}
			else {
				mycard[i]->locate(scene2, 600, 300);
				allCard[myCardnum[i]].num = allCard[stdnum].num;
				allCard[myCardnum[i]].color = allCard[stdnum].color;
			}
			return true;
			});
	}

}