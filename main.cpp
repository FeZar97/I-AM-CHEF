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
	CHPD_STRBR = 0x0010,		// разделенная клубничка
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
		//cerr << "on " << x << " " << y << " exist " << ingrid2String(ingridCollection) << endl;
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

void use(kitchenCellType);
void use(pair<int,int>);
void use(string);

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

	// кухня размером 7х11
	vector<vector<kitchenCellType>> cellArray;

	// массив расстояний до всех мест кухни относительно МОЕГО повара
	short distArrayForMe[K_HEIGHT][K_WIDTH];

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
	pair<int, int> getNearFreeTable(int x, int y, int oppX, int oppY){

		// проверяются 8 окружающих клеток
		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				// если это стол (возможно с каким то продуктом, оставленным ранее)
				if(cellArray[i][j] == SOME_PRODUCT){

					bool isTableFree = true;
					// поиск среди ранее оставленных ингридиентов existingProductVec ячейки i j
					// если в оставленных ингридиентах такой ячейки нет, то на нее можно что то положить
					for(auto &ingrs: existingProductVec)
						if(ingrs.x == j && ingrs.y == i)
							isTableFree = false;

					if(isTableFree)
					{
						//cerr << "cell " << j << " " << i << " is free" << endl;
						return pair<int, int>{j, i};
					}
				}

		//cerr << "not exist free cells in area8, find more..." << endl;

		// если дошли до сюда, значит в ближайших 8 клетках от повара нет свободных клеток
		// надо найти все проходные ячейки вокруг ячейки (x,y) и для каждой из них попытаться найти свободный стол в окружении 8 клеток

		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
			{
				// если это проходная ячейка и на ней нет напарника
				// то проверяем все ее окружение
				if(cellArray[i][j] == EMPTY && !(oppX == j && oppY == i)){

					// проверяются 8 окружающих клеток около найденной свободной клетки
					for(int k = i - 1; k <= i + 1; k++)
						for(int l = j - 1; l <= j + 1; l++)
							// если это стол (возможно с каким то продуктом, оставленным ранее)
							if(cellArray[k][l] == SOME_PRODUCT){

								bool isTableFree = true;
								// поиск среди ранее оставленных ингридиентов existingProductVec ячейки i j
								// если в оставленных ингридиентах такой ячейки нет, то на нее можно что то положить
								for(auto &ingrs : existingProductVec)
									if(ingrs.x == l && ingrs.y == k) isTableFree = false;

								if(isTableFree) return pair<int, int>{l, k};
							}
				}
			}

		return pair<int, int>{-1, -1};
	};

	// если на кухне есть все ингридиенты, возвращает true,
	// если хотя бы одного ингридиента на кухне нет, то возвращает false
	bool existAllHardIngrids(short desiredCollection, short ingridsInHands){

		// если по заказу нужен круасанн
		if(desiredCollection & CROISSANT)
		{
			cerr << "нужен круасанн" << endl;
			// если его нет ни в руках, ни отдельно лежащего и нет тарелки с круассаном(при этом остальные ингридиенты тоже должны подходить)
			
			int productWithDishAndCrois = -1;

			// поиск подходящей тарелки с круасанном среди уже существующих продуктов
			for(int i = 0; i < existingProductNb; i++)
			{
				if((existingProductVec[i].ingridCollection & CROISSANT)
				   && (existingProductVec[i].ingridCollection & DISH)
				   && (PRODUCT::is1PartOf2(existingProductVec[i].ingridCollection, desiredCollection)))
				{
					cerr << "существует тарелка с круасаном и мб еще чем то, подходящим под наш заказ: " << existingProductVec[i].x << " " << existingProductVec[i].y << endl;
					productWithDishAndCrois = i;
				}
			}

			if(!((ingridsInHands & CROISSANT) && (ingridsInHands & DISH)) // в руках нет тарелки с круасаном
				 && !isExistCell(getCellWithIngridStrongly(CROISSANT)) // нет ячейки кухни с ОТДЕЛЬНЫМ круасаном
				 && productWithDishAndCrois == -1)// если так и не нашли тарелку с круасаном и еще чем то
			{
				return false;
			}
		}

		// если (по заказу нужна CHPD_STRBR) а (на кухне ее нет) и (в руках ее нет НА ТАРЕЛКЕ)
		if( (desiredCollection & CHPD_STRBR) && (!isExistCell(getCellWithIngridStrongly(CHPD_STRBR))) && ( !((ingridsInHands & CHPD_STRBR) && (ingridsInHands & DISH))) )
		{
			cerr << "нужна клубничка" << endl;
			return false;
		}

		// если по заказу нужен TART а на кухне его нет и в руках его нет НА ТАРЕЛКЕ
		if((desiredCollection & TART) && (!isExistCell(getCellWithIngridStrongly(TART))) && ( !((ingridsInHands & TART) && (ingridsInHands & DISH))) )
		{
			cerr << "нужен торт" << endl;
			return false;
		}

		return true;
	};

	// ближайшая к моему повару свободная ячейка стола
	pair<int, int> nearFreeCell = {-1, -1};

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
				use(kitchen.nearFreeCell);
				return;
			}
			else
			{
				// ищем тарелки среи уже существующий на кухне продуктов
				for(auto &product : kitchen.existingProductVec)
					if(PRODUCT::is1PartOf2(product.ingridCollection, desiredCollection) && !PRODUCT::is1PartOf2(product.ingridCollection, ingridsInHands) && (product.ingridCollection & DISH))
					{
						cerr << "есть нужный продукт с тарелкой" << endl;
						use(pair<int, int>{product.x, product.y});
						return;
					}

				cerr << "нет ничего подходящего, берем из посудомойки" << endl;
				// если не нашли в существующих, идем к посудомойке
				use(DISHWASH);
				return;
			}
		}
		// если тарелка в руках есть, то можем собирать только те продукты, которые без тарелок
		else
		{
			// ищем самый ближайший и идем его брать

			int minDist = 777, minProductIdx = -1;

			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				// если существующий продукт является частью рецепта, ингридиентов из существующего продукта еще нет в руках и продукта не содержит тарелку
				if((kitchen.existingProductVec[i].ingridCollection & desiredCollection) && !(kitchen.existingProductVec[i].ingridCollection & ingridsInHands) && !(kitchen.existingProductVec[i].ingridCollection & DISH))
				{
					if(kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] < minDist){

						minDist = kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x];
						minProductIdx = i;
					}
				}
			}
			
			if(minProductIdx != -1)
			{
				cerr << "есть продукт, являющийся часть рецепта..." << endl;
				use(pair<int, int>{kitchen.existingProductVec[minProductIdx].x, kitchen.existingProductVec[minProductIdx].y});
				return;
			}

			/*
			for(auto &product : kitchen.existingProductVec)
				// если существующий продукт является частью рецепта, ингридиентов из существующего продукта еще нет в руках и продукта не содержит тарелку
				if((product.ingridCollection & desiredCollection) && !(product.ingridCollection & ingridsInHands) && !(product.ingridCollection & DISH))
				{
					cerr << product.ingridCollection << " " << desiredCollection << " " << ingridsInHands << endl;
					cerr << "есть продукт, являющийся часть рецепта..." << endl;
					use(pair<int, int>{product.x, product.y});
					return;
				}
				*/
		}

		// если дошли до сюда, это означает что осталось взять только ингридиенты со спавнов: черничка/мороженка
		if((BLBR & desiredCollection) && !(BLBR & ingridsInHands))
		{
			cerr << "берем черничку" << endl;
			use(BLBR_CREATE);
		}
		else
		{
			if((ICE_C & desiredCollection) && !(ICE_C & ingridsInHands))
			{
				cerr << "берем мороженку" << endl;
				use(ICE_CREATE);
			}
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

// расчет расстояние от точки моего повара до всех клеток кухни
void calcDistances(){

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
				if( (kitchen.distArrayForMe[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY) )
				{
					//cerr << "(" << j << "," << i << ") = " << kitchen.distArrayForMe[i][j] << endl;

					// обновляются только смежные4 ячейки
					// если расстояние до ячейки еще не известно, она проходная и в ней нет оппонента, то обновляем

					// левая
					if( (kitchen.distArrayForMe[i][j - 1] > 333) && (kitchen.cellArray[i][j - 1] == EMPTY) && !(i == opponent.y && (j - 1) == opponent.x) ){
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
					if(kitchen.distArrayForMe[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY && !( (i + 1) == opponent.y && j == opponent.x)){
						kitchen.distArrayForMe[i + 1][j] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	4. (" << j << "," << i + 1 << ") = " << kitchen.distArrayForMe[i + 1][j] << endl;
					}
				}
		/*
		cerr << "расстояния до проходных ячеек:" << endl;
		for(i = 0; i < K_HEIGHT; i++)
		{
			for(j = 0; j < K_WIDTH; j++)
			{
				cerr.width(4);
				cerr << kitchen.distArrayForMe[i][j] << " ";
			}
			cerr << endl;
		}
		*/
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

		needPrepairing = PREPAIRING_FLAG;
		me.usedOven = false;
		prepChpdStrw = 0;
		prepCrois = 0;
		prepTarts = 0;
	}

	// если все заготовки были сделаны
	if(needPrepairing && prepChpdStrw == 2 && prepCrois == 1) needPrepairing = false;

	me.read();
	opponent.read();

	// существующие готовые ингридиенты на кухне
	cin >> kitchen.existingProductNb;
	kitchen.existingProductVec.clear();
	kitchen.existingProductVec.resize(kitchen.existingProductNb);
	for(auto &prod: kitchen.existingProductVec) prod.readInput();

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
	kitchen.nearFreeCell = kitchen.getNearFreeTable(me.x, me.y, opponent.x, opponent.y);
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

void use(string command){
	cout << command + "; " + get_random_spell() << endl;
	cerr << command << endl;
};

// создание на кухне порезанной клубнички
void prepairChpdStrbr(){

	cerr << "готовлю клубничку" << endl;

	// если руки пустые, берем цельную клубничку
	if(me.ingridsInHands == NONE) use(STRBR_CREATE);
	// если в руках цельная клубничка, режем ее
	else if(me.ingridsInHands == STRBR) use(CHOP_BOARD);
	// если в руках не цельная клубничка, то ставим это на стол
	else
	{
		// если в руках порезанная клубничка, то увеличиваем счетчик приготовленных клубничек
		if(me.ingridsInHands == CHPD_STRBR) prepChpdStrw++;
		use(kitchen.nearFreeCell);
	};
};

// создание на кухне круасанна
void prepairCroissant(){

	// если я уже использовал печку, то надо либо ждать приготовления круасанна, либо забрать круасан если он готов
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

				pair<int, int> dishCell = kitchen.findNearest(DISH);

				if(me.ingridsInHands == NONE)
				{
					cerr << "в руках ничего нет" << endl;

					int minAward = 0;
					int prodIdx = -1;

					// надо найти тарелку с подходящими ингридиентами, или, если такой нет, взять чистую
					for(int i = 0; i < kitchen.existingProductNb; i++)
					{
						if((PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // если это часть моего заказа
						   && (kitchen.existingProductVec[i].ingridCollection & DISH)  // если продукт содержит тарелку
						   && (kitchen.existingProductVec[i].ingridCollection > minAward)) // самый ценный продукт
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// если нашли тарлеку с чем то годным
					if(prodIdx != -1)
					{
						cerr << "есть хорошенькая тарелка с продуктами..." << endl;
						use(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
					// если не нашли тарелку с чем то подходящим, берем чистую
					else
						use(dishCell);

					return;
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "в руках тарелка, идем к печке" << endl;
					use(OVEN);
					return;
				} else
				{
					use(kitchen.nearFreeCell);
				}

				break;
			}
				// если в печке готовый продукт, надо его забрать
			case CROISSANT:
			case TART:
				cerr << "выпечка готова, надо забрать" << endl;
				// если в руках нет тарелки / на тарелке уже есть круасанн
				if( !(me.ingridsInHands & DISH) || (me.ingridsInHands & CROISSANT) )
					use(kitchen.nearFreeCell);
				// если руки пустые, то надо забрать выпечку
				else
					use(OVEN);
				
				break;

				// если печка пустая, а флаг me.usedOven == true, то это означает, что свой круасанн я уже забрал
			case NONE:
				cerr << "выпечка в руках, кладем ее на стол" << endl;
				// если в руках приготовленный круасанн, то счетчик приготовленных круасаннов увлеичивается
				if(me.ingridsInHands == CROISSANT) ++prepCrois;
				// если в руках приготовленный торт, то счетчик приготовленных круасаннов увлеичивается
				else if(me.ingridsInHands == TART) ++prepTarts;

				// сбрасывается флаг использования мной печки
				me.usedOven = false;
				use(kitchen.nearFreeCell);
				break;

			default:
				use(OVEN);
		}
	}
	// если я еще не начал использовать печку
	else
	{
		cerr << "печка еще не используется" << endl;

		// если сырье в печку еще не было заложено, то надо найти это сырье и положить в печку

		// если руки чем то заняты
		if(me.ingridsInHands != NONE)
		{
			// если в руках уже есть тесто, то надо нести к печке
			if(me.ingridsInHands == DOUGH)
			{
				cerr << "в руках есть тесто" << endl;
				// если повар около печки
				if(me.isOvenNear())
				{
					cerr << "повар около печки" << endl;
					// если духовка свободна, а в руках продукт, который можно испечь, то мой повар ее занимает
					if(kitchen.ovenContents == NONE && (me.ingridsInHands == DOUGH || me.ingridsInHands == RAW_TART))
					{
						cerr << "загружаю печку, начинаю готовку" << endl;
						me.usedOven = true;
						use(OVEN);
					}
					// если в руках что то есть, то выкладываем рядом
					else
						use(kitchen.nearFreeCell);
				}
				// если не у духовки
				else
				{
					cerr << "повар не у печки, идем к печке" << endl;
					use(OVEN);
				}
			}
			// если в руках что то другое - выкладываем на свобоный стол
			else
			{
				cerr << "в руках что то лишнее..." << endl;
				// если вытащили круасан, увеличиваем счетчик
				if(me.ingridsInHands == CROISSANT) ++prepCrois;
				// если вытащили круасан, увеличиваем счетчик
				else if(me.ingridsInHands == TART) ++prepTarts;
				use(kitchen.nearFreeCell);
			}
		}
		// если в руках ничего нет, надо взять ТЕСТО
		else
		{
			// если повар у печки, а в печке готовая выпечка, то надо ее забрать оттуда
			if(me.isOvenNear() && (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT))
			{
				cerr << "повар у духовки, выпечка готова, забираем" << endl;
				use(OVEN);
			}
			// если в печке нет ничего готового
			else
			{
				// ищем существующее тесто / спавнер теста
				use(kitchen.findNearest(DOUGH));
			}
		}
	}
};

// создание на кухне торта
void prepairTart(){

	// если я уже использовал духовку, то надо либо ждать приготовления торта, либо забрать торт если он готов
	if(me.usedOven)
	{
		cerr << "печка уже используется" << endl;

		pair<int, int> dishCell = kitchen.findNearest(DISH);

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
						   && (kitchen.existingProductVec[i].ingridCollection > minAward)  ) // самый ценный продукт
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// если нашли тарлеку с чем то годным
					if(prodIdx != -1)
					{
						cerr << "есть хорошенькая тарелка с продуктами..." << endl;
						use(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
					// если не нашли тарелку с чем то подходящим, берем чистую
					else
						use(dishCell);

					return;
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "в руках тарелка, идем к печке" << endl;
					use(OVEN);
					return;
				} else
				{
					use(kitchen.nearFreeCell);
					return;
				}

				break;
			}

			// если в печке готовый продукт, надо его забрать
			case CROISSANT:
			case TART:
			{
				cerr << "выпечка готова, надо забрать" << endl;
				// если в руках есть тарелка с чем то, и на тарелке уже есть торт
				// либо если в руках что то круче тарелки
				if(me.ingridsInHands > DISH && me.ingridsInHands & TART)
					use(kitchen.nearFreeCell);
				// если руки пустые, то надо забрать выпечку
				else
					use(OVEN);

				break;
			}
			// если печка пустая, а флаг me.usedOven == true, то это означает, что свой торт я уже забрал
			case NONE:
				cerr << "выпечка в руках, кладем ее на стол" << endl;
				// если в руках приготовленный торт, то счетчик приготовленных тортов увлеичивается
				if(me.ingridsInHands == CROISSANT) ++prepCrois;
				// если в руках приготовленный торт, то счетчик приготовленных круасаннов увлеичивается
				else if(me.ingridsInHands == TART) ++prepTarts;

				// сбрасывается флаг использования мной печки
				me.usedOven = false;
				use(kitchen.nearFreeCell);
				break;

			default:
				use(OVEN);
		}
	}
	// если я еще не начал использовать печку
	else
	{
		cerr << "печка еще не используется" << endl;
		// если сырье в печку еще не было заложено, то надо найти это сырье и положить в печку

		// если руки чем то заняты
		if(me.ingridsInHands != NONE){

			switch(me.ingridsInHands){
				
				// если в руках тесто - несем на резку
				case DOUGH:
					use(CHOP_BOARD);
					break;

				// если в руках рубленое тесто, надо его заправить
				case CHPD_DOUGH: 
					use(BLBR_CREATE);
					break;

				// если сырой торт
				case RAW_TART:
					// если повар у печки
					if(me.isOvenNear())
					{
						cerr << "повар у печки" << endl;
						// если духовка свободна, то можно заложить
						if(kitchen.ovenContents == NONE)
						{
							cerr << "загружаю печку, начинаю готовку" << endl;
							me.usedOven = true;
							use(OVEN);
						}
						// если печка занята
						else
						{
							cerr << "печка занята, в руках сырой торт, выкладываю рядом" << endl;
							use(kitchen.nearFreeCell);
						}
					}
					// если повар не у духовки - то идем к духовке
					else
					{
						cerr << "повар не у печки, идем к печке" << endl;
						use(OVEN);
					}
					break;

				case CROISSANT:
					prepCrois++;
					use(kitchen.nearFreeCell);
					break;

				case TART:
					prepTarts++;
					use(kitchen.nearFreeCell);
					break;

				default:
					use(kitchen.nearFreeCell);
			}
		}
		// если в руках ничего нет, надо взять
		else
		{
			// если повар у печки, а в печке готовая выпечка, то надо ее забрать оттуда
			if(me.isOvenNear() && (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT))
			{
				cerr << "повар у духовки, выпечка готова, забираем" << endl;
				use(OVEN);
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
					use(cellWithRawTart);
				else if(isExistCell(cellWithChpdDough))
					use(cellWithChpdDough);
				else use(cellWithDough);
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
		use(WINDOW);
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

		// если в руках уже есть тарелка, то нужно искать только отдельно существующие торты
		if(me.ingridsInHands & DISH)
		{
			cerr << "		в руках есть тарелка" << endl;

			// если в руках нет торта и на кухне нет отдельно существующего торта, то придется делать торта
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
			int productWithDishAndTart = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & TART)			// если в продукте есть торт
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// также есть тарелка
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection))) // продукт является частью моего заказа
				{
					//cerr << "		существует тарелка с круасаном и мб еще чем то, подходящим под наш заказ: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
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
			} else
			{
				cerr << "		НЕ придется делать торт" << endl;
			}
		}
	}
	else
	{
		cerr << "	торт по заказу не нужен" << endl;
	}


	// если по заказу нужен круасанн
	if(me.desiredCollection & CROISSANT)
	{
		cerr << "	по заказу нужен круасанн" << endl;

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
			int productWithDishAndCrois = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & CROISSANT)		// если в продукте есть круасанн
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// также есть тарелка
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection))) // продукт является частью моего заказа
				{
					//cerr << "		существует тарелка с круасаном и мб еще чем то, подходящим под наш заказ: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
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
			}
		}
	} else
	{
		cerr << "	круасанн по заказу не нужен" << endl;
	}

	// если по заказу нужна резаная клубничка
	if(me.desiredCollection & CHPD_STRBR)
	{
		cerr << "	по заказу нужна порезанная клубничка" << endl;

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
	} else
	{
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
			for(int i = 0; i < 3; i++)
				if(clients.curClientVec[i].ingridCollection == me.desiredCollection)
					desiredIdx = i;

			// если заказа, который собирает мой повар нет, значит напарник сделал именно этот заказ
			if(desiredIdx == -1){

				cerr << "выполняемого заказа больше нет..." << endl;

				// если в рука есть тарелка с какими то ингридиентами, то надо либо найти заказ,
				// для которого содержимое моей тарелки подходит,
				// либо, если содержимое тарелки не подходит ни для одного заказа, то помыть ее
				if(me.ingridsInHands > DISH){

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
						if(me.ingridsInHands == STRBR || me.ingridsInHands == DOUGH) use(CHOP_BOARD);

						// если это резаное тесто, делаем из него сырой пирог
						else if(me.ingridsInHands == CHPD_DOUGH) use(BLBR_CREATE);

						// если это сырой пирог, кладем его в духовку
						else if(me.ingridsInHands == RAW_TART) use(OVEN);

						else use(DISHWASH);
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
