#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
using namespace std;

#define numOfCard 40

ScenePtr scene1, scene2, scene3, scene4;
ObjectPtr start, back, randomcard, nextbtn, endbtn, restart, help, ban, unobtn, keptComCard[14];
TimerPtr banTimer, unoTimer;
SoundPtr bgm, unoeffect, cardshare, cardslide, win, lose;

ObjectPtr mycard[14], comcard[14];
ObjectPtr stdCard, randomCard[25]; //기준 카드, 랜덤카드더미
int stdnum, randomnum[25]; //기준 카드, 랜덤카드가 가리키는 클래스 배열의 넘버
int myCardnum[14], comCardnum[14]; //각자 카드가 가리키는 클래스 배열의 넘버
int comNull = 7, myNull = 7; //각자 카드의 개수, mycard[]와 comcard[]의 첫 빈 공간 의미.
int mixCard[numOfCard]; //순서 섞은 카드 배열
int nextCard = 15; //다음에 뒤집을 카드 넘버

int seledtedCardnum[20]; //클릭한 카드 넘버 저장
int takeCardCount = 0; //플레이어가 카드를 낸 횟수
int keepCardCount = 0; //플레이어가 카드를 가져온 횟수

bool tookCard = 0; //플레이어가 카드를 냈는지 -> 안 냈으면 0
bool keptCard = 0; //플레이어가 카드를 가져왔는지 -> 안 가져왔으면 0
bool turn = 0; //플레이어 턴 = 0, 컴퓨터 턴 = 1
bool uno = false;

enum COL { red, green, blue, yellow };
class card { //카드 클래스: 숫자, 색깔, 이미지 오브젝트포인터를 멤버로 가짐
public:
	int num;
	COL color;
	ObjectPtr cardObject;

	card() {};
};
card allCard[numOfCard]; //모든 카드 클래스 배열 생성

void init_game(); //게임 초기 설정
void setClassMem(); //클래스 멤버 설정함수
void random(int card[numOfCard]); //시작할 때 카드섞기

void random_card();
void play_game();
void com_play(); //컴퓨터 자동 플레이
void my_play(); //플레이어 플레이
void end_game(); //게임 종료 화면
void ban_card();
void press_uno();
void keepCard();

int main()
{
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	bgm = Sound::create("sounds/bgm.mp3");
	unoeffect = Sound::create("sounds/uno.mp3");
	cardshare = Sound::create("sounds/cardshare.mp3");
	cardslide = Sound::create("sounds/cardslide.mp3");
	win = Sound::create("sounds/win.mp3");
	lose = Sound::create("sounds/lose.mp3");
	bgm->play(true);

	scene1 = Scene::create(" ", "images/start.png"); // 시작화면
	scene2 = Scene::create(" ", "images/play.png");
	scene3 = Scene::create(" ", "images/rule.png");

	start = Object::create("images/startbtn.png", scene1, 720, 100); // 시작버튼
	help = Object::create("images/helpbtn.png", scene1, 415, 100); //설명버튼

	start->setScale(0.6f);
	help->setScale(0.65f);

	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene2->enter();
		return true;
		});

	//게임룰 설명 페이지
	help->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene3->enter();
		start->locate(scene3, 1050, 550);
		return true;
		});

	init_game();
	startGame(scene1);

	return 0;
}

void init_game() {

	back = Object::create("images/0.png", scene2, 600, 270); // 처음에 카드 뒷면 누르면 카드 나눠줌
	randomcard = Object::create("images/0.png", scene2, 1100, 270, false); // 이걸 누르면 카드를 줌
	for (int i = 0; i < 14; i++) {
		keptComCard[i] = Object::create("images/0.png", scene2, 0, 0, false);
	}

	setClassMem();

	back->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		cardshare->play();
		play_game();
		back->hide();
		randomcard->show();
		return true;
		});

}


void setClassMem() {
	for (int i = 0; i < numOfCard; i++) { //멤버 num 설정
		if (i < 10) allCard[i].color = red;
		else if (10 <= i && i < 20) allCard[i].color = yellow;
		else if (19 <= i && i < 30) allCard[i].color = green;
		else allCard[i].color = blue;
	}
	for (int i = 0; i < numOfCard; i++) { //멤버 color 설정
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
	char path[numOfCard]; //멤버 allcard 설정
	for (int i = 0; i < numOfCard; i++) {
		sprintf(path, "images/%d.png", i + 1);
		allCard[i].cardObject = Object::create(path, scene2, 0, 0, false);
		allCard[i].cardObject->setScale(0.8f);
	}
}

int index_to_x(int who, int index) { //카드판 x 위치 지정 함수
	return 150 + 150 * index;
}

void random_card()
{
	random(mixCard); //mixCard[40]에 랜덤 숫자 40개 저장, 0~6:mycard, 7~13:comcard, 14:첫번째 stdcard

	for (int i = 0; i < 40; i++) {
		printf("mixcard[%d] = %d ", i, mixCard[i]);
	}
	printf("\n");
	for (int i = 0; i < 7; i++)
	{
		myCardnum[i] = mixCard[i]; //mixCard의 i번째 랜덤숫자를 myCardnum[i]에 대입
		mycard[i] = allCard[myCardnum[i]].cardObject; //myplay번째 카드객체의 Object를 mycard배열 i번째에 저장
		mycard[i]->locate(scene2, index_to_x(1, i), 140); //저장한 객체멤버의 위치 조정
		mycard[i]->show(); //객체멤버 보이기

		comCardnum[i] = mixCard[i + 7]; //mixCard의 i번째 랜덤숫자를 comCardnum[i]에 대입

		keptComCard[i]->locate(scene2, index_to_x(0, i), 500);
		keptComCard[i]->show();

	}

	stdnum = mixCard[14]; //첫번째 기준카드
	stdCard = allCard[stdnum].cardObject;
	stdCard->locate(scene2, 600, 310);
	stdCard->show();
	printf("stdnum = %d ", stdnum);
	printf("\n");
	for (int i = 0; i < 25; i++) { //랜덤카드더미
		randomnum[i] = mixCard[15 + i];
		randomCard[i] = allCard[randomnum[i]].cardObject;
		printf("randomnum[%d] = %d ", i, randomnum[i]);
		printf("\n");
	}
}

void random(int card[numOfCard]) { // 첫 카드 섞을때
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 40; i++) {
		//랜덤한 수 생성
		card[i] = rand() % numOfCard;
		// 예송 수정 -- 카드 7장 다 나오게
		for (int sub_i = 0; sub_i < i; sub_i++)
		{
			if (card[i] == card[sub_i])
			{
				i--;
				break;
			}
		}
	}

}

void play_game()
{

	random_card();
	showMessage("나부터 플레이합니다.");

	my_play();
	
}

void whosTurn() {
	
		if (turn == 0) my_play();
		else com_play();

}
void locateKeepCard(int num) {
	myCardnum[num] = randomnum[nextCard - 15];
	mycard[num] = allCard[myCardnum[num]].cardObject; //myplay번째 카드객체의 Object를 mycard배열 i번째에 저장

	if (num < 7)
		mycard[num]->locate(scene2, index_to_x(1, num), 140);
	else
		mycard[num]->locate(scene2, 225 + 150 * (num - 7), 20);

	mycard[num]->setScale(0.8f);
	mycard[num]->show();
}
void keepCard() {		//플레이어: 카드 가져오기
	if (takeCardCount > keepCardCount) {
		locateKeepCard(seledtedCardnum[keepCardCount]);
	}
	else {
		locateKeepCard(myNull);
	}

	keptCard = 1;
	keepCardCount++;
	myNull++;
	nextCard++;

	turn = 1;
	whosTurn();
	//		com_play();
}

void my_play() {

	nextbtn = Object::create("images/next.png", scene2, 200, 270, false);
	randomcard->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		if (myNull == 14) {
			bgm->stop();
			lose->play();
			showMessage("You Lose..");
			end_game();
		}

		else if (tookCard == 0) keepCard();	//이전에 카드를 내지 않았으면 카드 가져오기

			for (int i = 0; i < 7 + keepCardCount; i++) {
				printf("myCardnum[%d] = %d ", i, myCardnum[i]);
				printf("\n");
			}

		return true;
		});


	for (int i = 0; i < 7; i++) {
		mycard[i]->setOnMouseCallback([&, i](auto, auto, auto, auto)->bool {
			if (allCard[stdnum].num == allCard[myCardnum[i]].num || allCard[stdnum].color == allCard[myCardnum[i]].color)
			{
				stdCard->hide();
				stdnum = myCardnum[i];
				stdCard = allCard[stdnum].cardObject;   // 기준 카드로 바꾸기
				stdCard->locate(scene2, 600, 310);
				stdCard->setScale(0.8f);
				stdCard->show();
				nextbtn->show();
				printf("changed stdnum = %d", stdnum);
				printf("\n");
				myNull--;
				tookCard = 1;

				seledtedCardnum[takeCardCount] = i;	//선택한 카드의 mycard 배열 넘버를 저장 
				takeCardCount++;

				for (int i = 0; i < 7 + keepCardCount; i++) {
					printf("myCardnum[%d] = %d ", i, myCardnum[i]);
					printf("\n");
				}

				if (myNull == 1) {		//한장 남으면 우노 외침
					uno = true;
					press_uno();
				}
				else if (myNull == 0) {	//카드를 모두 내면 이김
					bgm->stop();
					win->play();
					showMessage("You Win!!");
					end_game();
				}
			}

			else ban_card();

			return true;
			});
	}
	for (int i = 7; i < keepCardCount; i++) {
		mycard[i]->setOnMouseCallback([&, i](auto, auto, auto, auto)->bool {
			if (allCard[stdnum].num == allCard[myCardnum[i]].num || allCard[stdnum].color == allCard[myCardnum[i]].color)
			{
				stdCard->hide();
				stdnum = myCardnum[i];
				stdCard = allCard[stdnum].cardObject;   // 기준 카드로 바꾸기
				stdCard->locate(scene2, 600, 310);
				stdCard->setScale(0.8f);
				stdCard->show();
				nextbtn->show();
				printf("changed stdnum = %d", stdnum);
				printf("\n");
				myNull--;
				tookCard = 1;

				seledtedCardnum[takeCardCount] = i;	//선택한 카드의 mycard 배열 넘버를 저장 
				takeCardCount++;

				for (int i = 0; i < 7 + keepCardCount; i++) {
					printf("myCardnum[%d] = %d ", i, myCardnum[i]);
					printf("\n");
				}

				if (myNull == 1) {		//한장 남으면 우노 외침
					uno = true;
					press_uno();
				}
				else if (myNull == 0) {	//카드를 모두 내면 이김
					bgm->stop();
					win->play();
					showMessage("You Win!!");
					end_game();
				}
			}

			else ban_card();

			return true;
			});
	}

	nextbtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		hideTimer();
		nextbtn->hide();

		turn = 1;
		whosTurn();
//		com_play();

		return true;
		});

}

void com_play() {
	int T = 0;
	for (int i = 0; i < comNull; i++) { //색과 숫자가 모두 다르면
		if (allCard[stdnum].num != allCard[comCardnum[i]].num && allCard[stdnum].color != allCard[comCardnum[i]].color)
			T++;
	}
	if (T == comNull) { //한장 가져간다.
		if (comNull == 14) { //14장 넘어가면 게임종료
			bgm->stop();
			win->play();

			showMessage("You Win!!");
			end_game();
		}
		else {
			cardslide->play();
			comCardnum[comNull] = randomnum[nextCard - 15];
			comcard[comNull] = allCard[comCardnum[comNull]].cardObject;	//카드 더미에서 한장 가져감

			if (comNull < 7) keptComCard[comNull]->locate(scene2, 150 + 150 * comNull, 500);
			else keptComCard[comNull]->locate(scene2, 225 + 150 * (comNull - 7), 540);
			keptComCard[comNull]->show();

			showMessage("컴퓨터가 한장 가져갔습니다.");

			comNull++;
			nextCard++; //comcard의 개수와 다음 뒤집을 카드 넘버 1씩 증가
			tookCard = 0;
			keptCard = 0;

			turn = 0;
			whosTurn();
//			my_play();
		}
	}
	else {
		int count = 0;			//컴퓨터가 낸 카드의 개수

		for (int i = 0; i < comNull; i++) { //색과 숫자 중 하나가 같으면
			if (allCard[stdnum].num == allCard[comCardnum[i]].num || allCard[stdnum].color == allCard[comCardnum[i]].color) {

				cardslide->play();
				stdCard->hide();
				stdnum = comCardnum[i];
				stdCard = allCard[stdnum].cardObject;
				stdCard->locate(scene2, 600, 310);
				stdCard->show();

				keptComCard[comNull - 1]->hide();

				comNull--;								//comcard의 개수 1 감소
				count++;

				for (int j = 0; j < comNull - i; j++) {
					comCardnum[i + j] = comCardnum[i + j + 1];
					comcard[i + j] = allCard[myCardnum[i + j]].cardObject;
				}

				if (comNull == 1) {		//한장 남으면 우노 외침
					unoeffect->play();
				}
			}
		}
		char countMessage[50];
		sprintf(countMessage, "컴퓨터가 카드를 %d장 냈습니다.", count);

		if (comNull == 0) {
			bgm->stop();
			lose->play();

			showMessage("You Lose..");
			end_game();
		}
		else showMessage(countMessage);

		count = 0;
		tookCard = 0;
		keptCard = 0;

		turn = 0;
		whosTurn();
//		my_play();
	}

	T = 0;
}
//  --  숫자나 색깔 다르면 ban이미지 1초 떴다 사라지기, 같으면 기준카드로 바꾸기
void ban_card() {
	ban = Object::create("images/ban.png", scene2, 550, 150, false);
	banTimer = Timer::create(1.f);
	ban->setScale(0.1f);
	ban->show();
	banTimer->start();
	banTimer->setOnTimerCallback([&](TimerPtr)->bool {
		ban->hide();
		return true;
		});

}

void press_uno() {
	unobtn = Object::create("images/unobtn.png", scene2, 1000, 20, false);
	unoTimer = Timer::create(3.f);
	unobtn->setScale(0.2f);
	unobtn->show();

	showTimer(unoTimer);
	unoTimer->start();		// 3초 안에 우노 버튼 눌러야 함

	unobtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		unoTimer->stop();
		hideTimer();
		unoTimer->set(3.f);
		unobtn->hide();
		unoeffect->play();
		uno = false;
		nextbtn->show();
		return true;
		});

	unoTimer->setOnTimerCallback([&](TimerPtr)->bool {
		if (uno == true) {    // 3초 지나고도 버튼이 눌리지 않은 상태
			hideTimer();
			unoTimer->set(3.f);
			unobtn->hide();
			nextbtn->hide();
			cardslide->play();

			keepCard();
		}

		return true;
		});

}

void end_game() { //게임 종료 화면
	bgm->stop();
	nextbtn->hide();

	restart = Object::create("images/restartbtn.png", scene2, 370, 350); // 재도전버튼
	endbtn = Object::create("images/endbtn.png", scene2, 720, 350); // 끝내기버튼
	restart->setScale(0.8f);
	endbtn->setScale(0.8f);

	endbtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		endGame();
		return true;
		});

	restart->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		scene1->enter();
		bgm->play();

		restart->hide();
		endbtn->hide();
		randomcard->hide();

		for (int i = 0; i < myNull + keepCardCount; i++) {
			mycard[i]->hide();
		}
		for (int i = 0; i < comNull; i++) {
			keptComCard[i]->hide();
		}
		stdCard->hide();

		comNull = 7;
		myNull = 7;
		nextCard = 15;

		back->show();

		return true;
		});
}
