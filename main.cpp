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

ScenePtr scene1, scene2, scene3;
ObjectPtr start, back, randomcard, endbtn, restart, help, ban, unobtn, pressed_uno;
TimerPtr timer1 = Timer::create(1.f), timer2 = Timer::create(3.f), timer3 = Timer::create(1.f);

int comNull = 7, myNull = 7;		//각자 카드의 개수, mycard[]와 comcard[]의 첫 빈 공간 의미.
int mixCard[numOfCard];				//순서 섞은 카드 배열
int nextCard = 15;					//다음에 뒤집을 카드 넘버

bool takeCardCount = 0;				//플레이어가 카드를 냈는지 구분 -> 안 냈으면 0
bool keepCardCount = 0;				//플레이어가 카드를 가져왔는지 -> 안 가져왔으면 0
bool uno = false;

enum COL { red, green, blue, yellow };
class card {					//카드 클래스: 숫자, 색깔, 이미지 오브젝트포인터를 멤버로 가짐
public:
	int num;
	COL color;
	ObjectPtr cardObject;

	card() {};
};
card allCard[numOfCard], stdCard, myCard[14], comCard[14], randomCard[25], emptyCard;		//모든 카드 클래스 배열, 기준카드, 플레이어카드, 컴퓨터카드, 랜덤카드더미, 카드냈을때비는자리 클래스 생성

void init_game();				//게임 초기 설정
void setClassMem();				//클래스 멤버 설정함수
void random(int card[numOfCard]);		// 시작할 때 카드섞기

void play_game();
void com_play();				//컴퓨터 자동 플레이
void my_play();					//플레이어 플레이
void end_game();				//게임 종료 화면
void ban_card();

int main()
{
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

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

	setClassMem();
	back->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		play_game();
		back->hide();
		randomcard->show();
		return true;
		});

	my_play();

}
void setClassMem() {
	for (int i = 0; i < numOfCard; i++) {				//멤버 color 설정
		if (i < 10) allCard[i].color = red;
		else if (i > 9 && i < 20) allCard[i].color = yellow;
		else if (i > 19 && i < 30) allCard[i].color = green;
		else allCard[i].color = blue;
	}
	for (int i = 0; i < numOfCard; i++) {				//멤버 num 설정
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

int index_to_x(int who, int index) { //카드판 x 위치 지정 함수
	return 150 + 150 * index;
}
void play_game()
{
	random(mixCard);			//mixCard[40]에 랜덤 숫자 40개 저장, 0~6:mycard, 7~13:comcard, 14:첫번째 stdcard


	for (int i = 0; i < 7; i++)	//카드분배하기
	{									
		myCard[i] = allCard[mixCard[i]-1];		
		myCard[i].cardObject->locate(scene2, index_to_x(1, i), 60);		//**예외발생
		myCard[i].cardObject->show();								
								
		comCard[i]= allCard[mixCard[i + 7] - 1];
		comCard[i].cardObject->locate(scene2, index_to_x(0, i), 500);	//**예외발생
		comCard[i].cardObject->setImage("images/0.png");
		comCard[i].cardObject->show();									
	}

	stdCard = allCard[mixCard[14] - 1];							//첫번째 기준카드
	stdCard.cardObject->locate(scene2, 600, 270);
	stdCard.cardObject->show();

	for (int i = 0; i < 25; i++) {						//랜덤카드더미
		randomCard[i] = allCard[mixCard[i + 15] - 1];
		randomCard[i].cardObject->hide();
	}
}

void random(int card[numOfCard]) { // 첫 카드 섞을때
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 40; i++) {
		//랜덤한 수 생성
		card[i] = rand() % numOfCard + 1;
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

void my_play() {
	if (takeCardCount == 0) {		//이전에 카드를 내지 않았을 때 카드 가져오기
		randomcard->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

			if (myNull == 14) {
				showMessage("You Lose..");
				end_game();
			}

			else if (keepCardCount == 0) {		//가져온 카드가 없다면 카드 가져올수있도록
				myCard[myNull] = randomCard[nextCard - 15];

				if (myNull < 7) myCard[myNull].cardObject->locate(scene2, 150 + 150 * myNull, 60);
				else myCard[myNull].cardObject->locate(scene2, 225 + 150 * (myNull - 7), 20);
				myCard[myNull].cardObject->show();

				myNull++;
				nextCard++;

				keepCardCount = 1;
				com_play();
			}

			return true;
			});
	}

/*
	for (int i = 0; i < myNull; i++) {		//카드 내기
		myCard[i].cardObject->setOnMouseCallback([&](auto, auto, auto, auto)->bool {		//**오류부분!!!! => 교수님께 여쭤보기
			if (stdCard.num != myCard[i].num && stdCard.color != myCard[i].color) end_game();//ban_card(); =>end_game은 작동 확인용
			else {
				stdCard = myCard[i];

				for (int j = 0; j < myNull - i + 1; j++) {	//갖고 있던 카드들 배열 앞으로 땡기기
					myCard[i] = myCard[i + 1];
				}
				myCard[myNull - 1] = emptyCard;		//끝자리 비우기
				
				myNull--;
				takeCardCount = 1;

				if (myNull == 1) {		//한장 남으면 우노 외침
					uno = true;
					press_uno();
				}

				//카드 내기 완료 버튼 추가해야함
			}
			
			return true;
			});
	}
	*/
}
void com_play() {
	int T = 0;
	for (int i = 0; i < comNull; i++) {			//색과 숫자가 모두 다르면 한장 가져간다.
		if (stdCard.num != comCard[i].num && stdCard.color != comCard[i].color) T++;
	}

	if (T == comNull) {							
		if (comNull == 14) {			//14장 넘어가면 게임종료
			showMessage("You Win!!");
			end_game();
		}
		else {								
			comCard[comNull] = randomCard[nextCard - 15];

			if (comNull < 7) comCard[comNull].cardObject->locate(scene2, 150 + 150 * comNull, 500);
			else comCard[comNull].cardObject->locate(scene2, 225 + 150 * (comNull - 7), 540);

			comCard[comNull].cardObject->setImage("images/0.png");
			comCard[comNull].cardObject->show();
			showMessage("컴퓨터가 한장 가져갔습니다.");

			comNull++;
			nextCard++;							//comcard의 개수와 다음 뒤집을 카드 넘버 1씩 증가
			my_play();
		}
	}
	/*
	else {
		int count = 0;			//컴퓨터가 낸 카드의 개수
		
		for (int i = 0; i < comNull; i++) {				//색과 숫자 중 하나가 같으면
			if (stdCard.num == comCard[i].num || stdCard.color == comCard[i].color) {
				stdCard = comCard[i];		//기준카드를 해당 카드로 변경

				for (int j = 0; j < comNull - i + 1; j++) {	//갖고 있던 카드들 배열 앞으로 땡기기
					comCard[i] = comCard[i + 1];
				}

				comCard[comNull - 1].cardObject->hide();			//마지막 카드자리 숨김
				
				comNull--;								//comcard의 개수 1 감소
				count++;

				if (comNull == 1) {		//한장 남으면 우노 외침
					uno = true;
					press_uno();
				}	
			}
		}
		
		string counting = to_string(count);
		char countMessage[30];
		sprintf(countMessage, "컴퓨터가 카드를 %d장 냈습니다.", counting);

		if (comNull == 0) {
			showMessage("You Lose..");
			end_game();
		}
		else showMessage(countMessage);

		count = 0;
	}
	*/

	T = 0;

	keepCardCount = 0;
	my_play();
}

void ban_card() {
	ban = Object::create("images/ban.png", scene2, 600, 100, false);

	timer1->setOnTimerCallback([&](TimerPtr)->bool {
		timer1->set(0.1f);
		ban->hide();
		return true;
		});
	timer1->set(0.1f);
	timer1->start();
	ban->show();
}

void press_uno() {
	unobtn = Object::create("images/unobtn.png", scene2, 600, 300, false);
	pressed_uno = Object::create("images/uno.png", scene2, 600, 300, false);

	if (myNull == 1 || comNull == 1) uno = true;
	else uno = false;

	if (uno == true) {
		unobtn->show();
		timer2->set(3.f);
		timer2->start();
	}

	unobtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		unobtn->hide();
		uno == false;
		return true;
		});

	timer2->setOnTimerCallback([&](TimerPtr)->bool {
		if (uno == true) {
			myCard[myNull].cardObject->locate(scene2, 225 + 150 * (myNull - 7), 20);
			myCard[myNull].cardObject->show();

			myNull++;
			nextCard++;
		}
		else {
			timer3->set(1.f);
			timer3->start();
			pressed_uno->show();
		}
		return true;
		});

	timer3->setOnTimerCallback([&](TimerPtr)->bool {
		pressed_uno->hide();
		return true;
		});
}
void end_game() {				//게임 종료 화면
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
		start->locate(scene1, 720, 100);

		restart->hide();
		endbtn->hide();

		for (int i = 0; i < myNull; i++) {
			myCard[i].cardObject->hide();
		}
		for (int i = 0; i < comNull; i++) {
			comCard[i].cardObject->hide();
		}

		stdCard.cardObject->hide();
		randomcard->hide();

		comNull = 7;
		myNull = 7;
		nextCard = 15;

		init_game();

		return true;
		});
}
