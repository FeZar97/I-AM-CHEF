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
	NONE       = 0x0000,		// пустые руки / пустой стол
	DISH       = 0x0001,		// тареклка
	BLBR       = 0x0002,		// черничка
	ICE_C	   = 0x0004,		// мороженка
	STRBR	   = 0x0008,		// клубничка
	CHPD_STRBR = 0x0010,		// разделенная клубничка
	DOUGH	   = 0x0020,		// тесто
	CROISSANT  = 0x0040,		// круасанн

	// рецепты WOOD 3
	RECIPE1	   = DISH + ICE_C + BLBR,							// мороженка + черничка
	RECIPE2	   = DISH + ICE_C + CHPD_STRBR,						// мороженка + клубничка
	RECIPE3    = DISH + BLBR  + CHPD_STRBR,						// черничка  + клубничка

	// рецепты WOOD 2
	RECIPE4	   = DISH + ICE_C + BLBR + CHPD_STRBR,				// мороженка + черничка + клубничка

	// рецепты WOOD 1
	RECIPE5	   = DISH + CROISSANT + ICE_C,						// круасанн + мороженое
	RECIPE6	   = DISH + CROISSANT + BLBR + ICE_C,				// круасанн + черничка + мороженое
	RECIPE7	   = DISH + CROISSANT + CHPD_STRBR + BLBR,			// круасанн + клубничка + черничка
	RECIPE8	   = DISH + CROISSANT + CHPD_STRBR + ICE_C,			// круасанн + клубничка + мороженка
	RECIPE9	   = DISH + ICE_C + BLBR + CHPD_STRBR + CROISSANT	// круасанн + клубничка + мороженка + черничка
};

// преобразование строки в коллекцию ингрдиентов
// ингридиенты в изделии разделены дефисами
short splitProduct2Ingrid(string productString){

	short result = 0x0;
	size_t hyphPos;

	while(!productString.empty()){

		if(productString.substr(0, 2) == "NO") result |= 0x0000;	// пустые руки / пустой стол
		if(productString.substr(0, 2) == "DI") result |= 0x0001;	// тареклка
		if(productString.substr(0, 2) == "BL") result |= 0x0002;	// черничка
		if(productString.substr(0, 2) == "IC") result |= 0x0004;	// мороженка
		if(productString.substr(0, 2) == "ST") result |= 0x0008;	// клубничка
		if(productString.substr(0, 2) == "CH") result |= 0x0010;	// разделенная клубничка
		if(productString.substr(0, 2) == "DO") result |= 0x0020;	// тесто
		if(productString.substr(0, 2) == "CR") result |= 0x0040;	// круасанн

		hyphPos = productString.find("-");
		if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
		else break;
	}
	return result;
};

// содержится ли коллекция part в коллекции product
// если в part есть хотя бы один лишний ингридиент, то возвращается ложь
bool is1PartOf2(short part, short product){
	return ((part ^ product) & product);
};

// нужен ли ингридиент ingrid для product 
bool is1NeedFor2(short part, short product){
	return part & product;
};

// варианты клеток кухни
enum kitchenCellType {
	EMPTY		 = 0x0000, // проход
	SOME_PRODUCT = 0x0001, // продукт, в т.ч. ингридиент
	DISHWASH	 = 0X0002, // спавн тарелок
	WINDOW		 = 0x0004, // ТОЛЯН К ОКОШКУ
	BLBR_CREATE	 = 0x0008, // спавн чернички
	ICE_CREATE	 = 0x0010, // спавн мороженки
	STRBR_CREATE = 0x0020, // спавн клубнички
	CHOP_BOARD	 = 0x0040, // доска два соска
	DOUGH_CREATE = 0x0080, // спавн теста
	OVEN		 = 0x0100  // печка
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
				case 48: // повар 0
				case 49: // повар 1
				case 50: // повар 2
				case 46: kitchen[i][j] = EMPTY;			break; // . проход
				case 68: kitchen[i][j] = DISHWASH;		break; // D спавн тарелок
				case 87: kitchen[i][j] = WINDOW;		break; // W ТОЛЯН К ОКОШКУ
				case 66: kitchen[i][j] = BLBR_CREATE;	break; // B спавн чернички
				case 73: kitchen[i][j] = ICE_CREATE;	break; // I спавн мороженки
				case 83: kitchen[i][j] = STRBR_CREATE;	break; // S спавн клубнички
				case 67: kitchen[i][j] = CHOP_BOARD;	break; // C доска два соска
				case 72: kitchen[i][j] = DOUGH_CREATE;	break; // H спавн теста
				case 79: kitchen[i][j] = OVEN;			break; // O печка
				default: kitchen[i][j] = SOME_PRODUCT;		   // продукт / пустой стол
			}
		}
		cerr << endl;
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
		case STRBR_CREATE:	return "STRBR_CREATE";
		case CHOP_BOARD:	return "CHOP_BOARD";
		case DOUGH_CREATE:	return "DOUGH_CREATE";
		case OVEN:			return "OVEN";
	}
};

// координаты целевой клетки
pair<int, int> getCellWithAppliance(kitchenCellType desiredCell){
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			if(kitchen[i][j] == desiredCell) return pair<int, int>{j, i};
};

// изделие (на столе / в руках)
class product{
public:
	int x;
	int y;
	short ingridCollection;

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

// возвращает координаты ячейки кухни в которой есть ingrid (возможно с чем то еще кроме ingrid)
pair<int, int> getCellWithIngrid(short ingrid){
	for(auto &p : existingProductVec)
		// если ячейка стола содержит в точности то, что необходимо, юзаем ее
		if(p.ingridCollection & ingrid) return pair<int, int>{p.x, p.y};
	return pair<int, int>{-1, -1};
};

// если существует ингридиент, в точности переданный в функцию
bool existStrongly(short ingrid){

	for(auto &p : existingProductVec)
		// если ячейка стола содержит в точности то, что необходимо
		if(!((p.ingridCollection & ingrid) ^ ingrid))
			return true;
	return false;
};
// возвращает координаты ячейки кухни в которой есть строго ingrid
pair<int, int> getCellWithIngridStrongly(short ingrid){
	// если ячейка стола содержит в точности то, что необходимо
	for(auto &p : existingProductVec)
		if(!((p.ingridCollection & ingrid) ^ ingrid))
			return pair<int, int>{p.x, p.y};
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
	short ingridCollection;
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
	short ingridsInHands; // что несет в руках

	short desiredCollection = 0; // рецепт, выбранный для приготовления
	bool desiredReady = true; // готовность заказа

	int ovenReadyTurn = -1;

	void read(){
		string productString;
		cin >> x >> y >> productString;
		ingridsInHands = splitProduct2Ingrid(productString);
	}

	// есть ли в руках такой ингридиент
	bool haveIngridInHands(short ingrid){
		return ingrid & ingridsInHands;
	}

	// есть ли в желамеом блюде ингридиент ingrid
	bool isDesiredConsist(short ingrid){
		return desiredCollection & ingrid;
	}

	// есть ли рядом печка
	bool isOvenNear(){

		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				if(kitchen[i][j] == OVEN)
					return true;
		return false;
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

	pair<int, int> cellCoords = getCellWithAppliance(cellType);
	cout << "USE " << cellCoords.first << " " << cellCoords.second << endl;
	cerr << "USE " << kitchenCell2String(cellType) << endl;
};

// выводит переданные координаты
void use(pair<int, int> p){
	if(isCoordsValid(p)){
		cout << "USE " << p.first << " " << p.second << endl;
		cerr << "USE PAIR " << p.first << " " << p.second << endl;
	}
};

// порезанная клубничка на тарелочке
void makeChpdStrbr(){
	/*
	// если на кухне есть порезанная клубничка, а в руках нет ничего кроме тарелки/пустых рук
	if(isCoordsValid(getCellWithIngridCollection(CHPD_STRBR))
	 && me.ingridsInHands <= DISH){

		// если в руках есть тарелка, то идем за клубничкой
		if(me.haveIngridInHands(DISH))				use(getCellWithIngridCollection(CHPD_STRBR));
		// иначе берем тарелку
		else								use(DISHWASH);
	}else{
		// если порезанной клубнички нет...

		// если в руках клубничка, идем ее резать
		if(me.ingridsInHands == STRBR)	use(CHOP_BOARD);
		else
			// если в руках что то есть и это не клубничка, то кладем это на свободныю ячейку стола
			if(me.ingridsInHands != NONE) use(getFreeTableCoord(me.x, me.y));
		// если в руках ничего нет, то берем клубничку
			else							use(STRBR_CREATE);
	}
	*/
};


// круасанн + порезанная клубничка
void makeCroissantWithChpdStrbr(){
	// если круасанн существует на кухне
	if(existStrongly(CROISSANT) || me.haveIngridInHands(CROISSANT))
	{
		// если порезанная клубничка существует на кухне
		if(existStrongly(CHPD_STRBR) || me.haveIngridInHands(CHPD_STRBR))
		{
			// если у меня в руках нет порезанной клубнички
			if(!me.haveIngridInHands(CHPD_STRBR))
			{
				cerr << "i have NOT CHPD_STRBR with DISH in handle" << endl;

				if(me.ingridsInHands > DISH) use(getFreeTableCoord(me.x, me.y)); // если в руках что то кроме тарелки, то выкладываем это на стол
				else
					if(me.ingridsInHands == NONE) use(DISHWASH); // если нет тарелки берем тарелку
					else use(getCellWithIngridStrongly(CHPD_STRBR)); // если тарелка есть берем клубничку
			}
			// если клубничка уже на тарелке в руках, осталось забрать круасанн
			else
			{
				cerr << "i have CHPD_STRBR with DISH in handle" << endl;
				if(!me.haveIngridInHands(CROISSANT)) use(getCellWithIngridStrongly(CROISSANT)); // если у меня в руках нет круасана берем круасан
			}
		}
		// если порезанной клубнички нет на кухне, делаем клубничку
		else
		{
			// если руки пустые, берем цельную клубничку
			if(me.ingridsInHands == NONE) use(STRBR_CREATE);
			// если в руках что то есть
			else
			{
				// если в руках цельная клубничка, режем ее
				if(me.ingridsInHands == STRBR) use(CHOP_BOARD);
				// если в руках не цельная клубничка, то ставим это на стол
				else use(getFreeTableCoord(me.x, me.y));
			}
		}
	}
	// если круасана нет, делаем круасан
	else
	{
		// если печка занята
		if(me.ovenReadyTurn != -1){

			// если круасанн готов - забираем
			if(turnsRemaining < me.ovenReadyTurn){ use(OVEN); me.ovenReadyTurn = -1; } else{ cout << "WAIT" << endl; cerr << "WAITING FOR CROISSANT" << endl; } // если ждем приготовления теста - пропускаем ход
		}
		// если печка свободна
		else
		{
			// если руки пустые - берем тесто
			if(me.ingridsInHands == NONE) use(DOUGH_CREATE);
			// если в руках тесто, кладем его в печь и запоминаем ход, когда тесто приготовится
			else
				if(me.ingridsInHands == DOUGH)
				{
					use(OVEN);
					// если находимся у печки и можем в нее положить тесто
					if(me.isOvenNear())
						me.ovenReadyTurn = turnsRemaining - 11;
				}
			// если в руках не тесто, то ставим это на стол
				else use(getFreeTableCoord(me.x, me.y));
		}
	}
};

// изготовление заказа по рецепту
void makeDesiredCollection(){

	//cerr << "me.desiredCollection: " << int(me.desiredCollection) << endl;
	cerr << "	my current ingrids: " << int(me.ingridsInHands) << endl;

	bool canCook = true;
	// очередность: круасан - клубничка - тарелка - мороженка - черничка
	// как только нужный ингридиент оказывается на тарелке, обнуляем бит этого ингридиента в desiredCollection

	switch(me.desiredCollection){

		// рецепт 1 = мороженка + черничка
		case(RECIPE1):{
			cerr << "i cook ICE + BLBR" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(BLBR))			use(WINDOW);
			//else if(me.haveIngridInHands(ICE_C))		use(BLBR_CREATE);
			//else if(me.haveIngridInHands(DISH))		use(ICE_CREATE);
			//else if(me.ingridsInHands == NONE)		use(DISHWASH);
			break;
		}

		// рецепт 2 = мороженка + клубничка
		case(RECIPE2):{
			cerr << "i cook CHPD_STRBR + ICE" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(ICE_C))				use(WINDOW);
			//else if(me.haveIngridInHands(CHPD_STRBR)
			//	 && me.haveIngridInHands(DISH))			use(ICE_CREATE);
			//else makeChpdStrbr();
			break;
		}

		// рецепт 3 = клубничка + черничка
		case(RECIPE3):{
			cerr << "i cook CHPD_STRBR + BLBR" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(BLBR))				use(WINDOW);
			//else if(me.haveIngridInHands(CHPD_STRBR)
			//	 && me.haveIngridInHands(DISH))			use(BLBR_CREATE);
			//else makeChpdStrbr();
			break;
		}

		// рецепт 4 = мороженка + черничка + клубничка
		case(RECIPE4):{
			cerr << "i cook CHPD_STRBR + ICE_C + BLBR" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(BLBR))				use(WINDOW);
			//else if(me.haveIngridInHands(ICE_C))		use(BLBR_CREATE);
			//else if(me.haveIngridInHands(CHPD_STRBR)
			//	 && me.haveIngridInHands(DISH))			use(ICE_CREATE);
			//else makeChpdStrbr();
			break;
		}

		// рецепт 5 = круасанн + мороженое
		case(RECIPE5):{
			cerr << "i cook CROISSANT + ICE_C" << endl;
			cout << "WAIT" << endl;
			break;
		}

		// рецепт 6 = круасанн + черничка + мороженое
		case(RECIPE6):{
			cerr << "i cook CROISSANT + BLBR + ICE_C" << endl;
			cout << "WAIT" << endl;
			break;
		}

		// рецепт 7 = круасанн + клубничка + черничка
		case(RECIPE7):{
			cerr << "i cook CROISSANT + CHPD_STRBR + BLBR" << endl;

			if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR + BLBR)			use(WINDOW);
			else if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR)				use(BLBR_CREATE);
			else makeCroissantWithChpdStrbr();
			break;
		}

		// рецепт 8 = круасанн + клубничка + мороженка
		case(RECIPE8):{
			cerr << "i cook CROISSANT + CHPD_STRBR + ICE_C + " << endl;

			if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR + ICE_C)	use(WINDOW);
			else if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR)		use(ICE_CREATE);
			else makeCroissantWithChpdStrbr();
			break;
		}

		// рецепт 9 = круасанн + клубничка + мороженка + черничка
		case(RECIPE9):{
			cerr << "i cook CROISSANT + CHPD_STRBR + ICE_C + BLBR" << endl;

			if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR + ICE_C + BLBR)	use(WINDOW);
			else if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR + ICE_C)		use(BLBR_CREATE);
			else if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR)				use(ICE_CREATE);
			else makeCroissantWithChpdStrbr();
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
		cerr << "CHANGE ORDER" << endl;
		me.desiredCollection = curCustomerVec.at(1).ingridCollection;
		me.desiredReady = false;
	}else{
		// 2. если заказ второго клиента изменился, и me.desiredCollection не является частью заказа второго клиента
		//    то скидываем me.desiredCollection в dishwasher
		if(me.desiredCollection != curCustomerVec.at(1).ingridCollection && !is1PartOf2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)){
			cerr << "makeSomething case 1" << endl;
			cerr << "	my desiredCollection: " << int(me.desiredCollection) << ", order2 is " << int(curCustomerVec.at(1).ingridCollection) << endl;

			// меняем заказ
			me.desiredCollection = curCustomerVec.at(1).ingridCollection;
			me.desiredReady = false;
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
