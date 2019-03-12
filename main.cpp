#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <ctime>

using namespace std;

string spells[73] = {"Anapneo", "Waddiwasi", "Erecto", "Glisseo", "Homenum Revelio",
					"Deletrius", "Deprimo", "Depulso", "Defodio", "Geminio",
					"Dissendio", "Diffindo", "Duro", "Obliviate", "Impedimenta",
					"Impervious", "Imperio", "Incarcerous", "Incendio", "Cave inimicum",
					"Quietus", "Colloportus", "Confundo", "Crucio", "Conjunctivitus",
					"Levicorpus", "Legilimens", "Liberacorpus", "Locomotor", "Locomotor Mortis",
					"Lumos", "Mobiliarbus", "Mobilicorpus", "Morsmordre", "Nox",
					"Muffliato", "Enervate", "Oppugno", "Orchideous", "Stupefy",
					"Petrificus Totalus", "Peskipiksi Pesternomi", "Portus", "Priori Incantatem", "Protego",
					"Protego Totalum", "Reducto", "Reducio", "Relashio", "Reparo",
					"Riddikulus", "Rictusempra", "Salvio Hexia", "Sectumsempra", "Serpensortia",
					"Silencio", "Sonorus", "Stupefy", "Tarantallegra", "Tergeo",
					"Finite Incantatem", "Fera Verto", "Ferula", "Fidelius Charm", "Flagrate",
					"Furnunculus", "Evanesco", "Excuro", "Expecto Patronum", "Expelliarmus",
					"Engorgio", "Enervate", "Episkey"};

string get_random_spell(){
	return spells[std::rand() % 73];
}

const int K_HEIGHT = 7;
const int K_WIDTH = 11;

// поле для расчета дистанции от одной ячейки до другой
short distanceField[K_HEIGHT][K_WIDTH];

// копия кухни
short **distFromMeToProducts;

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
	CHPD_DOUGH = 0x0080,		// нарезное тесто
	RAW_TART   = 0x0082,		// сырой пирог
	TART	   = 0x0100,		// готовый торт

	// рецепты WOOD 3
	RECIPE1	   = DISH + ICE_C + BLBR,			// мороженка + черничка
	RECIPE2	   = DISH + ICE_C + CHPD_STRBR,		// мороженка + клубничка
	RECIPE3    = DISH + BLBR  + CHPD_STRBR,		// черничка  + клубничка

	// рецепты WOOD 2
	RECIPE4	   = RECIPE1 + CHPD_STRBR,			// мороженка + черничка + клубничка

	// рецепты WOOD 1
	RECIPE5	   = DISH + CROISSANT + ICE_C,		// круасанн + мороженое
	RECIPE6	   = DISH + CROISSANT + BLBR,		// круасанн + черничка
	RECIPE7	   = DISH + CROISSANT + CHPD_STRBR,	// круасанн + клубничка
	RECIPE8	   = RECIPE1 + CROISSANT,			// круасанн + черничка + мороженое
	RECIPE9	   = RECIPE3 + CROISSANT,			// круасанн + клубничка + черничка
	RECIPE10   = RECIPE2 + CROISSANT,			// круасанн + клубничка + мороженка
	RECIPE11   = RECIPE4 + CROISSANT,			// круасанн + клубничка + мороженка + черничка

	// рецепты BRONZE
	RECIPE12   = DISH + TART + ICE_C,			// торт + мороженка
	RECIPE13   = DISH + TART + BLBR,			// торт + черничка
	RECIPE14   = DISH + TART + CHPD_STRBR,		// торт + клубничка
	RECIPE15   = DISH + TART + CROISSANT,		// торт + круасанн
	RECIPE16   = TART + RECIPE5,				// торт + круасанн + мороженка
	RECIPE17   = TART + RECIPE6,				// торт + круасанн + черничка
	RECIPE18   = TART + RECIPE7,				// торт + круасанн + клубничка
	RECIPE19   = TART + RECIPE8,				// торт + круасанн + черничка + мороженое
	RECIPE20   = TART + RECIPE9,				// торт + круасанн + клубничка + черничка
	RECIPE21   = TART + RECIPE10,				// торт + круасанн + клубничка + мороженка

};

// варианты клеток кухни
enum kitchenCellType{
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

// изделие (на столе / в руках)
class PRODUCT{
public:
	int x;
	int y;
	short ingridCollection;

	// считывание инпутов
	void readInput(){
		string productString;
		cin >> x >> y >> productString;
		ingridCollection = splitProduct2Ingrid(productString);
		//cerr << "on " << x << " " << y << " exist " << ingrid2String(ingridCollection) << endl;
	}

	// преобразование строки в коллекцию ингрдиентов
	static short splitProduct2Ingrid(string productString){

		short result = 0x0;
		size_t hyphPos;

		while(!productString.empty()){

			if(productString.substr(0, 2) == "NO") result |= 0x0000;	// пустые руки / пустой стол
			if(productString.substr(0, 2) == "DI") result |= 0x0001;	// тареклка
			if(productString.substr(0, 2) == "BL") result |= 0x0002;	// черничка
			if(productString.substr(0, 2) == "IC") result |= 0x0004;	// мороженка
			if(productString.substr(0, 2) == "ST") result |= 0x0008;	// клубничка
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "ST") result |= 0x0010;	// разделенная клубничка
			if(productString.substr(0, 2) == "DO") result |= 0x0020;	// тесто
			if(productString.substr(0, 2) == "CR") result |= 0x0040;	// круасанн
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "DO") result |= 0x0080;	// нарезное тесто
			if(productString.substr(0, 2) == "RA") result |= 0x0082;	// сырой пирог
			if(productString.substr(0, 2) == "TA") result |= 0x0100;	// готовый пирог

			hyphPos = productString.find("-");
			if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
			else break;
		}
		return result;
	};

	// содержится ли коллекция part в коллекции product
	// если в part есть хотя бы один лишний ингридиент, то возвращается ложь
	static bool is1PartOf2(short part, short product){
		return (!((part & product) ^ part));
	};

	// перевод перечисления в строку
	static string ingrid2String(short ingrid){

		switch(ingrid){
			case NONE:			return "NONE";
			case DISH:			return "DISH";
			case BLBR:			return "BLBR";
			case ICE_C:			return "ICE_C";
			case STRBR:			return "STRBR";
			case CHPD_STRBR:	return "CHPD_STRBR";
			case DOUGH:			return "DOUGH";
			case CROISSANT:		return "CROISSANT";
			case CHPD_DOUGH:	return "CHPD_DOUGH";
			case RAW_TART:		return "RAW_TART";
			case TART:			return "TART";
		}
	};
};

// проверка координат на валидность
bool isExistCell(pair<int, int> p){
	return p.first != -1 && p.second != -1;
};

void use(kitchenCellType);
void use(pair<int,int>);
void use(string);
void useIngrid(short);

// КУХНЯ
class KITCHEN{
public:

	// координаты спавнеров / устройств
	pair<int, int> ovenCell{-1,-1};
	pair<int, int> chopCell{-1,-1};
	pair<int, int> dishCell{-1,-1};
	pair<int, int> winCell{-1,-1};
	pair<int, int> doughCell{-1,-1};
	pair<int, int> strbrCell{-1,-1};
	pair<int, int> blbrCell{-1,-1};
	pair<int, int> iceCell{-1,-1};

	// кухня размером 7х11
	vector<vector<kitchenCellType>> cellArray;

	// инициализация кухни
	void initialize(){

		string kitchenStroke;
		cellArray.resize(K_HEIGHT);

		for(int i = 0; i < K_HEIGHT; i++){

			cin >> kitchenStroke;
			cellArray[i].resize(K_WIDTH);

			for(int j = 0; j < K_WIDTH; j++){

				// по умолчанию - непроходимое поле
				distanceField[i][j] = -1;

				switch(int(kitchenStroke[j])){
					case 48: // повар 0
					case 49: // повар 1
					case 50: // повар 2
					case 46: cellArray[i][j] = EMPTY; distanceField[i][j] = 0;		break; // . проход
					case 68: cellArray[i][j] = DISHWASH;		dishCell  = {j, i};	break; // D спавн тарелок
					case 87: cellArray[i][j] = WINDOW;			winCell	  = {j, i};	break; // W ТОЛЯН К ОКОШКУ
					case 66: cellArray[i][j] = BLBR_CREATE;		blbrCell  = {j, i};	break; // B спавн чернички
					case 73: cellArray[i][j] = ICE_CREATE;		iceCell   = {j, i};	break; // I спавн мороженки
					case 83: cellArray[i][j] = STRBR_CREATE;	strbrCell = {j, i};	break; // S спавн клубнички
					case 67: cellArray[i][j] = CHOP_BOARD;		chopCell  = {j, i};	break; // C доска два соска
					case 72: cellArray[i][j] = DOUGH_CREATE;	doughCell = {j, i};	break; // H спавн теста
					case 79: cellArray[i][j] = OVEN;			ovenCell  = {j, i};	break; // O печка
					default: cellArray[i][j] = SOME_PRODUCT;							   // продукт / пустой стол
				}

			}
		}
	};

	// перевод перечисления в строку
	string cell2String(kitchenCellType cellType){

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

	// ингридиеты на кухне на конкретной итерации
	int existingProductNb;
	vector<PRODUCT> existingProductVec;

	// поиск оборудования
	pair<int, int> getCellWithAppliance(kitchenCellType desiredCell){
		for(int i = 0; i < K_HEIGHT; i++)
			for(int j = 0; j < K_WIDTH; j++)
				if(cellArray[i][j] == desiredCell) return pair<int, int>{j, i};
		return pair<int, int>{-1, -1};
	};

	// не строгий поиск ингридиентов
	pair<int, int> getCellWithIngrid(short ingrid){
		for(auto &p : existingProductVec)
			// если ячейка стола содержит ingrid (возможно что то еще кроме ingrid)
			if(p.ingridCollection & ingrid) return pair<int, int>{p.x, p.y};
		return pair<int, int>{-1, -1};
	};

	// строгий поиск ингридиентов
	pair<int, int> getCellWithIngridStrongly(short ingrid){
		// если ячейка стола содержит в точности то, что необходимо и ничего больше
		for(auto &p: existingProductVec)
			if(p.ingridCollection == ingrid)
			{
				//cerr << "CELL " << p.x << " " << p.y << " contains " << PRODUCT::ingrid2String(ingrid) << endl;
				return pair<int, int>{p.x, p.y};
			}
		return pair<int, int>{-1, -1};
	};
	
	// координаты свободного стола, ближайшего к переданным координатам
	pair<int, int> getNearFreeTable(int x, int y){

		// проверяются 8 окружающих клеток
		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				// если это стол (возможно с каким то продуктом, оставленным ранее)
				if(cellArray[i][j] == SOME_PRODUCT){

					bool isTableFree = true;
					// поиск среди ранее оставленных ингридиентов existingProductVec ячейки i j
					// если в оставленных ингридиентах такой ячейки нет, то на нее можно что то положить
					for(auto &ingrs: existingProductVec)
						if(ingrs.x == j && ingrs.y == i) isTableFree = false;

					if(isTableFree) return pair<int, int>{j, i};
				}
		return pair<int, int>{-1, -1};
	};

	// если на кухне есть все ингридиенты, возвращает true,
	// если хотя бы одного ингридиента на кухне нет, то возвращает false
	// проверки требуют только нарезанные клубнички, круасанны и торты
	bool existAllHardIngrids(short order, short ingridsInHands){

		// если (по заказу нужна CHPD_STRBR) а (на кухне ее нет) и (в руках ее нет НА ТАРЕЛКЕ)
		if( (order & CHPD_STRBR) && (!isExistCell(getCellWithIngridStrongly(CHPD_STRBR))) && ( !((ingridsInHands & CHPD_STRBR) && (ingridsInHands & DISH))) )
		{
			cerr << "NOT EXIST IN KITCHEN " + PRODUCT::ingrid2String(CHPD_STRBR) << endl;
			return false;
		}

		// если по заказу нужен CROISSANT а на кухне его нет и в руках его нет НА ТАРЕЛКЕ
		if((order & CROISSANT) && (!isExistCell(getCellWithIngridStrongly(CROISSANT))) && ( !((ingridsInHands & CROISSANT) && (ingridsInHands & DISH))) )
		{
			cerr << "NOT EXIST IN KITCHEN " + PRODUCT::ingrid2String(CROISSANT) << endl;
			return false;
		}

		// если по заказу нужен TART а на кухне его нет и в руках его нет НА ТАРЕЛКЕ
		if((order & TART) && (!isExistCell(getCellWithIngridStrongly(TART))) && ( !((ingridsInHands & TART) && (ingridsInHands & DISH))) )
		{
			cerr << "NOT EXIST IN KITCHEN " + PRODUCT::ingrid2String(TART) << endl;
			return false;
		}
		
		//cerr << "ALL HARD INGRID EXIST IN KITCHEN" << endl;
		return true;
	};
} kitchen;

// текущие клиенты и все сегодняшние клиенты
class CLIENTS{
public:
	// клиент = {заказ, бабки}
	class CLIENT{
	public:
		short ingridCollection;
		int price;

		// считывание инпутов
		void readInput(){
			string productString;
			cin >> productString >> price;
			ingridCollection = PRODUCT::splitProduct2Ingrid(productString);
		}
	};

	// клиенты на весь день
	int allClientNb;
	vector<CLIENT> allClientVec;

	// текущие клиенты
	int curClientNb;
	vector<CLIENT> curClientVec;
} clients;

// расчет расстояние от точки startCoords до всех остальных проходных клеток
void calcDistances(short **distField, pair<int, int> startCoords){
	
	// nonChekedCellsNb - количество непроверенных проходных ячеек
	int i, j, nonChekedCellsNb = 0, k, l;

	// подсчет количества ячеек, по которым можно ходить
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			if(kitchen.cellArray[i][j] == EMPTY)
				nonChekedCellsNb++;

	// предварительно сбрасываем все ячейки в значение -1
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			distField[i][j] = -1;

	// ячейка startCoords - проходная, расстояние до нее == 0
	distField[startCoords.second][startCoords.first] = 0;
	nonChekedCellsNb--;

	// пока на поле есть проходные ячейки с неизвестной дистанцией
	while(nonChekedCellsNb){
		for(i = 0; i < K_HEIGHT; i++)
			for(j = 0; j < K_WIDTH; j++)
				// если находим проходную ячейку, для которой известно расстояние до нее, обновляем значения всех смежных проходных ячеек
				if(distField[i][j] != -1 && kitchen.cellArray[i][j] == EMPTY){
					for(k = i - 1; k <= i + 1; k++) // по высоте
						for(l = j - 1; l <= j + 1; l++){ // по ширине
							// если ячейка валидная
							if(k >= 0 && k < K_HEIGHT && l >= 0 && l < K_WIDTH){
								// если она еще не была проверена и она являеся проходом на кухне
								if(distField[k][l] == -1 && kitchen.cellArray[k][l] == EMPTY){
									distField[k][l] = distField[i][j] + 1;
									nonChekedCellsNb--;
								}
							}
						}
				}
	}

	cerr << "MAP:" << endl;
	for(i = 0; i < K_HEIGHT; i++){
		for(j = 0; j < K_WIDTH; j++)
			cerr << distField[i][j] << " ";
		cerr << endl;
	}
};

// шеф повар
class CHEF{
public:
	int x;
	int y;
	short ingridsInHands; // что несет в руках

	short desiredCollection = 0; // рецепт, выбранный для приготовления
	bool desiredReady = true; // готовность заказа

	void read(){
		string productString;
		cin >> x >> y >> productString;
		ingridsInHands = PRODUCT::splitProduct2Ingrid(productString);
	}

	// есть ли в руках такой ингридиент
	bool haveIngridInHands(short ingrid){
		return ingrid & ingridsInHands;
	}

	// есть ли рядом печка
	bool isOvenNear(){
		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				if(kitchen.cellArray[i][j] == OVEN)
					return true;
		return false;
	}

	// возвращает true, если десерт в руках != целевому рецепту
	bool needSomething(){
		return !(desiredCollection == ingridsInHands);
	}

	// если именно этот ингридиент есть в руках - 1
	// если хотя бы одного бита не хватает - 0
	bool takesAlreadyToHands(short ingrid){
		return !((ingridsInHands & ingrid) ^ ingrid);
	}

	// поиск ближайшего ингридиента
	void findNearest(short ingrid){

		short minDist = 777,
			  minIngridIdx = -1;


		pair<int, int> ingridCoords = kitchen.getCellWithIngridStrongly(ingrid);

		// если такой ингридиент вообще существует на кухне
		if(isExistCell(ingridCoords)){

			// если такой ингридиент есть в списке существующих на кухне ингридиентов, то надо выбрать самый ближайший
			for(int i = 0; i < kitchen.existingProductNb; i++){

				// ищем в векторе существующих продуктов требуемый ингридиент
				if(kitchen.existingProductVec[i].ingridCollection == ingrid){

					use(pair<int, int>{kitchen.existingProductVec[i].x, kitchen.existingProductVec[i].y});
					/*
					if(distFromMeToProducts[kitchen.existingProductVec[i]][ingridCoords.first] < minDist){
						minIngridIdx = ingrid;
						minDist = distFromMeToProducts[ingridCoords.second][ingridCoords.first];
					}
					*/
				}
			}
		}
		// если нужный ингридиент не существует среди временных продуктов, то это спавнер (тарелок/чернички/мороженки/клубнички/теста)
		else
		{
			switch(ingrid){
				case STRBR: use(STRBR_CREATE); break;
				case BLBR:	use(BLBR_CREATE); break;
				case ICE_C: use(ICE_CREATE); break;
				case DISH:	use(DISHWASH); break;
				case DOUGH:	use(DOUGH_CREATE); break;
				default: use("WAIT; !!!!!!!");
			}
		}

	}

	// поиск на кухне ингридиента, ближайшего к повару и который еще нужен по рецепту
	void takeNeededIngrid(){

		// начинаем с тарелки
		short someIngrid = 1;

		// проверются все варианты ингридиентов, участвующих в рецепте desiredCollection
		// пока someIngrid <= desiredCollection, someIngrid может быть составной частью desiredCollection
		while(someIngrid <= desiredCollection){

			// нужен ли нам вообще этот ингридиент?
			// нужен, если он есть в заказе и его нет в руках
			if((someIngrid & desiredCollection) && !(someIngrid & ingridsInHands)){

				//findNearest(someIngrid);
				useIngrid(someIngrid);
				break;
			}
			// если этот ингридиент не интересен, переходим к следующему
			else
			{
				someIngrid <<= 1;
			}
		}
	};
} me, opponent;

void recalcDistances(){
	//memcpy(distanceField, tempDistanceField, K_HEIGHT * K_WIDTH * sizeof(short));

	// мои координаты - относительно них рассчитывается расстояние до остальных ячеек кухни
	pair<int, int> myCoords{me.x, me.y};

	// расчет расстояний от моего повара до всех свободных ячеек
	calcDistances(distFromMeToProducts, myCoords);

	// для всех существующих ингридиентов определяются расстояни от повара до этого ингридиента
	// расстояние определяется минимальным неотрицательным значением ближайшей проходной ячейки
	for(int p = 0; p < kitchen.existingProductNb; p++){

		short equalMinDist = 777;

		// проверяем окружающие 8 ячеек поля и ищем минимальную, но != -1 ячейку
		for(int i = kitchen.existingProductVec[p].y - 1; i <= kitchen.existingProductVec[p].y + 1; i++)
			for(int j = kitchen.existingProductVec[p].x - 1; j <= kitchen.existingProductVec[p].x + 1; j++)
				// если проверяемые координаты валидны
				if(isExistCell(pair<int, int>{j, i})){
					// если есть проходная ячейка, с расстоянием меньше минимального, запоминаем его
					if(distFromMeToProducts[i][j] < equalMinDist && distFromMeToProducts[i][j] != -1)
						equalMinDist = distFromMeToProducts[i][j];
				}

		// после проверки всех окружающих ячеек сохраняем equalMinDist в tempDistanceField, по координатам, соответсвующим этому ингридиенту
		distFromMeToProducts[kitchen.existingProductVec[p].y][kitchen.existingProductVec[p].x] = equalMinDist;
	}
}

// оставшееся кол-во ходов
int turnsRemaining;
// ход, на котором содержимое в печке приготовится
// если == -1, то печь свободна
// иначе - печь сейчас занята и знанчение ovenReadyTurn соответсвует ходу, на котором можно извлечь содержимое
int ovenReadyTurn = -1;

// считывание инпутов
void readInput(){
	kitchen.existingProductVec.clear();
	clients.curClientVec.clear();

	cin >> turnsRemaining;
	me.read();
	opponent.read();

	// существующие готовые ингридиенты на кухне
	cin >> kitchen.existingProductNb;
	kitchen.existingProductVec.clear();
	kitchen.existingProductVec.resize(kitchen.existingProductNb);
	for(auto &prod: kitchen.existingProductVec) prod.readInput();

	// ignore until wood 1 league
	string ovenContents;
	int ovenTimer;
	cin >> ovenContents >> ovenTimer;

	// текущие клиенты
	cin >> clients.curClientNb;
	clients.curClientVec.resize(clients.curClientNb);
	for(auto &cust : clients.curClientVec)
		cust.readInput();

	//recalcDistances();
};

// использование нужного приспособления
void use(kitchenCellType cellType){

	pair<int, int> cellWithAppliance = kitchen.getCellWithAppliance(cellType);
	if(isExistCell(cellWithAppliance)){
		cout << "USE " << cellWithAppliance.first << " " << cellWithAppliance.second << "; " + get_random_spell() << endl;
		cerr << "USE " << kitchen.cell2String(cellType) << endl;
	}
};

// использование нужного продукта
void use(pair<int, int> p){
	if(isExistCell(p)){
		cout << "USE " << p.first << " " << p.second << + "; " + get_random_spell() << endl;
		cerr << "USE PAIR " << p.first << " " << p.second << endl;
	}
};

// взять нужный ингридиент
void useIngrid(short ingrid){

	switch(ingrid){

		cerr << "NEED TAKE " + PRODUCT::ingrid2String(ingrid) << endl;

		case DISH:{
			// если в руках что то есть - выкладываем
			if(me.ingridsInHands != NONE) use(kitchen.getNearFreeTable(me.x, me.y));
			// если есть пустая тарелка, оставленная до этого не на мойке
			else if(isExistCell(kitchen.getCellWithIngridStrongly(DISH))) use(kitchen.getCellWithIngridStrongly(DISH));
				 // если нет, то берем из мойки
				 else use(DISHWASH);
			break;
		}
		case BLBR:			use(BLBR_CREATE); break;
		case ICE_C:			use(ICE_CREATE); break;
		case STRBR:			use(STRBR_CREATE); break;
		case CHPD_STRBR:	use(kitchen.getCellWithIngridStrongly(CHPD_STRBR)); break;
		case DOUGH:			use(DOUGH_CREATE); break;
		case CROISSANT:		use(kitchen.getCellWithIngridStrongly(CROISSANT)); break;
		case CHPD_DOUGH:	use(kitchen.getCellWithIngridStrongly(CHPD_DOUGH)); break;
		case RAW_TART:		use(kitchen.getCellWithIngridStrongly(RAW_TART)); break;
		case TART:			use(kitchen.getCellWithIngridStrongly(TART)); break;
	}
};

void use(string command){
	cout << command + "; " + get_random_spell() << endl;
	cerr << command << endl;
};

// создание на кухне порезанной клубнички
void prepairChpdStrbr(){
	pair<int, int> nearFreeCell = kitchen.getNearFreeTable(me.x, me.y);

	// если руки пустые, берем цельную клубничку
	if(me.ingridsInHands == NONE) use(STRBR_CREATE);
	// если в руках что то есть
	else
		// если в руках цельная клубничка, режем ее
		if(me.ingridsInHands == STRBR) use(CHOP_BOARD);
		// если в руках не цельная клубничка, то ставим это на стол
		else use(nearFreeCell);
};

// создание на кухне круасанна
void prepairCroissant(){
	pair<int, int> nearFreeCell = kitchen.getNearFreeTable(me.x, me.y);

	// если печка занята
	if(ovenReadyTurn != -1){

		// если круасанн готов - забираем
		if(turnsRemaining < ovenReadyTurn){
			use(OVEN);
			// если печка рядом, то сможем забрать выпечку и печка становится свободной
			if(me.isOvenNear())
				ovenReadyTurn = -1;
		}
		// если ждем приготовления теста - пропускаем ход
		else
		{
			// если мы не у печки, то идем к печке
			if(!me.isOvenNear()) use(OVEN);
			// если у печки - ждем
			else use("WAIT");
		}
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
					ovenReadyTurn = turnsRemaining - 11;
			}
		// если в руках не тесто, то ставим это на стол
			else use(nearFreeCell);
	}
};

// создание на кухне торта
void prepairTart(){
	pair<int, int> nearFreeCell = kitchen.getNearFreeTable(me.x, me.y);

	//cerr << "PREPAIRING TART..." << endl;

	// если в руках торт, кладем его на стол
	if(me.ingridsInHands == TART)
		use(nearFreeCell);
	else
	{
		// если печка занята
		if(ovenReadyTurn != -1)
		{
			// если торт готов - забираем
			if(turnsRemaining < ovenReadyTurn){
				use(OVEN);
				// если печка рядом, то сможем забрать выпечку и печка становится свободной
				if(me.isOvenNear()) ovenReadyTurn = -1;
			}
			// если ждем приготовления торта - пропускаем ход
			else
			{
				// если мы не у печки, то идем к печке
				if(!me.isOvenNear()) use(OVEN);
				else use("WAIT");
			}
		}
		// если печка свободна
		else
		{
			switch(me.ingridsInHands){
				// если в руках сырое тесто, кладем его в печку
				case RAW_TART:
					use(OVEN);
					// если находимся у печки и можем в нее положить тесто
					if(me.isOvenNear())
						ovenReadyTurn = turnsRemaining - 10;
					break;
				// если в руках резаное тесто, добавляем черничку
				case CHPD_DOUGH: use(BLBR_CREATE); break;
				// если в руках тесто, режем его
				case DOUGH: use(CHOP_BOARD); break;
				// если руки пустые - берем тесто
				case NONE: use(DOUGH_CREATE); break;
				// если в руках не сырой пирог и не тесто, то ставим это на стол
				default: use(nearFreeCell);
			}
		}
	}
};

// все ингридиенты есть на кухе, надо их собрать
void collectDesired(){

	//cerr << "collectDesired" << endl;

	// если мне еще нужен какой то ингридиент, ищу его на кухне
	if(me.needSomething()) me.takeNeededIngrid();
	// иначе отношу заказ клиенту
	else use(WINDOW);
};

// нет каких то ингридиентов из заказа, надо сделать то, чего нет
void prepairHardIngrids(){
	
	//cerr << "prepairHardIngrids" << endl;

	// если по заказу нужна CHPD_STRBR а на кухне ее нет
	if( (me.desiredCollection & CHPD_STRBR) && (!isExistCell(kitchen.getCellWithIngridStrongly(CHPD_STRBR))) )
	{
		//cerr << "PREPAIR " + PRODUCT::ingrid2String(CHPD_STRBR) << endl;
		prepairChpdStrbr();
		return;
	}

	// если по заказу нужен CROISSANT а на кухне его нет
	if((me.desiredCollection & CROISSANT) && (!isExistCell(kitchen.getCellWithIngridStrongly(CROISSANT))))
	{
		//cerr << "PREPAIR " + PRODUCT::ingrid2String(CROISSANT) << endl;
		prepairCroissant();
		return;
	}

	// если по заказу нужен TART а на кухне его нет
	if((me.desiredCollection & TART) && (!isExistCell(kitchen.getCellWithIngridStrongly(TART))))
	{
		//cerr << "PREPAIR " + PRODUCT::ingrid2String(TART) << endl;
		prepairTart();
		return;
	}

	//cerr << "ALL HARD INGRID EXIST IN KITCHEN";
	return;
};

// изготовление заказа по рецепту
void makeDesiredCollection(){

	cerr << "desired = " << me.desiredCollection << ", inHands = " << me.ingridsInHands << endl;

	if(kitchen.existAllHardIngrids(me.desiredCollection, me.ingridsInHands))
		collectDesired();
	else
		prepairHardIngrids();
}

// работаем
void makeSomething(){

	// 0. если только что выполнили заказ, то берем заказ у второго клиента
	if( (me.ingridsInHands == 0 && me.desiredReady)
		|| // ИЛИ
			  // 1. если заказ второго клиента изменился, но me.desiredCollection является частью заказа второго клиента,
		      //    то меняем me.desiredCollection на заказ второго клиента
		(me.desiredCollection != clients.curClientVec.at(1).ingridCollection && PRODUCT::is1PartOf2(me.desiredCollection, clients.curClientVec.at(1).ingridCollection)) )
	{
		cerr << "CHANGE ORDER" << endl;
		me.desiredCollection = clients.curClientVec.at(1).ingridCollection;
		me.desiredReady = false;
	}else{
		// 2. если заказ второго клиента изменился, и me.desiredCollection не является частью заказа второго клиента
		//    то скидываем me.desiredCollection в dishwasher
		if(me.desiredCollection != clients.curClientVec.at(1).ingridCollection && !PRODUCT::is1PartOf2(me.desiredCollection, clients.curClientVec.at(1).ingridCollection)){
			cerr << "makeSomething case 1" << endl;
			cerr << "	my desiredCollection: " << int(me.desiredCollection) << ", order2 is " << int(clients.curClientVec.at(1).ingridCollection) << endl;

			// меняем заказ
			me.desiredCollection = clients.curClientVec.at(1).ingridCollection;
			me.desiredReady = false;
		}
	}

	makeDesiredCollection();
};

int main()
{
	std::srand(unsigned(std::time(0)));

	// таблица расстояний от моего повара до всех ячеек кухни
	distFromMeToProducts = new short*[K_HEIGHT];
	for(int i = 0; i < K_HEIGHT; i++)
		distFromMeToProducts[i] = new short[K_WIDTH];

	// заказы
	cin >> clients.allClientNb;
	clients.allClientVec.resize(clients.allClientNb);
	for(auto &client: clients.allClientVec) client.readInput();

	kitchen.initialize();

	while(1){
		readInput();

		makeSomething();
	}
}
