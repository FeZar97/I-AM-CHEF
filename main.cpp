#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

const int K_HEIGHT = 7;
const int K_WIDTH = 11;

// список ингридиентов
enum INGRID{
	NONE       = 0x01,		// пустые руки / пустой стол
	DISH       = 0x02,		// тареклка
	BLBR       = 0x04,		// черничка
	ICE_C	   = 0x08,		// мороженка
	STRBR	   = 0x10,		// клубничка
	CHPD_STRBR = 0x20		// разделенная клубничка
};

// варианты клеток кухни
enum kitchenCellType {
	EMPTY		 = 0x00, // проход
	SOME_PRODUCT = 0x01, // продукт, в т.ч. ингридиент
	DISHWASH	 = 0X02, // спавн тарелок
	WINDOW		 = 0x04, // ТОЛЯН К ОКОШКУ
	BLUE_CREATE	 = 0x08, // спавн чернички
	ICE_CREATE	 = 0x10, // спавн мороженки
	STRAW_CREATE = 0x20, // спавн клубнички
	CHOP_BOARD	 = 0x40	 // доска два соска
};
// кухня размером 7х11
vector<vector<kitchenCellType>> kitchen;
// инициализация кухни
void initializeKitchen(){

	string kitchenStroke;
	kitchen.resize(K_HEIGHT);

	for(int i = 0; i < K_HEIGHT; i++){

		cin >> kitchenStroke;
		kitchen[i].resize(K_WIDTH);

		for(int j = 0; j < K_WIDTH; j++){
			switch(int(kitchenStroke[j])){
				case 46: kitchen[i][j] = EMPTY;			break; // проход
				case  0: kitchen[i][j] = SOME_PRODUCT;  break; // продукт, в т.ч. ингридиент
				case 68: kitchen[i][j] = DISHWASH;		break; // спавн тарелок
				case 87: kitchen[i][j] = WINDOW;		break; // ТОЛЯН К ОКОШКУ
				case 66: kitchen[i][j] = BLUE_CREATE;	break; // спавн чернички
				case 73: kitchen[i][j] = ICE_CREATE;	break; // спавн мороженки
				case 83: kitchen[i][j] = STRAW_CREATE;	break; // спавн клубнички
				case 67: kitchen[i][j] = CHOP_BOARD;	break; // доска два соска
			}
		}
	}

};

// координаты целевой клетки
pair<int, int> getCellCoords(kitchenCellType desiredCell){
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			if(kitchen[i][j] == desiredCell) return pair<int, int>{j, i};
	return pair<int, int>{0, 0};
};

// преобразование строки в коллекцию ингрдиентов
// ингридиенты в изделии разделены дефисами
char splitProduct2Ingrid(string productString){

	char result = 0x0;
	size_t hyphPos;

	while(!productString.empty()){

		if(productString[0] == 'N') result |= 0x01;	// пустые руки / пустой стол
		if(productString[0] == 'D') result |= 0x02;	// тареклка
		if(productString[0] == 'B') result |= 0x04;	// черничка
		if(productString[0] == 'I') result |= 0x08;	// мороженка
		if(productString[0] == 'S') result |= 0x10;	// клубничка
		if(productString[0] == 'C') result |= 0x20;	// разделенная клубничка

		hyphPos = productString.find("-");
		if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
		else break;
	}

	return result;
};

// изделие (на столе / в руках)
class product{
public:
	int x;
	int y;
	char ingridCollection;

	// считывание инпутов
	void readInput(){
		string productString;
		cin >> x >> y >> productString;
		ingridCollection = splitProduct2Ingrid(productString);
	}
};

// ингридиеты на кухне на конкретной итерации
int existingProductNb;
vector<product> existingProductVec;

// клиент = {заказ, бабки}
class customer{
public:
	char ingridCollection;
	int price;

	// считывание инпутов
	void readInput(){
		string productString;
		cin >> productString >> price;
		ingridCollection = splitProduct2Ingrid(productString);
	}
};

// клиенты на весь день
int allCustomerNb;
vector<customer> allCustomerVec;

// текущие клиенты
int curCustomerNb;
vector<customer> curCustomerVec;

// игрок
class player{
public:
	int x;
	int y;
	char ingridCollection; // что несет в руках

	void read(){
		string productString;
		cin >> x >> y >> productString;
		ingridCollection = splitProduct2Ingrid(productString);
	}

	// есть ли в коллекции такой ингридиент
	bool haveIngrid(char ingrid){
		return ingrid & ingridCollection;
	}

	// возвращает недостающую коллекцию
	char needingIngrids(char desiredProduct){
		return (desiredProduct & ingridCollection) ^ desiredProduct;
	}

} me, opponent;

// оставшееся кол-во ходов
int turnsRemaining;

// считывание инпутов
void readInput(){
	existingProductVec.clear();
	curCustomerVec.clear();

	cin >> turnsRemaining;
	me.read();
	opponent.read();

	cin >> existingProductNb;
	existingProductVec.clear();
	existingProductVec.resize(existingProductNb);
	for(auto &prod: existingProductVec) prod.readInput();

	// ignore until wood 1 league
	string ovenContents;
	int ovenTimer;
	cin >> ovenContents >> ovenTimer;

	cin >> curCustomerNb;
	curCustomerVec.resize(curCustomerNb);
	for(auto &cust: curCustomerVec) cust.readInput();
};

// изготовление заказа по рецепту
void makeCollection(char desiredCollection){

	/*
	if (me.haveItem("ICE_CREAM")) getItem("WINDOW");
	else
		if (me.haveItem("BLUEBERRIES")) getItem("ICE_CREAM");
		else
			if (me.haveItem("DISH")) getItem("BLUEBERRIES");
			else
				if (me.haveItem("NONE")) getItem("DISH");
	*/
}

// работаем
void makeSomething(){
	// делаем заказ второго клиента
	char desiredCollection = curCustomerVec.at(2).ingridCollection;
	makeCollection(desiredCollection);
};

int main()
{
	// заказы
	cin >> allCustomerNb;
	allCustomerVec.resize(allCustomerNb);
	for(auto &client : allCustomerVec) client.readInput();

	initializeKitchen();

	while(1){
		readInput();

		makeSomething();
	}
}