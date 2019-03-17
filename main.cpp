#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <ctime>
#include <chrono>

#define			PREPAIRING_FLAG			false
#define			HOW_MUCH_STRWBR_PREP	2
#define			HOW_MUCH_CROIS_PREP		1
#define			HOW_MUCH_TARTS_PREP		0
#define			BLOCK_TRESHOLD			2

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

// список ингридиентов
enum INGRID{
	NONE       = 0x0000,		// пустые руки / пустой стол
	DISH       = 0x0001,		// тареклка
	BLBR       = 0x0002,		// черничка
	ICE_C	   = 0x0004,		// мороженка
	STRBR	   = 0x0008,		// клубничка
	CHPD_STRBR = 0x0010,		// разделанная клубничка
	DOUGH	   = 0x0020,		// тесто
	CROISSANT  = 0x0040,		// круасанн
	CHPD_DOUGH = 0x0080,		// нарезное тесто
	RAW_TART   = 0x0100,		// сырой пирог
	TART	   = 0x0200,		// готовый торт

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
	}

	// преобразование строки в коллекцию ингрдиентов
	static short splitProduct2Ingrid(string productString){

		short result = 0x0;
		size_t hyphPos;

		while(!productString.empty()){
			if(productString.substr(0, 2) == "NO") result |= NONE;			// пустые руки / пустой стол
			if(productString.substr(0, 2) == "DI") result |= DISH;			// тареклка
			if(productString.substr(0, 2) == "BL") result |= BLBR;			// черничка
			if(productString.substr(0, 2) == "IC") result |= ICE_C;			// мороженка
			if(productString.substr(0, 2) == "ST") result |= STRBR;			// клубничка
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "ST") result |= CHPD_STRBR;	// разделенная клубничка
			if(productString.substr(0, 2) == "DO") result |= DOUGH;			// тесто
			if(productString.substr(0, 2) == "CR") result |= CROISSANT;		// круасанн
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "DO") result |= CHPD_DOUGH;	// нарезное тесто
			if(productString.substr(0, 2) == "RA") result |= RAW_TART;		// сырой пирог
			if(productString.substr(0, 2) == "TA") result |= TART;			// готовый пирог

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
	return ( (p.first != -1) && (p.second != -1) && (p.first < K_WIDTH) && (p.second < K_HEIGHT) );
};

void use(pair<int,int>);
void moveAsTank(pair<int, int>);

int prepChpdStrw, prepCrois, prepTarts;

// КУХНЯ
class KITCHEN{
public:

	// все про печку
	short ovenContents;
	int ovenTimer;

	// координаты спавнеров / устройств
	pair<int, int> ovenCell{-1,-1};
	pair<int, int> chopCell{-1,-1};
	pair<int, int> dishCell{-1,-1};
	pair<int, int> winCell{-1,-1};
	pair<int, int> doughCell{-1,-1};
	pair<int, int> strbrCell{-1,-1};
	pair<int, int> blbrCell{-1,-1};
	pair<int, int> iceCell{-1,-1};

	// ближайшая к моему повару свободная ячейка стола
	pair<int, int> nearFreeCell = {-1, -1};

	// кухня размером 7х11
	vector<vector<kitchenCellType>> cellArray;

	short distArrayForMe[K_HEIGHT][K_WIDTH]; // массив расстояний до всех мест кухни относительно МОЕГО повара с учетом существования оппонента
	short distArrayForWithoutOpponent[K_HEIGHT][K_WIDTH]; // тот же массив растояний, но без учета существования повара напарника

	short distArrayForOpponent[K_HEIGHT][K_WIDTH]; // массив расстояний до всех мест кухни относительно повара НАПАРНИКА с учетом существования оппонента

	// инициализация кухни
	void initialize(){

		string kitchenStroke;
		cellArray.resize(K_HEIGHT);

		for(int i = 0; i < K_HEIGHT; i++){

			cin >> kitchenStroke;
			cellArray[i].resize(K_WIDTH);

			for(int j = 0; j < K_WIDTH; j++){

				switch(int(kitchenStroke[j])){
					case 48: // повар 0
					case 49: // повар 1
					case 50: // повар 2
					case 46: cellArray[i][j] = EMPTY;								break; // . проход
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
	short existingProductMap[K_HEIGHT][K_WIDTH]; // массив с готовыми продуктами в виде карты кухни

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
	
	// координаты ближайшей свободно столешницы
	// в приоритете столешницы на центральном столе
	pair<int, int> getNearFreeTable(){

		// расстояние до свободной столешницы
		// увеличивается на 1 до тех пор пока не будет найдена свободная столешница
		int distToFreeCell = 0;

		while(true){

			// проверка центральных столешниц
			for(int i = 2; i <= 4; i++){
				for(int j = 2; j <= 8; j++){
					// если это стол (возможно с каким то продуктом, оставленным ранее)
					// и расстояние до этой ячейки равно distToFreeCell
					if(existingProductMap[i][j] == NONE && distArrayForMe[i][j] == distToFreeCell && cellArray[i][j] != EMPTY){
						return pair<int, int>{j, i};
					}
				}
			}

			// проверка периметровых клеток

			// проверка горизонатльных столешниц
			for(int i = 0; i < K_WIDTH; i++){

				// проверка верхней столешницы
				if(existingProductMap[0][i] == NONE && distArrayForMe[0][i] == distToFreeCell)
					return pair<int, int>{i, 0};

				// проверка нижней столешницы
				else if(existingProductMap[6][i] == NONE && distArrayForMe[6][i] == distToFreeCell)
					return pair<int, int>{i, 6};
			}

			// проверка вертикальных столешниц
			for(int i = 0; i < K_HEIGHT; i++)
			{
				// левая
				if(existingProductMap[i][0] == NONE && distArrayForMe[i][10] == distToFreeCell)
					return pair<int, int>{0, i};

				// правая
				else if(existingProductMap[i][10] == NONE && distArrayForMe[i][10] == distToFreeCell)
					return pair<int, int>{10, i};
			}

			distToFreeCell++;
		}

		return pair<int, int>{-1, -1};
	};

	// поиск ближайшего ингридиента ingrid
	pair<int,int> findNearest(short ingrid){

		pair<int, int> ingridCoords = getCellWithIngridStrongly(ingrid);

		// если такой ингридиент вообще существует на кухне
		if(isExistCell(ingridCoords)){

			// среди всех таких продуктов надо выбрать ближайший
			// если искомый продукт возможно брать со спавна, то надо учитывать и расстояние до спавнера

			short minDist = 777, minIngridIdx = -1;

			switch(ingrid){
				case DISH:  minDist = distArrayForMe[dishCell.second][dishCell.first];   minIngridIdx = -2; break;
				case BLBR:  minDist = distArrayForMe[blbrCell.second][blbrCell.first];   minIngridIdx = -2; break;
				case STRBR: minDist = distArrayForMe[strbrCell.second][strbrCell.first]; minIngridIdx = -2; break;
				case DOUGH: minDist = distArrayForMe[doughCell.second][doughCell.first]; minIngridIdx = -2; break;
				default: minDist = 777, minIngridIdx = -1;
			}

			// если такой ингридиент есть в списке существующих на кухне ингридиентов, то надо выбрать самый ближайший
			for(int i = 0; i < existingProductNb; i++){

				// если есть такой продукт и растояние меньше минимума, то сохраняем его
				if((existingProductVec[i].ingridCollection == ingrid) && (distArrayForMe[existingProductVec[i].y][existingProductVec[i].x] < minDist))
				{
					minDist = distArrayForMe[existingProductVec[i].y][existingProductVec[i].x];
					minIngridIdx = i;
				}
			}

			// если minIngridIdx == -1, то нет ничего подходящего
			if(minIngridIdx == -1)
			{
				return pair<int, int>{-1, -1};
			}
			// если != -1, то есть интерсующий продукт
			// либо спавнер, либо отдельно лежащий продукт
			else
			{
				// если спавнер - возвращаются координаты спавнера
				if(minIngridIdx == -2)
				{
					switch(ingrid){
						case DISH:  return dishCell; break;
						case BLBR:  return blbrCell; break;
						case STRBR: return strbrCell; break;
						case DOUGH: return doughCell; break;
						default:	return pair<int, int>{-1, -1};
					}
				}
				// если не спавнер, а отдельный продукт
				else
				{
					return pair<int, int>{existingProductVec[minIngridIdx].x, existingProductVec[minIngridIdx].y};
				}
			}
		}
		// если такого ингридиента не существует
		else
		{
			// если ingrid - простой продукт, то возвращаются координаты спавнера
			// если сложный - возвращается (-1,-1)
			switch(ingrid){

				case DISH:  return dishCell; break;
				case BLBR:  return blbrCell; break;
				case STRBR: return strbrCell; break;
				case DOUGH: return doughCell; break;
				default:	return pair<int, int>{-1, -1};
			}
		}
	}
} kitchen;

// текущие клиенты и все сегодняшние клиенты
class CLIENTS{
public:
	// клиент = {заказ, бабки}
	class CLIENT{
	public:
		short ingridCollection;
		int award;

		// считывание инпутов
		void readInput(){
			string productString;
			cin >> productString >> award;
			ingridCollection = PRODUCT::splitProduct2Ingrid(productString);
		}
	};

// --------------------------------------------------------ВСЕ КЛИЕНТЫ-------------------------------------------------------------
	int allClientNb;
	vector<CLIENT> allClientVec;

	// общее количество сложных продуктов
	short allChpdStrwbrNb = 0;
	short allCroissantNb = 0;
	short allTartNb = 0;
	void calcAllHardProductNb(){
		for(auto c : allClientVec){
			if(c.ingridCollection & CHPD_STRBR) allChpdStrwbrNb++;
			if(c.ingridCollection & CROISSANT)  allCroissantNb++;
			if(c.ingridCollection & TART)		allTartNb++;
		}
		//cerr << "ALL: CHPD_STRBR(" << allChpdStrwbrNb << "), CROISSANT(" << allCroissantNb << "), TART(" << allTartNb << ")" << endl;
	}

	// считывание инпутов об общем кол-ве клиентов
	void readAllClientsInput(){

		cin >> allClientNb;
		allClientVec.resize(allClientNb);
		for(auto &client : allClientVec) client.readInput();

		calcAllHardProductNb();
	}

	// --------------------------------------------------------ТЕКУЩИЕ КЛИЕНТЫ-------------------------------------------------------------
	int curClientNb;
	vector<CLIENT> curClientVec;

	// текущее количество сложных продуктов
	short curChpdStrwbrNb = 0;
	short curCroissantNb = 0;
	short curTartNb = 0;
	void calcCurHardProductNb(){
		curChpdStrwbrNb = 0;
		curCroissantNb = 0;
		curTartNb = 0;
		for(auto c : curClientVec){
			if(c.ingridCollection & CHPD_STRBR) curChpdStrwbrNb++;
			if(c.ingridCollection & CROISSANT)  curCroissantNb++;
			if(c.ingridCollection & TART)		curTartNb++;
		}
		//cerr << "CURRENT: CHPD_STRBR(" << curChpdStrwbrNb << "), CROISSANT(" << curCroissantNb << "), TART(" << curTartNb << ")" << endl;
	}

	// считывание инпутов об текущем кол-ве клиентов
	void readCurClientsInput(){

		curClientVec.clear();

		cin >> curClientNb;
		curClientVec.resize(curClientNb);

		for(auto &client : curClientVec) client.readInput();

		calcCurHardProductNb();
	}

	// возвращает индекс текущего заказа, имеющего наибольшую стоимость
	short getCurClientIdxWithMaxAward(){

		short idxWithMaxAward = 0;

		for(int i = 1; i < 3; i++)
			if(curClientVec[i].award > curClientVec[idxWithMaxAward].award)
				idxWithMaxAward = i;
		return idxWithMaxAward;
	}

} clients;

// шеф повар
class CHEF{
public:
	int x;
	int y;
	short ingridsInHands; // что несет в руках
	short desiredCollection = -1; // рецепт, выбранный для приготовления
	short servedClientIdx = -1; // индекс клиента, чей заказ сейчас делает повар
	bool usedOven = false; // использовал ли шеф печку
	short whatNeedMake = -1;

	int howLongIStateOnPlace = 0; // количество ходов, которое мой повар стоял на месте
	pair<int, int> previousStateCoords = {-1,-1}; // координаты предыдущего стояния

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

	// поиск на кухне ингридиента, ближайшего к повару и который еще нужен по рецепту
	void takeNeededIngrid(){

		// все существующие СЛОЖНЫЕ продукты на кухне объединяются в один массив и упорядочиваютя по стоимости
		// затем выбирается самый дорогой продукт, который подходит по рецепту
		PRODUCT temp;

		// упорядочивание вектора существующих продуктов
		for(int i = 0; i < kitchen.existingProductNb - 1; i++){

			int prodIdxWithMaxAward = i;

			for(int j = i + 1; j < kitchen.existingProductNb; j++)
				if(kitchen.existingProductVec[j].ingridCollection > kitchen.existingProductVec[prodIdxWithMaxAward].ingridCollection)
					prodIdxWithMaxAward = j;

			temp = kitchen.existingProductVec[i];
			kitchen.existingProductVec[i] = kitchen.existingProductVec[prodIdxWithMaxAward];
			kitchen.existingProductVec[prodIdxWithMaxAward] = temp;
		}

		cerr << "отсортированные готовые продукты:" << endl;
		for(auto &product : kitchen.existingProductVec)
			cerr << "(" << product.x << "," << product.y << ") ";
		cerr << endl;

		// теперь вектор ресурсов отсортирован по ценности

		// перед тем как начать собирать самые ценные продукты, надо проверить, есть ли руках тарелка
		// если в руках тарелки нет, то сначала надо искать либо готовые продукты на тарелке, либо отдельные тарелки,
		// либо если нет ни 1 ни 2 то идти на спавн тарелок

		// если тарелки нет, ищем продукты с тарелками
		if(!(ingridsInHands & DISH))
		{
			cerr << "нужен продукт с тарелкой / тарелка" << endl;

			// если в руках есть какой то ингридиент без тарелки, кладем его на стол
			if(ingridsInHands != NONE)
			{
				cerr << "в руках не тарелка, а нужна тарелка - выклыдваем" << endl;
				moveAsTank(kitchen.nearFreeCell);
				return;
			}
			else
			{
				// ищем тарелки среи уже существующий на кухне продуктов
				for(auto &product : kitchen.existingProductVec)
					if(PRODUCT::is1PartOf2(product.ingridCollection, desiredCollection) && !PRODUCT::is1PartOf2(product.ingridCollection, ingridsInHands) && (product.ingridCollection & DISH))
					{
						cerr << "есть нужный продукт с тарелкой" << endl;
						moveAsTank(pair<int, int>{product.x, product.y});
						return;
					}

				cerr << "нет ничего подходящего, берем из посудомойки" << endl;
				// если не нашли в существующих, идем к посудомойке
				moveAsTank(kitchen.dishCell);
				return;
			}
		}
		// если тарелка в руках есть, то можем собирать только те продукты, которые без тарелок
		else
		{
			// надо найти ближайший продукт, нужный по рецепту

			pair<int, int> cellWithClosestProduct{-1,-1};

			// сначала проверяются продукты, уже существующие на кухне
			int minDist = 999, minProductIdx = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				// если существующий продукт является частью рецепта и его еще нет в руках
				if( (kitchen.existingProductVec[i].ingridCollection & desiredCollection) && !(kitchen.existingProductVec[i].ingridCollection & ingridsInHands) )
				{
					if(kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] < minDist){

						minDist = kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x];
						minProductIdx = i;
					}
				}
			}
			// если что то нашли - сохраняем
			if(minProductIdx != -1)
			{
				cellWithClosestProduct.first = kitchen.existingProductVec[minProductIdx].x;
				cellWithClosestProduct.second = kitchen.existingProductVec[minProductIdx].y;
			}

		// затем проверются спавнеры
			// если нужна мороженка, а в руках ее нет, и расстояние до спавнера мороженки меньше чем до готового продукта, то сохраняем 
			if(desiredCollection & ICE_C && !(ingridsInHands & ICE_C) && kitchen.distArrayForMe[kitchen.iceCell.second][kitchen.iceCell.first] < minDist)
			{
				cellWithClosestProduct = kitchen.iceCell;
				minDist = kitchen.distArrayForMe[kitchen.iceCell.second][kitchen.iceCell.first];
			}

			// если нужна черничка, а в руках ее нет,  и расстояние до спавнера чернички меньше чем до готового продукта, то сохраняем 
			if(desiredCollection & BLBR && !(ingridsInHands & BLBR) && kitchen.distArrayForMe[kitchen.blbrCell.second][kitchen.blbrCell.first] < minDist)
			{
				cellWithClosestProduct = kitchen.blbrCell;
				minDist = kitchen.distArrayForMe[kitchen.blbrCell.second][kitchen.blbrCell.first];
			}
			
			if(isExistCell(cellWithClosestProduct))
			{
				cerr << "есть продукт, являющийся часть рецепта..." << endl;
				moveAsTank(cellWithClosestProduct);
				return;
			}
		}
	};

	// изменились ли координаты стояния
	// если после предыдущего хода мои координаты изменились, возвращается true
	// иначе - false
	bool isStateCoordsChange(){

		if(x != previousStateCoords.first && y != previousStateCoords.second)
			return true;
		else
			return false;
	};

	// обновление координат после хода
	void updateCoords(){
		
		// если координаты изменились, то сбрасывается счетчик простаивания
		// обновляются предыдущие координаты стояния
		if(isStateCoordsChange()){

			howLongIStateOnPlace = 0;
			previousStateCoords = pair<int,int>{x, y};
		}
		// если координаты не измнились
		// то инкрементится счетчик простаивания
		else
		{
			howLongIStateOnPlace++;
		}
	};

} me, opponent;

// оставшееся кол-во ходов
int turnsRemaining;

// ход, на котором содержимое в печке приготовится
// если == -1, то печь свободна
// иначе - печь сейчас занята и знанчение ovenReadyTurn соответсвует ходу, на котором можно извлечь содержимое
int ovenReadyTurn = -1;

// флаг начала раунда, когда надо сделать много заготовок
// этот флаг уходит в false только когда сделано требуемое кол-во заготовок
bool needPrepairing = PREPAIRING_FLAG;

// расчет дистаницй от меня до всех клеток кухни с учетом существование напарника как преграды
void calcDistancesWithOpponent(){

	// nonChekedCellsNb - количество непроверенных проходных ячеек
	int i, j, nonChekedCellsNb = 0, k, l;

	// количество проходных ячеек, до которых надо найти расстояния
	nonChekedCellsNb = 29;

	// начальная инициализация 777
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			kitchen.distArrayForMe[i][j] = 777;

	// ячейка со мной - проходная, расстояние == 0
	kitchen.distArrayForMe[me.y][me.x] = 0;

	// если оппонент закрывает вход в кишку, то надо убрать те проходные ячейки, которые он закрывает

	// ПО ВСЕМ ПРОХОДНЫМ ЯЧЕЙКАМ
	// пока на полу есть проходные ячейки с неизвестной дистанцией
	int counter = 22;
	//while(nonChekedCellsNb > 1)
	while(counter > 0)
	{
		for(i = 1; i < K_HEIGHT - 1; i++)
			for(j = 1; j < K_WIDTH - 1; j++)
				// если находим проходную ячейку, для которой известно расстояние до нее, обновляем значения всех смежных проходных ячеек
				// ЭТА ЯЧЕЙКА ДОЛЖНА ОТЛИЧАТЬСЯ ОТ ТОЙ ЯЧЕЙКИ, НА КОТОРОЙ СТОИТ ОППОНЕНТ
				if((kitchen.distArrayForMe[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY))
				{
					//cerr << "(" << j << "," << i << ") = " << kitchen.distArrayForMe[i][j] << endl;

					// обновляются только смежные4 ячейки
					// если расстояние до ячейки еще не известно, она проходная и в ней нет оппонента, то обновляем

					// левая
					if((kitchen.distArrayForMe[i][j - 1] > 333) && (kitchen.cellArray[i][j - 1] == EMPTY) && !(i == opponent.y && (j - 1) == opponent.x)){
						kitchen.distArrayForMe[i][j - 1] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	1. (" << j - 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j - 1] << endl;
					}

					// правая
					if(kitchen.distArrayForMe[i][j + 1] > 333 && kitchen.cellArray[i][j + 1] == EMPTY && !(i == opponent.y && (j + 1) == opponent.x)){
						kitchen.distArrayForMe[i][j + 1] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	2. (" << j + 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j + 1] << endl;
					}

					// верхняя
					if(kitchen.distArrayForMe[i - 1][j] > 333 && kitchen.cellArray[i - 1][j] == EMPTY && !((i - 1) == opponent.y && j == opponent.x)){
						kitchen.distArrayForMe[i - 1][j] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	3. (" << j << "," << i - 1 << ") = " << kitchen.distArrayForMe[i - 1][j] << endl;
					}

					// нижняя
					if(kitchen.distArrayForMe[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY && !((i + 1) == opponent.y && j == opponent.x)){
						kitchen.distArrayForMe[i + 1][j] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	4. (" << j << "," << i + 1 << ") = " << kitchen.distArrayForMe[i + 1][j] << endl;
					}
				}
		counter--;
	}

	// обновление расстояние до всех ячеек стола
	// проходим по всем ячейкам кухни и если ячейка не проходная, то расстоние до нее == минимальному из смежных8 проходных
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			// если это ячейка стола
			if(kitchen.cellArray[i][j] != EMPTY){

				//cerr << endl << "check coord " << j << " " << i << endl;

				int minDist = 777;

				// проверяем смежные8
				// ищем ПРОХОДНЫЕ с минимальным расстоянием
				for(k = i - 1; k <= i + 1; k++)
					for(l = j - 1; l <= j + 1; l++)
						// проверка на валидность
						if(isExistCell(pair<int, int>{l, k}))
						{
							// если это проходная ячейка и расстояние до нее меньше минимума, то сохраняем это расстояние как новым минимум
							if(kitchen.cellArray[k][l] == EMPTY && kitchen.distArrayForMe[k][l] < minDist && kitchen.distArrayForMe[k][l] != -1)
							{
								minDist = kitchen.distArrayForMe[k][l];
								//cerr << "	coord " << l << " " << k << " is empty" << endl;
							}
						}

				//cerr << " dist to " << j << " " << i << " = " << minDist << endl;

				// этой ячейка стола присваивается минимальное проходное расстояние
				kitchen.distArrayForMe[i][j] = minDist;
			}

	/*
	cerr << "РАССТОЯНИЯ:" << endl;
	for(i = 0; i < K_HEIGHT; i++)
	{
		for(j = 0; j < K_WIDTH; j++)
		{
			cerr.width(3);
			cerr << kitchen.distArrayForMe[i][j] << " ";
		}
		cerr << endl;
	}
	*/
};

// расчет дистаницй от меня до всех клеток кухни БЕЗ учета напарника
void calcDistancesWithoutOpponent(){

	// nonChekedCellsNb - количество непроверенных проходных ячеек
	int i, j, k, l;

	// начальная инициализация 777
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			kitchen.distArrayForWithoutOpponent[i][j] = 777;

	// ячейка со мной - проходная, расстояние == 0
	kitchen.distArrayForWithoutOpponent[me.y][me.x] = 0;

	// ПО ВСЕМ ПРОХОДНЫМ ЯЧЕЙКАМ
	// пока на полу есть проходные ячейки с неизвестной дистанцией
	int counter = 21;
	//while(nonChekedCellsNb > 1)
	while(counter > 0)
	{
		for(i = 1; i < K_HEIGHT - 1; i++)
			for(j = 1; j < K_WIDTH - 1; j++)
				// если находим проходную ячейку, для которой известно расстояние до нее, обновляем значения всех смежных проходных ячеек
				// ЭТА ЯЧЕЙКА ДОЛЖНА ОТЛИЧАТЬСЯ ОТ ТОЙ ЯЧЕЙКИ, НА КОТОРОЙ СТОИТ ОППОНЕНТ
				if((kitchen.distArrayForWithoutOpponent[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY))
				{
					// обновляются только смежные4 ячейки
					// если расстояние до ячейки еще не известно и она проходная, то обновляем

					// левая
					if( kitchen.distArrayForWithoutOpponent[i][j - 1] > 333 && kitchen.cellArray[i][j - 1] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i][j - 1] = kitchen.distArrayForWithoutOpponent[i][j] + 1;

					// правая
					if( kitchen.distArrayForWithoutOpponent[i][j + 1] > 333 && kitchen.cellArray[i][j + 1] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i][j + 1] = kitchen.distArrayForWithoutOpponent[i][j] + 1;

					// верхняя
					if( kitchen.distArrayForWithoutOpponent[i - 1][j] > 333 && kitchen.cellArray[i - 1][j] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i - 1][j] = kitchen.distArrayForWithoutOpponent[i][j] + 1;

					// нижняя
					if( kitchen.distArrayForWithoutOpponent[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i + 1][j] = kitchen.distArrayForWithoutOpponent[i][j] + 1;
				}
		counter--;
	}

	// обновление расстояние до всех ячеек стола
	// проходим по всем ячейкам кухни и если ячейка не проходная, то расстоние до нее == минимальному из смежных8 проходных
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			// если это ячейка стола
			if(kitchen.cellArray[i][j] != EMPTY){

				int minDist = 777;

				// проверяем смежные8
				// ищем ПРОХОДНЫЕ с минимальным расстоянием
				for(k = i - 1; k <= i + 1; k++)
					for(l = j - 1; l <= j + 1; l++)
						// проверка на валидность
						if(isExistCell(pair<int, int>{l, k}))
						{
							// если это проходная ячейка и расстояние до нее меньше минимума, то сохраняем это расстояние как новым минимум
							if(kitchen.cellArray[k][l] == EMPTY && kitchen.distArrayForWithoutOpponent[k][l] < minDist && kitchen.distArrayForWithoutOpponent[k][l] != -1)
							{
								minDist = kitchen.distArrayForWithoutOpponent[k][l];
							}
						}

				//cerr << " dist to " << j << " " << i << " = " << minDist << endl;

				// этой ячейка стола присваивается минимальное проходное расстояние
				kitchen.distArrayForWithoutOpponent[i][j] = minDist;
			}
	/*
	cerr << "РАССТОЯНИЯ БЕЗ УЧЕТА ОППОНЕНТА:" << endl;
	for(i = 0; i < K_HEIGHT; i++)
	{
		for(j = 0; j < K_WIDTH; j++)
		{
			cerr.width(3);
			cerr << kitchen.distArrayForWithoutOpponent[i][j] << " ";
		}
		cerr << endl;
	}
	*/
};

// расчет расстояния для оппонента
void calcOpponentDistances(){

	// nonChekedCellsNb - количество непроверенных проходных ячеек
	int i, j, nonChekedCellsNb = 0, k, l;

	// количество проходных ячеек, до которых надо найти расстояния
	nonChekedCellsNb = 29;

	// начальная инициализация 777
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			kitchen.distArrayForOpponent[i][j] = 777;

	// ячейка со мной - проходная, расстояние == 0
	kitchen.distArrayForOpponent[opponent.y][opponent.x] = 0;

	// ПО ВСЕМ ПРОХОДНЫМ ЯЧЕЙКАМ
	// пока на полу есть проходные ячейки с неизвестной дистанцией
	int counter = 22;
	//while(nonChekedCellsNb > 1)
	while(counter > 0)
	{
		for(i = 1; i < K_HEIGHT - 1; i++)
			for(j = 1; j < K_WIDTH - 1; j++)
				// если находим проходную ячейку, для которой известно расстояние до нее, обновляем значения всех смежных проходных ячеек
				// ЭТА ЯЧЕЙКА ДОЛЖНА ОТЛИЧАТЬСЯ ОТ ТОЙ ЯЧЕЙКИ, НА КОТОРОЙ СТОИТ ОППОНЕНТ
				if((kitchen.distArrayForOpponent[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY))
				{
					//cerr << "(" << j << "," << i << ") = " << kitchen.distArrayForMe[i][j] << endl;

					// обновляются только смежные4 ячейки
					// если расстояние до ячейки еще не известно, она проходная и в ней нет оппонента, то обновляем

					// левая
					if((kitchen.distArrayForOpponent[i][j - 1] > 333) && (kitchen.cellArray[i][j - 1] == EMPTY) && !(i == me.y && (j - 1) == me.x)){
						kitchen.distArrayForOpponent[i][j - 1] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	1. (" << j - 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j - 1] << endl;
					}

					// правая
					if(kitchen.distArrayForOpponent[i][j + 1] > 333 && kitchen.cellArray[i][j + 1] == EMPTY && !(i == me.y && (j + 1) == me.x)){
						kitchen.distArrayForOpponent[i][j + 1] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	2. (" << j + 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j + 1] << endl;
					}

					// верхняя
					if(kitchen.distArrayForOpponent[i - 1][j] > 333 && kitchen.cellArray[i - 1][j] == EMPTY && !((i - 1) == me.y && j == me.x)){
						kitchen.distArrayForOpponent[i - 1][j] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	3. (" << j << "," << i - 1 << ") = " << kitchen.distArrayForMe[i - 1][j] << endl;
					}

					// нижняя
					if(kitchen.distArrayForOpponent[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY && !((i + 1) == me.y && j == me.x)){
						kitchen.distArrayForOpponent[i + 1][j] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	4. (" << j << "," << i + 1 << ") = " << kitchen.distArrayForMe[i + 1][j] << endl;
					}
				}
		counter--;
	}

	// обновление расстояние до всех ячеек стола
	// проходим по всем ячейкам кухни и если ячейка не проходная, то расстоние до нее == минимальному из смежных8 проходных
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			// если это ячейка стола
			if(kitchen.cellArray[i][j] != EMPTY){

				//cerr << endl << "check coord " << j << " " << i << endl;

				int minDist = 777;

				// проверяем смежные8
				// ищем ПРОХОДНЫЕ с минимальным расстоянием
				for(k = i - 1; k <= i + 1; k++)
					for(l = j - 1; l <= j + 1; l++)
						// проверка на валидность
						if(isExistCell(pair<int, int>{l, k}))
						{
							// если это проходная ячейка и расстояние до нее меньше минимума, то сохраняем это расстояние как новым минимум
							if(kitchen.cellArray[k][l] == EMPTY && kitchen.distArrayForOpponent[k][l] < minDist && kitchen.distArrayForOpponent[k][l] != -1)
							{
								minDist = kitchen.distArrayForOpponent[k][l];
								//cerr << "	coord " << l << " " << k << " is empty" << endl;
							}
						}

				//cerr << " dist to " << j << " " << i << " = " << minDist << endl;

				// этой ячейка стола присваивается минимальное проходное расстояние
				kitchen.distArrayForOpponent[i][j] = minDist;
			}

	/*
	cerr << "РАССТОЯНИЯ:" << endl;
	for(i = 0; i < K_HEIGHT; i++)
	{
		for(j = 0; j < K_WIDTH; j++)
		{
			cerr.width(3);
			cerr << kitchen.distArrayForMe[i][j] << " ";
		}
		cerr << endl;
	}
	*/
};

// расчет расстояние от точки моего повара до всех клеток кухни
void calcDistances(){

	calcDistancesWithOpponent();
	calcDistancesWithoutOpponent();
	calcOpponentDistances();
};

// возвращает индекс наиболее вероятного заказа, который делает напрник
void estimateOpponentOrderIdx(){

	// если у него в руках есть НЕ ПУСТАЯ тарелка, то опредяем по содержимому
	if(opponent.ingridsInHands > DISH){

		// количество заказов текущих клиентов, для которых подходит содержимое тарелки оппонента
		int isPartsOfCollectNb = 0, idx = -1;

		for(int i = 0; i < 3; i++)
			if(PRODUCT::is1PartOf2(opponent.ingridsInHands, clients.curClientVec.at(i).ingridCollection)){
				isPartsOfCollectNb++;
				idx = i;
			}

		/// если такой заказ только один, то он и является выполняемым
		// если такой заказ есть, он и считается выполняемым поваром-напрником
		if(isPartsOfCollectNb > 0){
			opponent.servedClientIdx = idx;
		}
		// если в руках НЕ ЗАКАЗ, то это какой то ингридиент
		else
		{
			// если в руках сырой торт или порезанное тесто, то ищем любой заказ с тортом
			if(opponent.ingridsInHands == RAW_TART || CHPD_DOUGH){
				for(int i = 0; i < 3; i++)
					if(clients.curClientVec.at(i).ingridCollection & TART)
						opponent.servedClientIdx = i;
			}
			// если в руках тесто, то ищем любой заказ с тортом/круасаном
			else if(opponent.ingridsInHands == DOUGH){
				for(int i = 0; i < 3; i++)
					if(clients.curClientVec.at(i).ingridCollection & TART || clients.curClientVec.at(i).ingridCollection & CROISSANT)
						opponent.servedClientIdx = i;
			}
			// если в руках клубничка, то ищем любой заказ с резаной клубничкой
			else if(opponent.ingridsInHands == STRBR){
				for(int i = 0; i < 3; i++)
					if(clients.curClientVec.at(i).ingridCollection & CHPD_STRBR)
						opponent.servedClientIdx = i;
			}
		}
	}
	// если тарелки нет или она пустая, то нельзя сказать что делает напарник,
	// но можно предположить, что он делает самый дорогой заказ
	else
	{
		opponent.servedClientIdx = clients.getCurClientIdxWithMaxAward();
	}
};

// считывание инпутов
void readInput(){
	kitchen.existingProductVec.clear();
	clients.curClientVec.clear();

	// если раунд только начался, готовим заготовки
	if(turnsRemaining == 200 || turnsRemaining == 199 || turnsRemaining == 198){

		cerr << "НАЧАЛСЯ НОВЫЙ РАУНД" << endl;

		needPrepairing = PREPAIRING_FLAG;

		me.usedOven = false;
		me.desiredCollection = -1;
		me.servedClientIdx = -1;

		prepChpdStrw = 0;
		prepCrois = 0;
		prepTarts = 0;
	}

	// если все заготовки были сделаны
	if(needPrepairing && prepChpdStrw == 2 && prepCrois == 1) needPrepairing = false;

	me.read();
	opponent.read();

	// сброс карты существующих продуктов
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			kitchen.existingProductMap[i][j] = NONE;
	// спавнеры добавляются к мапе
	kitchen.existingProductMap[kitchen.iceCell.second][kitchen.iceCell.first]     = ICE_C;
	kitchen.existingProductMap[kitchen.dishCell.second][kitchen.dishCell.first]   = DISH;
	kitchen.existingProductMap[kitchen.doughCell.second][kitchen.doughCell.first] = DOUGH;
	kitchen.existingProductMap[kitchen.strbrCell.second][kitchen.strbrCell.first] = STRBR;
	kitchen.existingProductMap[kitchen.blbrCell.second][kitchen.blbrCell.first]   = BLBR;
	kitchen.existingProductMap[kitchen.ovenCell.second][kitchen.ovenCell.first]   = -1;
	kitchen.existingProductMap[kitchen.chopCell.second][kitchen.chopCell.first]   = -1;
	kitchen.existingProductMap[kitchen.winCell.second][kitchen.winCell.first]	  = -1;


	// существующие готовые ингридиенты на кухне
	cin >> kitchen.existingProductNb;
	kitchen.existingProductVec.clear();
	kitchen.existingProductVec.resize(kitchen.existingProductNb);
	for(auto &prod : kitchen.existingProductVec){
		prod.readInput();
		kitchen.existingProductMap[prod.y][prod.x] = prod.ingridCollection;
	}

	// все про печку
	string ingridInOven;
	cin >> ingridInOven >> kitchen.ovenTimer;
	kitchen.ovenContents = PRODUCT::splitProduct2Ingrid(ingridInOven);

	// заказы текущих клиентов
	clients.readCurClientsInput();

	// расчет дистанций до всех мест кухни
	calcDistances();

	// предполгаем, какой заказ делает напарник
	estimateOpponentOrderIdx();

	// ближайшая свободноя чейка со столом
	kitchen.nearFreeCell = kitchen.getNearFreeTable();

	me.updateCoords();
	opponent.updateCoords();

	//cerr << "ближайшая свободная ячейка: " << kitchen.nearFreeCell.first << " " << kitchen.nearFreeCell.second << endl;
};

// использование нужного продукта
void use(pair<int, int> p){
	if(isExistCell(p)){
		cout << "USE " << p.first << " " << p.second << + "; " + get_random_spell() << + ";" << endl;
		cerr << "USE PAIR " << p.first << " " << p.second << endl;
	}
};

// движение в указанную точку
// если расстояние по кратчайшему пути <= 6, то проще заблокировать напарника и идти напролом, чем обходить всю кухню
void moveAsTank(pair<int, int> p)
{
	// если расстояние с учетом напарника равно расстоянию без учета напарника, то он не мешает и можно идти обычным юзом
	if(kitchen.distArrayForMe[p.second][p.first] == kitchen.distArrayForWithoutOpponent[p.second][p.first])
	{
		use(p);
	}
	// меньше быть не может
	// следовательно этот случай выполняется, когда расстояние с учетом напарника БОЛЬШЕ расстояния без учета напарника
	else
	{
		// движение напролом только если расстояние <= 8 и напарник не у печки
		if(kitchen.distArrayForWithoutOpponent[p.second][p.first] <= 8 && !opponent.isOvenNear() && opponent.howLongIStateOnPlace < BLOCK_TRESHOLD)
		{
			cerr << "выгоднее идти напролом" << endl;

			// надо определить координаты клетки пола, в которую я могу перейти, чтобы скоратить расстояние до конечной цели

			// сначала определяются две клетки пола сбоку от напарника
			// из этих двух клеток выбирается клетка, расстояние до которой минимально и делается мув в эту клетку

			pair<int, int> point{-1,-1};
			int minDist = 777;

			for(int i = 0; i < K_HEIGHT; i++)
				for(int j = 0; j < K_WIDTH; j++)
					// если это клетка пола и расстояние до этой клетки меньше минимума
					if(kitchen.cellArray[i][j] == EMPTY && kitchen.distArrayForMe[i][j] < minDist)
					{
						//cerr << "клетка " << j << " " << i << " имеет расстояние " << kitchen.distArrayForMe[i][j] << endl;

						// если это верхняя либо нижняя клетка
						if( ((i == opponent.y - 1) && (j == opponent.x)) || ((i == opponent.y + 1) && (j == opponent.x))
						   // если левая или правая
						 || ((i == opponent.y) && (j == opponent.x - 1)) || ((i == opponent.y) && (j == opponent.x + 1)) )
						{
							point = pair<int, int>{j, i};
							minDist = kitchen.distArrayForMe[i][j];
						}
					}
			cerr << "движение напролом в клетку " << point.first << " " << point.second << endl;
			cout << "MOVE " << point.first << " " << point.second << endl;
		}
		// если расстояние > 8 или напарник у печки, то лучше идти в обход
		else
		{
			use(p);
		}
	}
};

// если в руках  ингридиент, то возможно есть тарелка, куда его можно положить
// если в руках неготовый ингридиент, надо его приготовить, но только если приспособление рядом
void findWherePut(short ingrid){

	cerr << "ищу куда деть ингридиент из рук..." << endl;

	switch(ingrid){

		// если в руках готовый торт, то надо найти тарелку с чем то, к которой можно добавить торт
		// при этом суммарное содержимое тарелки должно будет удовлетворять одному из существующих заказов
		case TART:
		{
			// сброс флага надобности 
			if(me.whatNeedMake == TART)
				me.whatNeedMake = -1;

			// индекс существующей тарелки, если в итоге он будет равен -1, то надо просто выложить торт рядом
			int existingDishWhereCanPutTort = -1,
				minDist = 666;

			// среди всех существующих продуктов ищется тарелка, к которой можно добавить торт
			for(int i = 0; i < kitchen.existingProductNb; i++){

				if(kitchen.existingProductVec[i].ingridCollection & DISH // если это продукт с тарелкой
					&& !(kitchen.existingProductVec[i].ingridCollection & TART) // если тарелка не содержит торт
					&& (
						PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | TART), clients.curClientVec[0].ingridCollection) // эта тарелка с тортом будет подходить для 0-го заказа
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | TART), clients.curClientVec[1].ingridCollection) // эта тарелка с тортом будет подходить для 1-го заказа
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | TART), clients.curClientVec[2].ingridCollection) // эта тарелка с тортом будет подходить для 2-го заказа
						)
					&& kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] < minDist
				   )
				{
					existingDishWhereCanPutTort = i;
					minDist = kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x];
				}
			}

			// если подходящая тарелка найдена - используем ее
			if(existingDishWhereCanPutTort != -1)
			{
				cerr << "	существует тарелка, в которую можно положить готовый торт, иду к ней" << endl;
				moveAsTank(pair<int, int>{kitchen.existingProductVec[existingDishWhereCanPutTort].x, kitchen.existingProductVec[existingDishWhereCanPutTort].y});
			}
			// если такой тарелки нет, то кладем на ближайший свободный стол
			else
			{
				cerr << "	не существует тарелки, в которую можно положить готовый торт, кладу на свободный стол" << endl;
				moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		// если в руках готовый круасанн, то надо найти тарелку с чем то, к которой можно добавить круасанн
		// при этом суммарное содержимое тарелки должно будет удовлетворять одному из существующих заказов
		case CROISSANT:
		{
			// сброс флага надобности 
			if(me.whatNeedMake == CROISSANT)
				me.whatNeedMake = -1;

			// индекс существующей тарелки, если в итоге он будет равен -1, то надо просто выложить круасанн рядом
			int existingDishWhereCanPutCroissant = -1;

			// среди всех существующих продуктов ищется тарелка, к которой можно добавить круасанн
			for(int i = 0; i < kitchen.existingProductNb; i++){

				if(		 kitchen.existingProductVec[i].ingridCollection & DISH // если это продукт с тарелкой
					&& !(kitchen.existingProductVec[i].ingridCollection & CROISSANT) // если тарелка не содержит круасанн
					&& (
						   PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CROISSANT), clients.curClientVec[0].ingridCollection) // эта тарелка с круасанном будет подходить для 0-го заказа
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CROISSANT), clients.curClientVec[1].ingridCollection) // эта тарелка с круасанном будет подходить для 1-го заказа
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CROISSANT), clients.curClientVec[2].ingridCollection) // эта тарелка с круасанном будет подходить для 2-го заказа
						)
				   )
					existingDishWhereCanPutCroissant = i;
			}

			// если подходящая тарелка найдена - используем ее
			if(existingDishWhereCanPutCroissant != -1)
			{
				cerr << "	существует тарелка, в которую можно положить готовый круасанн, иду к ней" << endl;
				moveAsTank(pair<int, int>{kitchen.existingProductVec[existingDishWhereCanPutCroissant].x, kitchen.existingProductVec[existingDishWhereCanPutCroissant].y});
			}
			// если такой тарелки нет, то кладем на ближайший свободный стол
			else
			{
				cerr << "	не существует тарелки, в которую можно положить готовый круасанн, кладу на свободный стол" << endl;
				moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}
	
		// если в руках порезанная клубничка, то надо найти тарелку с чем то, к которой можно добавить порезанную клубничку
		// при этом суммарное содержимое тарелки должно будет удовлетворять одному из существующих заказов
		case CHPD_STRBR:
		{
			// сброс флага надобности 
			if(me.whatNeedMake == CHPD_STRBR)
				me.whatNeedMake = -1;

			// индекс существующей тарелки, если в итоге он будет равен -1, то надо просто выложить порезанную клубничку рядом
			int existingDishWhereCanPutChpdStrbr = -1,
				minAward = 0;

			// среди всех существующих продуктов ищется тарелка, к которой можно добавить порезанную клубничку
			for(int i = 0; i < kitchen.existingProductNb; i++){

				// если это продукт с тарелкой и тарелка не содержит порезанную клубничку
				if(kitchen.existingProductVec[i].ingridCollection & DISH && !(kitchen.existingProductVec[i].ingridCollection & CHPD_STRBR))
				{
					// надо проверить, будет ли тарелка с порезанной клубничкой удовлетворять какому либо заказу
					for(int j = 0; j < clients.curClientNb; j++)
					{
						// если тарелка с порезанной клубничкой будет подходить для j-го заказа, запоминаем
						if(   PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CHPD_STRBR), clients.curClientVec[j].ingridCollection)
						   && kitchen.existingProductVec[i].ingridCollection > minAward) // ценность заказа
						{
							existingDishWhereCanPutChpdStrbr = i;
							minAward = kitchen.existingProductVec[i].ingridCollection;
						}
					}
				}
						   
			}

			// если подходящая тарелка найдена - используем ее
			if(existingDishWhereCanPutChpdStrbr != -1)
			{
				cerr << "	существует тарелка, в которую можно положить порезанную клубничку, иду к ней" << endl;
				moveAsTank(pair<int, int>{kitchen.existingProductVec[existingDishWhereCanPutChpdStrbr].x, kitchen.existingProductVec[existingDishWhereCanPutChpdStrbr].y});
			}
			// если такой тарелки нет, то кладем на ближайший свободный стол
			else
			{
				cerr << "	не существует тарелки, в которую можно положить порезанную клубничку, кладу на свободный стол" << endl;
				moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}
	
		// если в руках клубничка, то ее надо либо порезать, либо положить на свободный стол
		// если разделочная доска в пределгах досягаемости за 1 ход, то идем резать
		// иначе - кладем на свободный стол
		case STRBR:
		{
			if(kitchen.distArrayForMe[kitchen.chopCell.second][kitchen.chopCell.first] <= 4)
				moveAsTank(kitchen.chopCell);
			else
				moveAsTank(kitchen.nearFreeCell);
			break;
		}

		// если в руках тесто, то надо выяснить что нужнее: торт или круасанн
		// надо посмотреть заказ, который сечас выполняет мой повар
		// если по заказу нужен торт, а на кухне нет отдельного торта / торта на тарелке, то надо готовить торт
		// если по заказу нужен круасанн, а на кухне нет отдельного круасана / круасана на тарелке, то надо готовить круасанн
		case DOUGH:
		{

			// в зависимости от того, что сейчас нужно делать из теста
			switch(me.whatNeedMake){

				// если нужен торт - несем тесто на резку
				case TART:
					moveAsTank(kitchen.chopCell);
					break;

				// если нужен круасанн - несем тесто в духовку
				case CROISSANT:

					// если мы стоим рядом с печкой
					if(me.isOvenNear())
					{
						// если печка свободна - заклыдваем в нее выпечку и помечаем, что печка нами используется
						if(kitchen.ovenContents == NONE)
						{
							me.usedOven = true;
							moveAsTank(kitchen.ovenCell);
						}
						// если в печке что то есть, то надо выложить торт из рук и забрать содержимое печки
						else
						{
							moveAsTank(kitchen.nearFreeCell);
						}
					}
					// если не у печки - идем к печке
					else
					{
						moveAsTank(kitchen.ovenCell);
					}

					break;

				// если тесто не нужно ни для чего, то выкладываем его рядом
				default:
					moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		// если в рубленое тесто и черничка в пределах одного хода
		// то заправляем тесто
		// если черничка далеко - выбрасываем рубленое тесто
		case CHPD_DOUGH:
		{
			// в зависимости от того, что сейчас нужно делать из теста
			switch(me.whatNeedMake){

				// если нужен торт - несем пирог на запрвка
				case TART:
					moveAsTank(kitchen.blbrCell);
					break;

				// если торт не нужен, то нет смысла сюсюкаться с этим пирогом, выбрасываем
				default:
					moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		// если в руках сырой пирог
		case RAW_TART:
		{
			// в зависимости от того, что сейчас нужно делать из теста
			switch(me.whatNeedMake){

				// если нужен торт - несем пирог на заправку
				case TART:

					// если мы стоим рядом с печкой
					if(me.isOvenNear())
					{
						// если печка свободна - заклыдваем в нее выпечку и помечаем, что печка нами используется
						if(kitchen.ovenContents == NONE)
						{
							me.usedOven = true;
							moveAsTank(kitchen.ovenCell);
						}
						// если в печке что то есть, то надо выложить торт из рук и забрать содержимое печки
						else
						{
							moveAsTank(kitchen.nearFreeCell);
						}
					}
					// если не у печки - идем к печке
					else
					{
						moveAsTank(kitchen.ovenCell);
					}

					break;

				// если торт не нужен, то нет смысла сюсюкаться с этим пирогом, выбрасываем
				default:
					moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		case DISH:
		{
			if(kitchen.ovenContents != NONE)
				use(kitchen.ovenCell);
			else
				moveAsTank(kitchen.nearFreeCell);

			break;
		}

		default:
			// если в руках тарелка с чем то, то надо ее максимально укомплектовать
			if(ingrid > DISH)
			{

				cerr << "в руках тарелка с чем то, пробуем немного собрать заказ" << endl;

				// если по заказу нужен круасан/торт/клубничка, а в руках есть тарелка но нет какого то из этих ингридиентов, то надо поискать на кухне какие то ингридиенты
				pair<int, int>  cellWithTart      = kitchen.findNearest(TART),
								cellWithCroissant = kitchen.findNearest(CROISSANT),
								cellWithChpdStrbr = kitchen.findNearest(CHPD_STRBR);

				// если по заказу торт нужен, на тарелке его нет и есть ячейка с тортом, то идем его брать
				if(me.desiredCollection & TART && !(ingrid & TART) && isExistCell(cellWithTart))
				{
					cerr << "	можно добавить на тарелку торт" << endl;
					moveAsTank(cellWithTart);
				}
				// если по заказу круасанн нужен, на тарелке его нет и есть ячейка с круасанном, то идем его брать
				else if(me.desiredCollection & CROISSANT && !(ingrid & CROISSANT) && isExistCell(cellWithCroissant))
				{
					cerr << "	можно добавить на тарелку круасанн" << endl;
					moveAsTank(cellWithCroissant);
				}
				// если по заказу порезанная клубничка нужна, на тарелке ее нет и есть ячейка с порезанной клубничкой, то идем ее брать
				else if(me.desiredCollection & CHPD_STRBR && !(ingrid & CHPD_STRBR) && isExistCell(cellWithChpdStrbr))
				{
					cerr << "	можно добавить на тарелку порезанную клубничку" << endl;
					moveAsTank(cellWithChpdStrbr);
				}
				// если в печке есть готовая выпечка, которую можно добавить на тарелку
				else if(kitchen.ovenContents == CROISSANT || kitchen.ovenContents == TART){

					// если на тарелке еще нет содержимого печки и если содержимое тареки с содержиммым печки подойдет под один из заказов, то берем содержимое печки и меняем заказ
					if(   !(me.ingridsInHands & kitchen.ovenContents)
						&& (   PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[0].ingridCollection)
							|| PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[1].ingridCollection)
							|| PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[2].ingridCollection)
						   )
					   )
					{
						moveAsTank(kitchen.ovenCell);
					}
					// если в руках уже есть содержимое печки, то надо выложить это из рук и забрать готовую выпечку из печки
					else
					{
						cerr << "		в печке готовая выпечка, не могу ее добавить на свою тарелку, кладу тарелку на стол" << endl;
						moveAsTank(kitchen.nearFreeCell);
					}
				}
				// если и в печке нет ничего готового, и нет ни одного готового ингридиента, который можно было бы добавить к тарелке, то выкладываем на стол
				else
				{
					cerr << "	к тарелке нечего добавить, кладу ее на стол" << endl;
					moveAsTank(kitchen.nearFreeCell);
				}
			}
			else
			{
				moveAsTank(kitchen.nearFreeCell);
			}
	}
};

// попытка спасти выпечку
// если начинаем спасение возвращает true
// если не будет спасения, то возвращает false
bool saveBaking(){

	// если в духовке готовый продукт, созданный по моей инициативе, то достаем его
	if( (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT) )
	{
		// если расстояние до печки позволяет успеть забрать выпечку
		if( kitchen.distArrayForMe[kitchen.ovenCell.second][kitchen.ovenCell.first] <= (kitchen.ovenTimer / 2 - 1) * 4 )
		{
			cerr << "		в духовке есть готовая выпечка, расстояние позволяет ее забрать" << endl;

			// если напарник далеко от духовки ИЛИ ЕСЛИ ОН В АУТЕ, то надо забрать
			if(kitchen.distArrayForOpponent[kitchen.ovenCell.second][kitchen.ovenCell.first] > 4 || opponent.howLongIStateOnPlace > 5)
			{
				cerr << "			напарник далеко от духовки, надо забрать" << endl;

				// если печка рядом
				if(me.isOvenNear())
				{
					cerr << "			духовка рядом, пытаюсь забрать выпечку" << endl;

					// если можем забрать - забираем
					if(me.ingridsInHands == NONE // если руки пустые
					   // либо если в руках есть тарелка, на которой нет содержимого духовки, и при добавлении содержимого духовки на тарелку общее блюдо удовлетворит одного из клиентов
					   || (!(me.ingridsInHands & kitchen.ovenContents)
						   && (PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[0].ingridCollection)
							   || PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[1].ingridCollection)
							   || PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[2].ingridCollection)
							   )
						   && (me.ingridsInHands & DISH)
						  )
					   )
					{
						moveAsTank(kitchen.ovenCell);
					}
					// если в руках что то лишнее
					else
						moveAsTank(kitchen.nearFreeCell);

					return true;
				} else
				{
					cerr << "			духовка не рядом, иду к ней" << endl;
					moveAsTank(kitchen.ovenCell);
					return true;
				}
			}
			else
			{
				cerr << "			напарник близко к духовке, наверно заберет" << endl;
				return false;
			}
		}
		// если не упсеем забрать выпечку
		else
		{
			cerr << "		в духовке есть готовая выпечка, но я не упсею ее забрать" << endl;
			return false;
		}
	}
	// если в печке нет готовой выпечки
	else
		return false;
};

// создание на кухне порезанной клубнички
void prepairChpdStrbr(){

	me.whatNeedMake = CHPD_STRBR;

	cerr << "готовлю клубничку" << endl;

	// если руки пустые, берем цельную клубничку
	if(me.ingridsInHands == NONE)
		moveAsTank(kitchen.strbrCell);
	// если в руках цельная клубничка, режем ее
	else if(me.ingridsInHands == STRBR)
		moveAsTank(kitchen.chopCell);
	// если что то другое - выкладываем
	else
		findWherePut(me.ingridsInHands);
};

// создание на кухне круасанна
void prepairCroissant(){

	me.whatNeedMake = CROISSANT;

	// если я уже использовал печку, то надо либо ждать приготовления круасанна, либо забрать круасан если он готов
	if(me.usedOven)
	{
		cerr << "я уже начал использовать печку" << endl;

		switch(kitchen.ovenContents){

			// если в печке готовится тесто и до его приготовления еще много времени, то можно поискать работу на кухне...
			case RAW_TART:
			case DOUGH:
			{
				cerr << "	пока готовится моя выпечка, надо принести тарелку" << endl;

				if(me.ingridsInHands == NONE)
				{
					cerr << "		в руках ничего нет" << endl;

					// сначала пытаемся найти тарелку, в которую можно будет положить круасанн

					int minAward = 0;
					int prodIdx = -1;

					// надо найти тарелку с подходящими ингридиентами, или, если такой нет, взять чистую
					for(int i = 0; i < kitchen.existingProductNb; i++)
					{
						if((PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // если это часть моего заказа
						   && (kitchen.existingProductVec[i].ingridCollection & DISH)  // если продукт содержит тарелку
						   && (kitchen.existingProductVec[i].ingridCollection > minAward) // самый ценный продукт
						   && (kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] <= (kitchen.ovenTimer / 2 - 1) * 4)) // ограничение на расстояние
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// если не нашли тарелку, подходящую к нашему заказу, проверяем, может есть тарелка, удволетворяющая другому заказу, в которую можно добавить круасанн
					if(prodIdx == -1)
					{

						int newOrderIdxWithCroissant = -1;

						for(int i = 0; i < kitchen.existingProductNb; i++)
						{
							if( (kitchen.existingProductVec[i].ingridCollection & DISH)  // если продукт содержит тарелку
								&& !(kitchen.existingProductVec[i].ingridCollection & CROISSANT) // и в тарелке нету круасанна
							    && (kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] <= (kitchen.ovenTimer / 2 - 1) * 4)) // ограничение на расстояние
							{
								for(int j = 0; j < clients.curClientNb; j++)
								{
									// если тарелка после добавления в нее торта будет подходить для заказа, то берем эту тарелку
									if(PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection | CROISSANT, clients.curClientVec[j].ingridCollection))
									{
										minAward = kitchen.existingProductVec[i].ingridCollection;
										prodIdx = i;
										newOrderIdxWithCroissant = j;
									}
								}
							}
						}

						// если и эта проверка не дала результатов, то надо брать чистую тарелку
						if(prodIdx == -1)
						{
							// ищем ближайшую чистую тарелку
							pair<int, int> dishCell = kitchen.findNearest(DISH);

							// если успеем взять тарелку, пока готовится тесто
							if(kitchen.distArrayForMe[dishCell.second][dishCell.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
							{
								moveAsTank(dishCell);
							}
							// если расстояние до чистой тарелки > (kitchen.ovenTimer/2 - 2), то надо сделать что то другое
							else
							{
								// ищем ближайший продукт, который можно испечь
								pair<int, int> cellWithRawTart = kitchen.findNearest(RAW_TART);
								pair<int, int> cellWithDough   = kitchen.findNearest(DOUGH);

								// если существует сырой пирог, то надо проверить, можем ли мы его взять
								if(isExistCell(cellWithRawTart))
								{
									cerr << "		на кухне сущствует сырой пирог..." << endl;
									// если сырой пирог в пределах досягаемости
									if(kitchen.distArrayForMe[cellWithRawTart.second][cellWithRawTart.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		расстояние до сырого пирога позволяет его взять" << endl;
										moveAsTank(cellWithRawTart);
										return;
									}
									// если он слишком далеко, пробуем найти тесто
									else
									{
										cerr << "		до сырого пирога слишком далеко идти" << endl;
									}
								}

								// если существует тесто, то надо проверить, можем ли мы его взять
								if(isExistCell(cellWithDough))
								{
									cerr << "		на кухне сущствует тесто..." << endl;
									// если тестл в пределах досягаемости
									if(kitchen.distArrayForMe[cellWithDough.second][cellWithDough.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		расстояние до теста позволяет его взять" << endl;
										moveAsTank(cellWithDough);
										return;
									}
									// если он слишком далеко, пробуем найти тесто
									else
									{
										cerr << "		до теста слишком далеко идти" << endl;
									}
								}

								// если нет ни пирога ни теста, то просто идем к печке
								moveAsTank(kitchen.ovenCell);
							}
						}
						// если нашли тарелку, в которую вписался бы круасан, то меняем заказ на тот, что соответсвует этой тарелке и продолжаем
						else
						{
							// если заказ существует, то берем его в качестве исполняемого
							// и идем за тарелкой
							if(newOrderIdxWithCroissant != -1)
							{
								cerr << "смена заказа, есть тарелка которая с круасаном удовлетворит клиента " << newOrderIdxWithCroissant << endl;

								me.desiredCollection = clients.curClientVec[newOrderIdxWithCroissant].ingridCollection;
								me.servedClientIdx = newOrderIdxWithCroissant;

								moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
							}
							// если такого заказа нет, то кладем на стол
							else
							{
								cerr << "КАКОЙ ТО ОЧЕНЬ СЛОЖНЫЙ СЛУЧАЙ" << endl;
								moveAsTank(kitchen.nearFreeCell);
							}
						}
					}
					// если нашли тарлеку с чем подходящим для моего заказа
					else
					{
						cerr << "		есть хорошенькая тарелка с продуктами и повар успеет до нее дойти" << endl;
						moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "в руках тарелка, идем к печке" << endl;
					moveAsTank(kitchen.ovenCell);
				}
				else
				{
					//moveAsTank(kitchen.nearFreeCell);
					findWherePut(me.ingridsInHands);
				}

				break;
			}

			// если в печке готовый круасанн, надо его забрать
			case CROISSANT:
			{
				cerr << "круасанн готов, надо забрать из печки" << endl;

				// можем забирать из печки, если в руках тарелка без круасана
				// либо если в руках вообще ничего нет
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & CROISSANT)) )
				{
					// сбрасывается флаг использования мной печки
					me.usedOven = false;

					// сброс флага надобности 
					if(me.whatNeedMake == CROISSANT)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// если в руках что то лишнее - выклдываем на стол
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}
				break;
			}

			// если в печке готовый торт, надо его забрать
			case TART:
			{
				cerr << "торт готов, надо забрать из печки" << endl;

				// можем забирать из печки, если в руках тарелка без торта
				// либо если в руках вообще ничего нет
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & TART)) )
				{
					// сбрасывается флаг использования мной печки
					me.usedOven = false;

					// сброс флага надобности 
					if(me.whatNeedMake == TART)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// если в руках что то лишнее - выклдываем на стол
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}

				break;
			}

			// если печка пустая, а флаг me.usedOven == true, то это означает, что свою выпечку я уже забрал
			case NONE:
			{
				findWherePut(me.ingridsInHands);

				// сбрасывается флаг использования мной печки
				me.usedOven = false;
				break;
			}

			default:
				findWherePut(me.ingridsInHands);
		}
	}
	// если я еще не начал использовать печку
	else
	{
		cerr << "печка еще не используется" << endl;

		switch(me.ingridsInHands)
		{
			// если в руках ничего нет, то надо взять ближайшее тесто
			case NONE:

				// если повар у печки, а в печке готовая выпечка, то надо ее забрать оттуда
				if(me.isOvenNear())
				{
					switch(kitchen.ovenContents)
					{
						// если в руках пусто, повар у духовки, а в духовке круасанн, то забираем круасанн и сбрасываем флаг надобности круасанна
						case CROISSANT:
							me.whatNeedMake = -1;
							cerr << "повар у духовки, выпечка готова, забираем" << endl;
							moveAsTank(kitchen.ovenCell);
							break;

						// если в руках пусто, а в печке готовый торт, то надо его вытащить
						case TART:
							moveAsTank(kitchen.ovenCell);
							break;

						// если в печке пусто, либо там лежит тесто/пирог, но заложенный не мной
						case DOUGH:
						case RAW_TART:
						case NONE:
							// ищем существующее тесто / спавнер теста
							moveAsTank(kitchen.findNearest(DOUGH));
							break;
					}
				}
				// если в печке нет ничего готового
				// если не у духовки, и в руках ничего нет, то надо найти тесто
				else
				{
					// ищем существующее тесто / спавнер теста
					moveAsTank(kitchen.findNearest(DOUGH));
				}
				break;

			default:
				// если в руках что то есть
				findWherePut(me.ingridsInHands);
		}
	}
};

// создание на кухне торта
void prepairTart(){

	me.whatNeedMake = TART;

	// если я уже использовал духовку, то надо либо ждать приготовления торта, либо забрать торт если он готов
	if(me.usedOven)
	{
		cerr << "я уже начал использовать печку" << endl;

		switch(kitchen.ovenContents){

			// если в печке готовится тесто и до его приготовления еще много времени, то можно поискать работу на кухне...
			case RAW_TART:
			case DOUGH:
			{
				cerr << "пока готовится моя выпечка, надо принести тарелку" << endl;

				// если в руках что то есть кроме тарелки - выклдываем

				if(me.ingridsInHands == NONE)
				{
					cerr << "в руках ничего нет" << endl;

					int minAward = 0;
					int prodIdx = -1;

					// надо найти тарелку с подходящими ингридиентами, или, если такой нет, взять чистую
					for(int i = 0; i < kitchen.existingProductNb; i++)
					{
						if( (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // если это часть моего заказа
						   && (kitchen.existingProductVec[i].ingridCollection & DISH)  // если продукт содержит тарелку
						   && (kitchen.existingProductVec[i].ingridCollection > minAward) // самый ценный продукт
						   && (kitchen.distArrayForMe[kitchen.existingProductVec[i].x][kitchen.existingProductVec[i].y] <= (kitchen.ovenTimer / 2 - 1) * 4) )
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// если не нашли тарелку, подходящую к нашему заказу, проверяем, может есть тарелка, удволетворяющая другому заказу, в которую можно добавить торт
					if(prodIdx == -1)
					{

						int newOrderIdxWithTart = -1;

						for(int i = 0; i < kitchen.existingProductNb; i++)
						{
							if(     (kitchen.existingProductVec[i].ingridCollection & DISH)  // если продукт содержит тарелку
							    && !(kitchen.existingProductVec[i].ingridCollection & TART) // и в тарелке нету торта
							    && (kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] <= (kitchen.ovenTimer / 2 - 1) * 4)) // ограничение на расстояние
							{
								for(int j = 0; j < clients.curClientNb; j++)
								{
									// если тарелка после добавления в нее торта будет подходить для заказа, то берем эту тарелку
									if(PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection | TART, clients.curClientVec[j].ingridCollection))
									{
										minAward = kitchen.existingProductVec[i].ingridCollection;
										prodIdx = i;
										newOrderIdxWithTart = j;
									}
								}
							}
						}

						// если и эта проверка не дала результатов, то надо брать чистую тарелку
						if(prodIdx == -1)
						{
							// ищем ближайшую чистую тарелку
							pair<int, int> dishCell = kitchen.findNearest(DISH);

							// если успеем взять тарелку, пока готовится тесто
							if(kitchen.distArrayForMe[dishCell.second][dishCell.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
							{
								moveAsTank(dishCell);
							}
							// если расстояние до чистой тарелки > (kitchen.ovenTimer/2 - 2), то надо сделать что то другое
							else
							{
								// ищем ближайший продукт, который можно испечь
								pair<int, int> cellWithRawTart = kitchen.findNearest(RAW_TART);
								pair<int, int> cellWithDough = kitchen.findNearest(DOUGH);

								// если существует сырой пирог, то надо проверить, можем ли мы его взять
								if(isExistCell(cellWithRawTart))
								{
									cerr << "		на кухне сущствует сырой пирог..." << endl;
									// если сырой пирог в пределах досягаемости
									if(kitchen.distArrayForMe[cellWithRawTart.second][cellWithRawTart.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		расстояние до сырого пирога позволяет его взять" << endl;
										moveAsTank(cellWithRawTart);
										return;
									}
									// если он слишком далеко, пробуем найти тесто
									else
									{
										cerr << "		до сырого пирога слишком далеко идти" << endl;
									}
								}

								// если существует тесто, то надо проверить, можем ли мы его взять
								if(isExistCell(cellWithDough))
								{
									cerr << "		на кухне сущствует тесто..." << endl;
									// если тестл в пределах досягаемости
									if(kitchen.distArrayForMe[cellWithDough.second][cellWithDough.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		расстояние до теста позволяет его взять" << endl;
										moveAsTank(cellWithDough);
										return;
									}
									// если он слишком далеко, пробуем найти тесто
									else
									{
										cerr << "		до теста слишком далеко идти" << endl;
									}
								}

								// если нет ни пирога ни теста, то просто идем к печке
								moveAsTank(kitchen.ovenCell);
							}
						}
						// если нашли тарелку, в которую вписался бы торт, то меняем заказ на тот, что соответсвует этой тарелке и продолжаем
						else
						{
							// если заказ существует, то берем его в качестве исполняемого
							// и идем за тарелкой
							if(newOrderIdxWithTart != -1)
							{
								cerr << "смена заказа, есть тарелка которая с тортом удовлетворит клиента " << newOrderIdxWithTart << endl;

								me.desiredCollection = clients.curClientVec[newOrderIdxWithTart].ingridCollection;
								me.servedClientIdx = newOrderIdxWithTart;

								moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
							}
							// если такого заказа нет, то кладем на стол
							else
							{
								cerr << "КАКОЙ ТО ОЧЕНЬ СЛОЖНЫЙ СЛУЧАЙ" << endl;
								moveAsTank(kitchen.nearFreeCell);
							}
						}
					}
					// если нашли тарлеку с чем подходящим для моего заказа
					else
					{
						cerr << "		есть хорошенькая тарелка с продуктами и повар успеет до нее дойти" << endl;
						moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "в руках тарелка, идем к печке" << endl;
					moveAsTank(kitchen.ovenCell);
				}
				else
				{
					findWherePut(me.ingridsInHands);
				}

				break;
			}

			// если в печке готовый круасанн, надо его забрать
			case CROISSANT:
			{
				cerr << "круасанн готов, надо забрать из печки" << endl;

				// можем забирать из печки, если в руках тарелка без круасана
				// либо если в руках вообще ничего нет
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & CROISSANT)) )
				{
					// сбрасывается флаг использования мной печки
					me.usedOven = false;

					// сброс флага надобности 
					if(me.whatNeedMake == CROISSANT)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// если в руках что то лишнее - выклдываем на стол
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}
				break;
			}

			// если в печке готовый торт, надо его забрать
			case TART:
			{
				cerr << "торт готов, надо забрать из печки" << endl;

				// можем забирать из печки, если в руках тарелка без торта
				// либо если в руках вообще ничего нет
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & TART)) )
				{
					// сбрасывается флаг использования мной печки
					me.usedOven = false;

					// сброс флага надобности 
					if(me.whatNeedMake == TART)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// если в руках что то лишнее - выклдываем на стол
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}

				break;
			}

			// если печка пустая, а флаг me.usedOven == true, то это означает, что свою выпечку я уже забрал
			case NONE:
			{
				findWherePut(me.ingridsInHands);

				// сбрасывается флаг использования мной печки
				me.usedOven = false;
				break;
			}

			default:
				moveAsTank(kitchen.ovenCell);
		}
	}
	// если я еще не начал использовать печку
	else
	{
		cerr << "печка еще не используется" << endl;

		// если руки чем то заняты
		if(me.ingridsInHands != NONE)
		{
			findWherePut(me.ingridsInHands);
		}
		// если в руках ничего нет, надо взять
		else
		{
			// если повар у печки, а в печке готовая выпечка, то надо ее забрать оттуда
			if(me.isOvenNear() && (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT))
			{
				cerr << "повар у духовки, выпечка готова, забираем" << endl;
				moveAsTank(kitchen.ovenCell);
			}
			// если в печке нет ничего готового
			else
			{
				// сначала надо поискать СЫРОЙ ПИРОГ
				// потом поискать РУБЛЕНОЕ ТЕСТО
				// если нет ни СЫРОГО ПИРОГА ни РУБЛЕНОГО ТЕСТА то идем за тестом

				pair<int, int> cellWithRawTart   = kitchen.findNearest(RAW_TART);
				pair<int, int> cellWithChpdDough = kitchen.findNearest(CHPD_DOUGH);
				pair<int, int> cellWithDough     = kitchen.findNearest(DOUGH);

				// если существует сырой торт, идем за ним
				if(isExistCell(cellWithRawTart))
					moveAsTank(cellWithRawTart);
				else if(isExistCell(cellWithChpdDough))
					moveAsTank(cellWithChpdDough);
				else moveAsTank(cellWithDough);
			}
		}
	}
};

// все ингридиенты есть на кухе, надо их собрать
void collectDesired(){

	// если мне еще нужен какой то ингридиент, ищу его на кухне
	if(me.needSomething())
	{
		cerr << "   надо еще что то собрать на кухне" << endl;
		me.takeNeededIngrid();
	}
	// иначе отношу заказ клиенту
	else
	{
		cerr << "   можно относить в окно" << endl;
		moveAsTank(kitchen.winCell);
	}
};

// нет каких то ингридиентов из заказа, надо сделать то, чего нет
// если начинается готовка какого то сложного ингридиента, возвращается true
// если готовить ничего не нужно, возвращается false
bool prepairHardIngrids(){

	cerr << "-------------------------------------" << endl;
	cerr << "подготовка сложных ингридиентов" << endl;

	// если по заказу нужен торт
	if(me.desiredCollection & TART)
	{
		cerr << "	по заказу нужен торт" << endl;

		// если в руках готовый торт, то его надо положить либо на тарелку к чему то, либо на пустой стол
		if(me.ingridsInHands == TART){

			cerr << "у меня в руках готовый торт" << endl;

			findWherePut(TART);
			return true;
		}

		// если в руках уже есть тарелка, то нужно искать только отдельно существующие торты
		if(me.ingridsInHands & DISH)
		{
			cerr << "		в руках есть тарелка" << endl;

			// если в руках нет торта и на кухне нет отдельно существующего торта, то придется делать торт
			if( !(me.ingridsInHands & TART) && !isExistCell(kitchen.getCellWithIngridStrongly(TART)) )
			{
				cerr << "		придется делать торт" << endl;
				prepairTart();
				return true;
			}
			else
			{
				cerr << "		НЕ придется делать торт" << endl;
			}
		}
		// если в руках тарекли нет, то можно искать и торт на тарелке
		else
		{
			cerr << "		в руках нет тарелки" << endl;

			// поиск подходящей тарелки с тортом среди уже существующих продуктов
			int productWithDishAndTart = -1, minAward = 0;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & TART)			// если в продукте есть торт
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// также есть тарелка
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // продукт является частью моего заказа
				    && kitchen.existingProductVec[i].ingridCollection > minAward) // самый ценный
				{
					minAward = kitchen.existingProductVec[i].ingridCollection;
					productWithDishAndTart = i;
				}
			}

			if(!((me.ingridsInHands & TART) && (me.ingridsInHands & DISH)) // в руках нет тарелки с тортом
				 && !isExistCell(kitchen.getCellWithIngridStrongly(TART)) // нет ячейки кухни с ОТДЕЛЬНЫМ тортом
				 && productWithDishAndTart == -1)// если так и не нашли тарелку с тортом и еще чем то
			{
				cerr << "		придется делать торт" << endl;
				prepairTart();
				return true;
			}
			else
			{
				// если есть тарелка с тортом и еще чем то, подоходящим по заказу,
				// то надо проверить, чтобы на тарелке также были и все остальные СЛОЖНЫЕ продукты

				// если по заказу нужен круасаннн, а на тарелке с тортом круасана нет, то надо сделать круасанн
				if(!((me.ingridsInHands & TART) && (me.ingridsInHands & DISH)) // в руках нет тарелка с тортом
						&& productWithDishAndTart != -1 // есть тарелка с тортом на кухне
						&& me.desiredCollection & CROISSANT // по заказу нужен круасанн
						&& !(kitchen.existingProductVec[productWithDishAndTart].ingridCollection & CROISSANT) // тарелка с тортом не содержит круасана
					)
				{
					cerr << "		есть тарелка с тортом, но на ней нет круасана, поэтому делаю круасан" << endl;
					prepairCroissant();
					return true;
				}
				// если по заказу нужна клубничка, а на тарелке с тортом клубнички нет, то надо сделать клубничку
				else if(!((me.ingridsInHands & TART) && (me.ingridsInHands & DISH)) // в руках нет тарелка с тортом
						&& productWithDishAndTart != -1 // есть тарелка с тортом на кухне
						&& me.desiredCollection & CHPD_STRBR // по заказу нужен круасанн
						&& !(kitchen.existingProductVec[productWithDishAndTart].ingridCollection & CHPD_STRBR) // тарелка с тортом не содержит клубнички
					)
				{
					cerr << "		есть тарелка с тортом, но на ней нет клубнички, поэтому делаю клубничку" << endl;
					prepairChpdStrbr();
					return true;
				}
				cerr << "		НЕ придется делать торт" << endl;
			}
		}
	}
	else
	{
		me.whatNeedMake = -1;
		cerr << "	торт по заказу не нужен" << endl;
	}

	// если по заказу нужен круасанн
	if(me.desiredCollection & CROISSANT)
	{
		cerr << "	по заказу нужен круасанн" << endl;

		// если в руках готовый торт, то его надо положить либо на тарелку к чему то, либо на пустой стол
		if(me.ingridsInHands == CROISSANT){

			cerr << "у меня в руках готовый круасанн" << endl;
			findWherePut(CROISSANT);
			return true;
		}

		// если в руках уже есть тарелка, то нужно искать только отдельно существующие круасанны
		if(me.ingridsInHands & DISH)
		{
			cerr << "		в руках есть тарелка" << endl;

			// если в руках нет круасанна и на кухне нет отдельно существующего круасанна, то придется делать круасанн
			if( !(me.ingridsInHands & CROISSANT) && !isExistCell(kitchen.getCellWithIngridStrongly(CROISSANT)) )
			{
				cerr << "		придется делать круасанн" << endl;
				prepairCroissant();
				return true;
			} else
			{
				cerr << "		НЕ придется делать круасанн" << endl;
			}
		}
		// если в руках тарекли нет, то можно искать и круасанн на тарелке
		else
		{
			cerr << "		в руках нет тарелки" << endl;

			// поиск подходящей тарелки с круасанном среди уже существующих продуктов
			int productWithDishAndCrois = -1, minAward = 0;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & CROISSANT)		// если в продукте есть круасанн
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// также есть тарелка
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // продукт является частью моего заказа
					&& kitchen.existingProductVec[i].ingridCollection > minAward) // самый ценный
				{
					//cerr << "		существует тарелка с круасаном и мб еще чем то, подходящим под наш заказ: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
					minAward = kitchen.existingProductVec[i].ingridCollection;
					productWithDishAndCrois = i;
				}
			}

			if(!((me.ingridsInHands & CROISSANT) && (me.ingridsInHands & DISH)) // в руках нет тарелки с круасанном
				 && !isExistCell(kitchen.getCellWithIngridStrongly(CROISSANT)) // нет ячейки кухни с ОТДЕЛЬНЫМ круасанном
				 && productWithDishAndCrois == -1) // если так и не нашли тарелку с круасанном и еще чем то
			{
				cerr << "		придется делать круасанн" << endl;
				prepairCroissant();
				return true;
			} else
			{
				cerr << "		НЕ придется делать круасанн" << endl;

				// если есть тарелка с круасаном и еще чем то, подоходящим по заказу,
				// то надо проверить, чтобы на тарелке также были и все остальные СЛОЖНЫЕ продукты

				// если по заказу нужен торт, а на тарелке с круасаном торта нет, то надо сделать торт
				if(!((me.ingridsInHands & CROISSANT) && (me.ingridsInHands & DISH)) // в руках нет тарелка с круасаном
						&& productWithDishAndCrois != -1 // есть тарелка с круасаном на кухне
						&& me.desiredCollection & TART // по заказу нужен торт
						&& !(kitchen.existingProductVec[productWithDishAndCrois].ingridCollection & TART) // тарелка с круасаном не содержит торта
					)
				{
					cerr << "		есть тарелка с круасаном, но на ней нет торта, поэтому делаю торт" << endl;
					prepairTart();
					return true;
				}
				// если по заказу нужна клубничка, а на тарелке с круасаном клубнички нет, то надо сделать клубничку
				else if(!((me.ingridsInHands & CROISSANT) && (me.ingridsInHands & DISH)) // в руках нет тарелка с круасаном
						&& productWithDishAndCrois != -1 // есть тарелка с круасаном на кухне
						&& me.desiredCollection & CHPD_STRBR // по заказу нужна клубничка
						&& !(kitchen.existingProductVec[productWithDishAndCrois].ingridCollection & CHPD_STRBR) // тарелка с круасаном не содержит клубничку
					)
				{
					cerr << "		есть тарелка с круасаном, но на ней нет порезанной клубнички, поэтому делаю клучбнику" << endl;
					prepairChpdStrbr();
					return true;
				}
			}
		}
	}
	else
	{
		me.whatNeedMake = -1;
		cerr << "	круасанн по заказу не нужен" << endl;
	}

	// если по заказу нужна резаная клубничка
	if(me.desiredCollection & CHPD_STRBR)
	{
		cerr << "	по заказу нужна порезанная клубничка" << endl;

		// если в руках порезанная клубничка, то ее надо положить либо на тарелку к чему то, либо на пустой стол
		if(me.ingridsInHands == CHPD_STRBR){

			cerr << "у меня в руках порезанная клубчника" << endl;

			findWherePut(CHPD_STRBR);
			return true;
		}

		// если в руках уже есть тарелка, то нужно искать только отдельно существующие резанные клубнички
		if(me.ingridsInHands & DISH)
		{
			cerr << "		в руках есть тарелка" << endl;

			// если в руках нет порезанной клубнички и на кухне нет отдельно существующей порезанной клубнички, то придется делать порезанную клубничку
			if(!(me.ingridsInHands & CHPD_STRBR) && !isExistCell(kitchen.getCellWithIngridStrongly(CHPD_STRBR)))
			{
				cerr << "		придется делать порезанную клубничку" << endl;
				prepairChpdStrbr();
				return true;
			} else
			{
				cerr << "		НЕ придется делать порезанную клубничку" << endl;
			}
		}
		// если в руках тарекли нет, то можно искать порезанную клубничку и на тарелке
		else
		{
			cerr << "		в руках нет тарелки" << endl;

			// поиск подходящей тарелки с порезанной клубнички среди уже существующих продуктов
			int productWithDishAndChpadStrbr = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & CHPD_STRBR)	// если в продукте есть порезанная клубничка
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// также есть тарелка
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection))) // продукт является частью моего заказа
				{
					//cerr << "		существует тарелка с порезанной клубничкой и мб еще чем то, подходящим под наш заказ: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
					productWithDishAndChpadStrbr = i;
				}
			}

			if(!((me.ingridsInHands & CHPD_STRBR) && (me.ingridsInHands & DISH)) // в руках нет тарелки с порезанной клубничкой
				 && !isExistCell(kitchen.getCellWithIngridStrongly(CHPD_STRBR)) // нет ячейки кухни с ОТДЕЛЬНОЙ порезанной клубничкой
				 && productWithDishAndChpadStrbr == -1) // если так и не нашли тарелку с порезанной клубничкой и еще чем то
			{
				cerr << "		придется делать порезанную клубничку" << endl;
				prepairChpdStrbr();
				return true;
			} else
			{
				cerr << "		НЕ придется делать порезанную клубничку" << endl;
			}
		}
	}
	else
	{
		me.whatNeedMake = -1;
		cerr << "	порезанная клубничка по заказу не нужен" << endl;
	}

	return false;
};

// изготовление заказа по рецепту
void makeDesiredCollection(){

	cerr << "-------------------------------------" << endl;
	cerr << "начинаю делать заказ" << endl;
	
	// ёсли не требуется приготовление сложные продуктов
	if(!prepairHardIngrids())
		// то переходим к собиранию продуктов
		collectDesired();
}

// заготовки на раунд
void makePrepairings(){

	if(prepChpdStrw < HOW_MUCH_STRWBR_PREP) prepairChpdStrbr();
	else if(prepCrois < HOW_MUCH_CROIS_PREP) prepairCroissant();
	else if(prepTarts < HOW_MUCH_TARTS_PREP) prepairTart();
};

// если приготовления завершились, то можно пытаться собирать рецепты
void makeSomeOrder(){

	// если надо спасать выпечку, то ничего другого пока делать не могу
	if(saveBaking())
		return;

	// если заказ клиента изменился, а в руках моего повара есть тарелка с какими то ингридиентами
	// то надо либо выбрать новый исполняемый заказ, в который входят все ингридиеты с тарелки
	//		   либо, если тарелка с такими ингридиентами не подходит ни для одного из текущих заказов,
	//				 то отнести тарелку в посудомойку

	// если еще не выбран заказ для исполнения, то берем заказ у первого клиента
	if(me.desiredCollection == -1){
		me.servedClientIdx = clients.getCurClientIdxWithMaxAward();

		// если оппонент тоже выбрал первого клиента, то берем заказ у нулевого клиента
		//if(opponent.servedClientIdx == 1)
		//	me.servedClientIdx = 0;

		me.usedOven = false;
		me.whatNeedMake = -1;

		cerr << "заказ не выбран. выбираю " << me.servedClientIdx << "-й" << endl;

		// запоминаем собираемый рецепт и приступаем к выполнению
		me.desiredCollection = clients.curClientVec[me.servedClientIdx].ingridCollection;

		makeDesiredCollection();
	}
	// если есть выбранный заказ
	else
	{
		// если заказ ОБСЛУЖИВАЕМОГО клиента изменился
		// заказ может поменяться, если оппонент выполнил чей то заказ
		// либо если мой повар сдал готовый заказ
		if(clients.curClientVec.at(me.servedClientIdx).ingridCollection != me.desiredCollection)
		{
			// надо найти тот заказ, который выполняет мой повар
			int desiredIdx = -1;
			for(int i = 0; i < clients.curClientNb; i++)
			{
				if(clients.curClientVec[i].ingridCollection == me.desiredCollection)
					desiredIdx = i;
			}

			// если заказа, который собирает мой повар нет, значит напарник сделал именно этот заказ
			if(desiredIdx == -1){

				cerr << "выполняемого заказа больше нет..." << endl;
				me.usedOven = false;
				me.whatNeedMake = -1;

				// если в рука есть тарелка с какими то ингридиентами, то надо либо найти заказ,
				// для которого содержимое моей тарелки подходит,
				// либо, если содержимое тарелки не подходит ни для одного заказа, то помыть ее
				if(me.ingridsInHands & DISH && me.ingridsInHands > DISH){

					// проверяем все текущие заказы на совпадение с теми ингридиентами, что есть у меня в тарелке
					int orderWithSameIngrids = -1;
					for(int i = 0; i < 3; i++)
						if(PRODUCT::is1PartOf2(me.ingridsInHands, clients.curClientVec.at(i).ingridCollection))
							orderWithSameIngrids = i;

					// если для какого то клиента содержимое моей тарелки подходит, делаем этого клиента основным
					if(orderWithSameIngrids != -1){
						cerr << "мои ингридиенты подходят для заказа " << orderWithSameIngrids << endl;
						me.servedClientIdx = orderWithSameIngrids;
						me.desiredCollection = clients.curClientVec.at(me.servedClientIdx).ingridCollection;
						makeDesiredCollection();
					}
					// если нет клиента, которому подошли бы мои ингридиенты, моем тарелку
					else
					{
						cerr << "мои ингридиенты не подходят ни для одного заказа" << endl;
						// это может быть какой либо сырой ингридиент (клубничка/тесто/порезанное тесто/сырой пирог)

						// если это клубничка/теста, надопорезать
						if(me.ingridsInHands == STRBR || me.ingridsInHands == DOUGH) moveAsTank(kitchen.chopCell);

						// если это резаное тесто, делаем из него сырой пирог
						else if(me.ingridsInHands == CHPD_DOUGH) moveAsTank(kitchen.blbrCell);

						// если это сырой пирог, кладем его в духовку
						else if(me.ingridsInHands == RAW_TART) moveAsTank(kitchen.ovenCell);

						else moveAsTank(kitchen.dishCell);
					}
				}
				// если в руках (нет тарелки) / (пустая тарелка), то надо выбрать новый заказ
				else
				{
					// если у напарника в руках (ничего нет) / (пустая тарелка), то можно выбрать любой заказ
					if(opponent.ingridsInHands <= DISH){

						// пускай это будет заказ первого клиента
						me.servedClientIdx = clients.getCurClientIdxWithMaxAward();
						me.desiredCollection = clients.curClientVec[me.servedClientIdx].ingridCollection;
						cerr << "у напарника нет ничего, берем самый дорогой заказ: " << me.servedClientIdx << endl;
						makeDesiredCollection();
					}
					// если у напарника есть что то в руках, пытаемся оценить что он делает и выбираем любой другой заказ
					else
					{
						estimateOpponentOrderIdx();
						cerr << "у напарника заказ " << opponent.servedClientIdx << ", берем следующий за ним" << endl;
						me.servedClientIdx = (opponent.servedClientIdx + 1) % clients.curClientNb;
						me.desiredCollection = clients.curClientVec[me.servedClientIdx].ingridCollection;
						makeDesiredCollection();
					}
				}
			}
			// если заказ, который выполняет мой повар все еще есть, то запоминаем этого клиента и продолжаем
			else
			{
				cerr << "заказ переместился, теперь он имеет индекс " << desiredIdx << endl;
				me.servedClientIdx = desiredIdx;
				makeDesiredCollection();
			}
		}
		// если заказ не изменился, делаем все по привычной схеме
		else
		{
			cerr << "заказ не изменился, делаю его же " << me.servedClientIdx << endl;
			makeDesiredCollection();
		}
	}
};

// работаем
void makeSomething(){

	// если надо сделать заготовки на раунд
	if(needPrepairing)
	{
		cerr << "надо сделать заготовки" << endl;
		makePrepairings();
	}
	// если все заготовки были сделаны - собираем заказы
	else
	{
		cerr << "надо делать заказы" << endl;
		makeSomeOrder();
	}
};

int main()
{
	std::srand(unsigned(std::time(0)));

	// все заказы на раунд
	clients.readAllClientsInput();

	// инициализация кухни
	kitchen.initialize();

	while(1){

		cin >> turnsRemaining;

		auto begin = std::chrono::high_resolution_clock::now();

		readInput();

		makeSomething();

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> fp_ms = end - begin;
		//cerr << "time: " << fp_ms.count() << " ms\n";
	}
}
