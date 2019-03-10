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
	NONE       = 0x00,		// пустые руки / пустой стол
	DISH       = 0x01,		// тареклка
	BLBR       = 0x02,		// черничка
	ICE_C	   = 0x04,		// мороженка
	STRBR	   = 0x08,		// клубничка
	CHPD_STRBR = 0x10,		// разделенная клубничка

	// рецепты
	RECIPE1	   = DISH + ICE_C + BLBR,			  // мороженка и черничка
	RECIPE2	   = DISH + ICE_C + CHPD_STRBR,       // мороженка и клубничка
	RECIPE3    = DISH + BLBR  + CHPD_STRBR,		  // мороженка + черничка + клубничка
	RECIPE4	   = DISH + ICE_C + BLBR + CHPD_STRBR // мороженка + черничка + клубничка
};

// преобразование строки в коллекцию ингрдиентов
// ингридиенты в изделии разделены дефисами
char splitProduct2Ingrid(string productString){

	char result = 0x0;
	size_t hyphPos;

	while(!productString.empty()){

		if(productString[0] == 'N') result |= 0x00;	// пустые руки / пустой стол
		if(productString[0] == 'D') result |= 0x01;	// тареклка
		if(productString[0] == 'B') result |= 0x02;	// черничка
		if(productString[0] == 'I') result |= 0x04;	// мороженка
		if(productString[0] == 'S') result |= 0x08;	// клубничка
		if(productString[0] == 'C') result |= 0x10;	// разделенная клубничка

		hyphPos = productString.find("-");
		if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
		else break;
	}
	return result;
};

// содержится ли коллекция part в коллекции product
// если в part есть хотя бы один лишний ингридиент, то возвращается ложь
bool is1PartOf2(char part, char product){
	return ((part ^ product) & product);
};

// варианты клеток кухни
enum kitchenCellType {
	EMPTY		 = 0x00, // проход
	SOME_PRODUCT = 0x01, // продукт, в т.ч. ингридиент
	DISHWASH	 = 0X02, // спавн тарелок
	WINDOW		 = 0x04, // ТОЛЯН К ОКОШКУ
	BLBR_CREATE	 = 0x08, // спавн чернички
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
				case 68: kitchen[i][j] = DISHWASH;		break; // спавн тарелок
				case 87: kitchen[i][j] = WINDOW;		break; // ТОЛЯН К ОКОШКУ
				case 66: kitchen[i][j] = BLBR_CREATE;	break; // спавн чернички
				case 73: kitchen[i][j] = ICE_CREATE;	break; // спавн мороженки
				case 83: kitchen[i][j] = STRAW_CREATE;	break; // спавн клубнички
				case 67: kitchen[i][j] = CHOP_BOARD;	break; // доска два соска
				default: kitchen[i][j] = SOME_PRODUCT;
			}
		}
	}
};
// перевод перечисления в строку
string kitchenCell2String(kitchenCellType cellType){

	switch(cellType){
		case EMPTY:			return "EMPTY";
		case SOME_PRODUCT:	return "SOME_PRODUCT";
		case DISHWASH:		return "DISHWASH";
		case WINDOW:		return "WINDOW";
		case BLBR_CREATE:	return "BLBR_CREATE";
		case ICE_CREATE:	return "ICE_CREATE";
		case STRAW_CREATE:	return "STRAW_CREATE";
		case CHOP_BOARD:	return "CHOP_BOARD";
	}
};

// координаты целевой клетки
pair<int, int> getCellCoords(kitchenCellType desiredCell){
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			if(kitchen[i][j] == desiredCell) return pair<int, int>{j, i};
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

// проверка координат на валидность
bool isCoordsValid(pair<int, int> p){
	return p.first != -1 && p.second != -1;
};

// возвращает координаты ячейки кухни в которой есть коллекция ингридиентов ingridCollection
pair<int, int> getCellWithIngridCollection(char ingridCollection){
	
	for(auto &p: existingProductVec)
		// если ячейка стола содержит в точности то, что необходимо, юзаем ее
		if(p.ingridCollection == ingridCollection) return pair<int, int>{p.x, p.y};
	return pair<int, int>{-1, -1};
};

// координаты свободного стола, ближайшего к переданным координатам
pair<int, int> getFreeTableCoord(int x, int y){
	// проверяются 8 окружающих клеток
	for(int i = y - 1; i <= y + 1; i++)
		for(int j = x - 1; j <= x + 1; j++)
			// если это стол (возможно с каким то продуктом, оставленным ранее)
			if(kitchen[i][j] == SOME_PRODUCT){
				bool isTableFree = true;
				// поиск среди ранее оставленных ингридиентов existingProductVec ячейки i j
				// если в оставленных ингридиентах такой ячейки нет, то на нее можно что то положить
				for(auto &ingrs : existingProductVec)
					if(ingrs.x == j && ingrs.y == i) isTableFree = false;

				if(isTableFree) return pair<int, int>{j, i};
			}
};

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
		//cerr << productString << " is equal " << int(ingridCollection) << endl;
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

	char desiredCollection = 0; // рецепт, выбранный для приготовления
	bool desiredReady = true; // готовность заказа

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

	// текущие клиенты
	cin >> curCustomerNb;
	curCustomerVec.resize(curCustomerNb);
	for(auto &cust : curCustomerVec){
		cust.readInput();
		//cerr << int(cust.ingridCollection) << endl;
	}
};

// выводит координаты требуемой клетки
void use(kitchenCellType cellType){

	pair<int, int> cellCoords = getCellCoords(cellType);
	cout << "USE " << cellCoords.first << " " << cellCoords.second << endl;
	cerr << "USE " << kitchenCell2String(cellType) << endl;
};

// выводит переданные координаты
void use(pair<int, int> p){
	if(isCoordsValid(p)) cout << "USE " << p.first << " " << p.second << endl;
};

// порезанная клубничка на тарелочке
void makeChpdStrbr(){
	if(isCoordsValid(getCellWithIngridCollection(CHPD_STRBR))){

		// если в руках есть тарелка, то идем за клубничкой
		if(me.haveIngrid(DISH))				use(getCellWithIngridCollection(CHPD_STRBR));
		// иначе берем тарелку
		else								use(DISHWASH);
	} else{
		// если порезанной клубнички нет...

		// если в руках клубничка, идем ее резать
		if(me.ingridCollection == STRBR)	use(CHOP_BOARD);
		else
			// если в руках что то есть и это не клубничка, то кладем это на свободныю ячейку стола
			if(me.ingridCollection != NONE) use(getFreeTableCoord(me.x, me.y));
		// если в руках ничего нет, то берем клубничку
			else							use(STRAW_CREATE);
	}
};

// изготовление заказа по рецепту
void makeDesiredCollection(){

	//cerr << "me.desiredCollection: " << int(me.desiredCollection) << endl;
	cerr << "	my current ingrids: " << int(me.ingridCollection) << endl;

	switch(me.desiredCollection){

		// рецепт 1 = мороженое с черничкой
		case(RECIPE1):{
			cerr << "i cook ICE + BLBR" << endl;

			if(me.haveIngrid(BLBR))					use(WINDOW);
			else if(me.haveIngrid(ICE_C))			use(BLBR_CREATE);
			else if(me.haveIngrid(DISH))			use(ICE_CREATE);
			else if(me.ingridCollection == NONE)	use(DISHWASH);
			break;
		}

		// рецепт 2 = мороженое с клубничкой
		case(RECIPE2):{
			cerr << "i cook CHPD_STRBR + ICE" << endl;

			if(me.haveIngrid(ICE_C))				use(WINDOW);
			else if(me.haveIngrid(CHPD_STRBR)
				 && me.haveIngrid(DISH))			use(ICE_CREATE);

			// если есть ячейка с порезаной клубничкой
			else makeChpdStrbr();
			break;
		}

		// рецепт 3 = порезанная клубничка и черничка
		case(RECIPE3):{
			cerr << "i cook CHPD_STRBR + BLBR" << endl;

			if(me.haveIngrid(BLBR))					use(WINDOW);
			else if(me.haveIngrid(CHPD_STRBR)
				 && me.haveIngrid(DISH))			use(BLBR_CREATE);

			// если есть ячейка с порезаной клубничкой
			else makeChpdStrbr();
			break;
		}

		// рецепт 4 = мороженое с черничкой и клубничкой
		case(RECIPE4):{
			cerr << "i cook CHPD_STRBR + ICE_C + BLBR" << endl;

			if(me.haveIngrid(BLBR))					use(WINDOW);
			else if(me.haveIngrid(ICE_C))			use(BLBR_CREATE);
			else if(me.haveIngrid(CHPD_STRBR)
				 && me.haveIngrid(DISH))			use(ICE_CREATE);

			// если есть ячейка с порезаной клубничкой
			else makeChpdStrbr();
			break;
		}
		default:
			cout << "WAIT" << endl;
	}
}

// работаем
void makeSomething(){

	// 0. если только что выполнили заказ, то берем заказ у второго клиента
	if( (me.desiredCollection == 0 && me.desiredReady)
		|| // ИЛИ
			  // 1. если заказ второго клиента изменился, но me.desiredCollection является частью заказа второго клиента,
		      //    то меняем me.desiredCollection на заказ второго клиента
		(me.desiredCollection != curCustomerVec.at(1).ingridCollection && is1PartOf2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)) )
	{
		cerr << "makeSomething case 0" << endl;
		me.desiredCollection = curCustomerVec.at(1).ingridCollection;
		me.desiredReady = false;
	}else{
		// 2. если заказ второго клиента изменился, и me.desiredCollection не является частью заказа второго клиента
		//    то скидываем me.desiredCollection в dishwasher
		if(me.desiredCollection != curCustomerVec.at(1).ingridCollection && !is1PartOf2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)){
			cerr << "makeSomething case 1" << endl;
			use(DISHWASH);
			return;
		}
	}

	makeDesiredCollection();
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