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

// ���� ��� ������� ��������� �� ����� ������ �� ������
short distanceField[K_HEIGHT][K_WIDTH];

// ����� �����
short **distFromMeToProducts;

// ������ ������������
enum INGRID{
	NONE       = 0x0000,		// ������ ���� / ������ ����
	DISH       = 0x0001,		// ��������
	BLBR       = 0x0002,		// ��������
	ICE_C	   = 0x0004,		// ���������
	STRBR	   = 0x0008,		// ���������
	CHPD_STRBR = 0x0010,		// ����������� ���������
	DOUGH	   = 0x0020,		// �����
	CROISSANT  = 0x0040,		// ��������
	CHPD_DOUGH = 0x0080,		// �������� �����
	RAW_TART   = 0x0082,		// ����� �����
	TART	   = 0x0100,		// ������� ����

	// ������� WOOD 3
	RECIPE1	   = DISH + ICE_C + BLBR,			// ��������� + ��������
	RECIPE2	   = DISH + ICE_C + CHPD_STRBR,		// ��������� + ���������
	RECIPE3    = DISH + BLBR  + CHPD_STRBR,		// ��������  + ���������

	// ������� WOOD 2
	RECIPE4	   = RECIPE1 + CHPD_STRBR,			// ��������� + �������� + ���������

	// ������� WOOD 1
	RECIPE5	   = DISH + CROISSANT + ICE_C,		// �������� + ���������
	RECIPE6	   = DISH + CROISSANT + BLBR,		// �������� + ��������
	RECIPE7	   = DISH + CROISSANT + CHPD_STRBR,	// �������� + ���������
	RECIPE8	   = RECIPE1 + CROISSANT,			// �������� + �������� + ���������
	RECIPE9	   = RECIPE3 + CROISSANT,			// �������� + ��������� + ��������
	RECIPE10   = RECIPE2 + CROISSANT,			// �������� + ��������� + ���������
	RECIPE11   = RECIPE4 + CROISSANT,			// �������� + ��������� + ��������� + ��������

	// ������� BRONZE
	RECIPE12   = DISH + TART + ICE_C,			// ���� + ���������
	RECIPE13   = DISH + TART + BLBR,			// ���� + ��������
	RECIPE14   = DISH + TART + CHPD_STRBR,		// ���� + ���������
	RECIPE15   = DISH + TART + CROISSANT,		// ���� + ��������
	RECIPE16   = TART + RECIPE5,				// ���� + �������� + ���������
	RECIPE17   = TART + RECIPE6,				// ���� + �������� + ��������
	RECIPE18   = TART + RECIPE7,				// ���� + �������� + ���������
	RECIPE19   = TART + RECIPE8,				// ���� + �������� + �������� + ���������
	RECIPE20   = TART + RECIPE9,				// ���� + �������� + ��������� + ��������
	RECIPE21   = TART + RECIPE10,				// ���� + �������� + ��������� + ���������

};

// �������� ������ �����
enum kitchenCellType{
	EMPTY		 = 0x0000, // ������
	SOME_PRODUCT = 0x0001, // �������, � �.�. ����������
	DISHWASH	 = 0X0002, // ����� �������
	WINDOW		 = 0x0004, // ����� � ������
	BLBR_CREATE	 = 0x0008, // ����� ��������
	ICE_CREATE	 = 0x0010, // ����� ���������
	STRBR_CREATE = 0x0020, // ����� ���������
	CHOP_BOARD	 = 0x0040, // ����� ��� �����
	DOUGH_CREATE = 0x0080, // ����� �����
	OVEN		 = 0x0100  // �����
};

// ������� (�� ����� / � �����)
class PRODUCT{
public:
	int x;
	int y;
	short ingridCollection;

	// ���������� �������
	void readInput(){
		string productString;
		cin >> x >> y >> productString;
		ingridCollection = splitProduct2Ingrid(productString);
		//cerr << "on " << x << " " << y << " exist " << ingrid2String(ingridCollection) << endl;
	}

	// �������������� ������ � ��������� �����������
	static short splitProduct2Ingrid(string productString){

		short result = 0x0;
		size_t hyphPos;

		while(!productString.empty()){

			if(productString.substr(0, 2) == "NO") result |= 0x0000;	// ������ ���� / ������ ����
			if(productString.substr(0, 2) == "DI") result |= 0x0001;	// ��������
			if(productString.substr(0, 2) == "BL") result |= 0x0002;	// ��������
			if(productString.substr(0, 2) == "IC") result |= 0x0004;	// ���������
			if(productString.substr(0, 2) == "ST") result |= 0x0008;	// ���������
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "ST") result |= 0x0010;	// ����������� ���������
			if(productString.substr(0, 2) == "DO") result |= 0x0020;	// �����
			if(productString.substr(0, 2) == "CR") result |= 0x0040;	// ��������
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "DO") result |= 0x0080;	// �������� �����
			if(productString.substr(0, 2) == "RA") result |= 0x0082;	// ����� �����
			if(productString.substr(0, 2) == "TA") result |= 0x0100;	// ������� �����

			hyphPos = productString.find("-");
			if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
			else break;
		}
		return result;
	};

	// ���������� �� ��������� part � ��������� product
	// ���� � part ���� ���� �� ���� ������ ����������, �� ������������ ����
	static bool is1PartOf2(short part, short product){
		return (!((part & product) ^ part));
	};

	// ������� ������������ � ������
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

// �������� ��������� �� ����������
bool isExistCell(pair<int, int> p){
	return p.first != -1 && p.second != -1;
};

void use(kitchenCellType);
void use(pair<int,int>);
void use(string);
void useIngrid(short);

// �����
class KITCHEN{
public:

	// ���������� ��������� / ���������
	pair<int, int> ovenCell{-1,-1};
	pair<int, int> chopCell{-1,-1};
	pair<int, int> dishCell{-1,-1};
	pair<int, int> winCell{-1,-1};
	pair<int, int> doughCell{-1,-1};
	pair<int, int> strbrCell{-1,-1};
	pair<int, int> blbrCell{-1,-1};
	pair<int, int> iceCell{-1,-1};

	// ����� �������� 7�11
	vector<vector<kitchenCellType>> cellArray;

	// ������������� �����
	void initialize(){

		string kitchenStroke;
		cellArray.resize(K_HEIGHT);

		for(int i = 0; i < K_HEIGHT; i++){

			cin >> kitchenStroke;
			cellArray[i].resize(K_WIDTH);

			for(int j = 0; j < K_WIDTH; j++){

				// �� ��������� - ������������ ����
				distanceField[i][j] = -1;

				switch(int(kitchenStroke[j])){
					case 48: // ����� 0
					case 49: // ����� 1
					case 50: // ����� 2
					case 46: cellArray[i][j] = EMPTY; distanceField[i][j] = 0;		break; // . ������
					case 68: cellArray[i][j] = DISHWASH;		dishCell  = {j, i};	break; // D ����� �������
					case 87: cellArray[i][j] = WINDOW;			winCell	  = {j, i};	break; // W ����� � ������
					case 66: cellArray[i][j] = BLBR_CREATE;		blbrCell  = {j, i};	break; // B ����� ��������
					case 73: cellArray[i][j] = ICE_CREATE;		iceCell   = {j, i};	break; // I ����� ���������
					case 83: cellArray[i][j] = STRBR_CREATE;	strbrCell = {j, i};	break; // S ����� ���������
					case 67: cellArray[i][j] = CHOP_BOARD;		chopCell  = {j, i};	break; // C ����� ��� �����
					case 72: cellArray[i][j] = DOUGH_CREATE;	doughCell = {j, i};	break; // H ����� �����
					case 79: cellArray[i][j] = OVEN;			ovenCell  = {j, i};	break; // O �����
					default: cellArray[i][j] = SOME_PRODUCT;							   // ������� / ������ ����
				}

			}
		}
	};

	// ������� ������������ � ������
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

	// ���������� �� ����� �� ���������� ��������
	int existingProductNb;
	vector<PRODUCT> existingProductVec;

	// ����� ������������
	pair<int, int> getCellWithAppliance(kitchenCellType desiredCell){
		for(int i = 0; i < K_HEIGHT; i++)
			for(int j = 0; j < K_WIDTH; j++)
				if(cellArray[i][j] == desiredCell) return pair<int, int>{j, i};
		return pair<int, int>{-1, -1};
	};

	// �� ������� ����� ������������
	pair<int, int> getCellWithIngrid(short ingrid){
		for(auto &p : existingProductVec)
			// ���� ������ ����� �������� ingrid (�������� ��� �� ��� ����� ingrid)
			if(p.ingridCollection & ingrid) return pair<int, int>{p.x, p.y};
		return pair<int, int>{-1, -1};
	};

	// ������� ����� ������������
	pair<int, int> getCellWithIngridStrongly(short ingrid){
		// ���� ������ ����� �������� � �������� ��, ��� ���������� � ������ ������
		for(auto &p: existingProductVec)
			if(p.ingridCollection == ingrid)
			{
				//cerr << "CELL " << p.x << " " << p.y << " contains " << PRODUCT::ingrid2String(ingrid) << endl;
				return pair<int, int>{p.x, p.y};
			}
		return pair<int, int>{-1, -1};
	};
	
	// ���������� ���������� �����, ���������� � ���������� �����������
	pair<int, int> getNearFreeTable(int x, int y){

		// ����������� 8 ���������� ������
		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				// ���� ��� ���� (�������� � ����� �� ���������, ����������� �����)
				if(cellArray[i][j] == SOME_PRODUCT){

					bool isTableFree = true;
					// ����� ����� ����� ����������� ������������ existingProductVec ������ i j
					// ���� � ����������� ������������ ����� ������ ���, �� �� ��� ����� ��� �� ��������
					for(auto &ingrs: existingProductVec)
						if(ingrs.x == j && ingrs.y == i) isTableFree = false;

					if(isTableFree) return pair<int, int>{j, i};
				}
		return pair<int, int>{-1, -1};
	};

	// ���� �� ����� ���� ��� �����������, ���������� true,
	// ���� ���� �� ������ ����������� �� ����� ���, �� ���������� false
	// �������� ������� ������ ���������� ���������, ��������� � �����
	bool existAllHardIngrids(short order, short ingridsInHands){

		// ���� (�� ������ ����� CHPD_STRBR) � (�� ����� �� ���) � (� ����� �� ��� �� �������)
		if( (order & CHPD_STRBR) && (!isExistCell(getCellWithIngridStrongly(CHPD_STRBR))) && ( !((ingridsInHands & CHPD_STRBR) && (ingridsInHands & DISH))) )
		{
			cerr << "NOT EXIST IN KITCHEN " + PRODUCT::ingrid2String(CHPD_STRBR) << endl;
			return false;
		}

		// ���� �� ������ ����� CROISSANT � �� ����� ��� ��� � � ����� ��� ��� �� �������
		if((order & CROISSANT) && (!isExistCell(getCellWithIngridStrongly(CROISSANT))) && ( !((ingridsInHands & CROISSANT) && (ingridsInHands & DISH))) )
		{
			cerr << "NOT EXIST IN KITCHEN " + PRODUCT::ingrid2String(CROISSANT) << endl;
			return false;
		}

		// ���� �� ������ ����� TART � �� ����� ��� ��� � � ����� ��� ��� �� �������
		if((order & TART) && (!isExistCell(getCellWithIngridStrongly(TART))) && ( !((ingridsInHands & TART) && (ingridsInHands & DISH))) )
		{
			cerr << "NOT EXIST IN KITCHEN " + PRODUCT::ingrid2String(TART) << endl;
			return false;
		}
		
		//cerr << "ALL HARD INGRID EXIST IN KITCHEN" << endl;
		return true;
	};
} kitchen;

// ������� ������� � ��� ����������� �������
class CLIENTS{
public:
	// ������ = {�����, �����}
	class CLIENT{
	public:
		short ingridCollection;
		int price;

		// ���������� �������
		void readInput(){
			string productString;
			cin >> productString >> price;
			ingridCollection = PRODUCT::splitProduct2Ingrid(productString);
		}
	};

	// ������� �� ���� ����
	int allClientNb;
	vector<CLIENT> allClientVec;

	// ������� �������
	int curClientNb;
	vector<CLIENT> curClientVec;
} clients;

// ������ ���������� �� ����� startCoords �� ���� ��������� ��������� ������
void calcDistances(short **distField, pair<int, int> startCoords){
	
	// nonChekedCellsNb - ���������� ������������� ��������� �����
	int i, j, nonChekedCellsNb = 0, k, l;

	// ������� ���������� �����, �� ������� ����� ������
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			if(kitchen.cellArray[i][j] == EMPTY)
				nonChekedCellsNb++;

	// �������������� ���������� ��� ������ � �������� -1
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			distField[i][j] = -1;

	// ������ startCoords - ���������, ���������� �� ��� == 0
	distField[startCoords.second][startCoords.first] = 0;
	nonChekedCellsNb--;

	// ���� �� ���� ���� ��������� ������ � ����������� ����������
	while(nonChekedCellsNb){
		for(i = 0; i < K_HEIGHT; i++)
			for(j = 0; j < K_WIDTH; j++)
				// ���� ������� ��������� ������, ��� ������� �������� ���������� �� ���, ��������� �������� ���� ������� ��������� �����
				if(distField[i][j] != -1 && kitchen.cellArray[i][j] == EMPTY){
					for(k = i - 1; k <= i + 1; k++) // �� ������
						for(l = j - 1; l <= j + 1; l++){ // �� ������
							// ���� ������ ��������
							if(k >= 0 && k < K_HEIGHT && l >= 0 && l < K_WIDTH){
								// ���� ��� ��� �� ���� ��������� � ��� ������� �������� �� �����
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

// ��� �����
class CHEF{
public:
	int x;
	int y;
	short ingridsInHands; // ��� ����� � �����

	short desiredCollection = 0; // ������, ��������� ��� �������������
	bool desiredReady = true; // ���������� ������

	void read(){
		string productString;
		cin >> x >> y >> productString;
		ingridsInHands = PRODUCT::splitProduct2Ingrid(productString);
	}

	// ���� �� � ����� ����� ����������
	bool haveIngridInHands(short ingrid){
		return ingrid & ingridsInHands;
	}

	// ���� �� ����� �����
	bool isOvenNear(){
		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				if(kitchen.cellArray[i][j] == OVEN)
					return true;
		return false;
	}

	// ���������� true, ���� ������ � ����� != �������� �������
	bool needSomething(){
		return !(desiredCollection == ingridsInHands);
	}

	// ���� ������ ���� ���������� ���� � ����� - 1
	// ���� ���� �� ������ ���� �� ������� - 0
	bool takesAlreadyToHands(short ingrid){
		return !((ingridsInHands & ingrid) ^ ingrid);
	}

	// ����� ���������� �����������
	void findNearest(short ingrid){

		short minDist = 777,
			  minIngridIdx = -1;


		pair<int, int> ingridCoords = kitchen.getCellWithIngridStrongly(ingrid);

		// ���� ����� ���������� ������ ���������� �� �����
		if(isExistCell(ingridCoords)){

			// ���� ����� ���������� ���� � ������ ������������ �� ����� ������������, �� ���� ������� ����� ���������
			for(int i = 0; i < kitchen.existingProductNb; i++){

				// ���� � ������� ������������ ��������� ��������� ����������
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
		// ���� ������ ���������� �� ���������� ����� ��������� ���������, �� ��� ������� (�������/��������/���������/���������/�����)
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

	// ����� �� ����� �����������, ���������� � ������ � ������� ��� ����� �� �������
	void takeNeededIngrid(){

		// �������� � �������
		short someIngrid = 1;

		// ���������� ��� �������� ������������, ����������� � ������� desiredCollection
		// ���� someIngrid <= desiredCollection, someIngrid ����� ���� ��������� ������ desiredCollection
		while(someIngrid <= desiredCollection){

			// ����� �� ��� ������ ���� ����������?
			// �����, ���� �� ���� � ������ � ��� ��� � �����
			if((someIngrid & desiredCollection) && !(someIngrid & ingridsInHands)){

				//findNearest(someIngrid);
				useIngrid(someIngrid);
				break;
			}
			// ���� ���� ���������� �� ���������, ��������� � ����������
			else
			{
				someIngrid <<= 1;
			}
		}
	};
} me, opponent;

void recalcDistances(){
	//memcpy(distanceField, tempDistanceField, K_HEIGHT * K_WIDTH * sizeof(short));

	// ��� ���������� - ������������ ��� �������������� ���������� �� ��������� ����� �����
	pair<int, int> myCoords{me.x, me.y};

	// ������ ���������� �� ����� ������ �� ���� ��������� �����
	calcDistances(distFromMeToProducts, myCoords);

	// ��� ���� ������������ ������������ ������������ ��������� �� ������ �� ����� �����������
	// ���������� ������������ ����������� ��������������� ��������� ��������� ��������� ������
	for(int p = 0; p < kitchen.existingProductNb; p++){

		short equalMinDist = 777;

		// ��������� ���������� 8 ����� ���� � ���� �����������, �� != -1 ������
		for(int i = kitchen.existingProductVec[p].y - 1; i <= kitchen.existingProductVec[p].y + 1; i++)
			for(int j = kitchen.existingProductVec[p].x - 1; j <= kitchen.existingProductVec[p].x + 1; j++)
				// ���� ����������� ���������� �������
				if(isExistCell(pair<int, int>{j, i})){
					// ���� ���� ��������� ������, � ����������� ������ ������������, ���������� ���
					if(distFromMeToProducts[i][j] < equalMinDist && distFromMeToProducts[i][j] != -1)
						equalMinDist = distFromMeToProducts[i][j];
				}

		// ����� �������� ���� ���������� ����� ��������� equalMinDist � tempDistanceField, �� �����������, �������������� ����� �����������
		distFromMeToProducts[kitchen.existingProductVec[p].y][kitchen.existingProductVec[p].x] = equalMinDist;
	}
}

// ���������� ���-�� �����
int turnsRemaining;
// ���, �� ������� ���������� � ����� ������������
// ���� == -1, �� ���� ��������
// ����� - ���� ������ ������ � ��������� ovenReadyTurn ������������ ����, �� ������� ����� ������� ����������
int ovenReadyTurn = -1;

// ���������� �������
void readInput(){
	kitchen.existingProductVec.clear();
	clients.curClientVec.clear();

	cin >> turnsRemaining;
	me.read();
	opponent.read();

	// ������������ ������� ����������� �� �����
	cin >> kitchen.existingProductNb;
	kitchen.existingProductVec.clear();
	kitchen.existingProductVec.resize(kitchen.existingProductNb);
	for(auto &prod: kitchen.existingProductVec) prod.readInput();

	// ignore until wood 1 league
	string ovenContents;
	int ovenTimer;
	cin >> ovenContents >> ovenTimer;

	// ������� �������
	cin >> clients.curClientNb;
	clients.curClientVec.resize(clients.curClientNb);
	for(auto &cust : clients.curClientVec)
		cust.readInput();

	//recalcDistances();
};

// ������������� ������� ��������������
void use(kitchenCellType cellType){

	pair<int, int> cellWithAppliance = kitchen.getCellWithAppliance(cellType);
	if(isExistCell(cellWithAppliance)){
		cout << "USE " << cellWithAppliance.first << " " << cellWithAppliance.second << "; " + get_random_spell() << endl;
		cerr << "USE " << kitchen.cell2String(cellType) << endl;
	}
};

// ������������� ������� ��������
void use(pair<int, int> p){
	if(isExistCell(p)){
		cout << "USE " << p.first << " " << p.second << + "; " + get_random_spell() << endl;
		cerr << "USE PAIR " << p.first << " " << p.second << endl;
	}
};

// ����� ������ ����������
void useIngrid(short ingrid){

	switch(ingrid){

		cerr << "NEED TAKE " + PRODUCT::ingrid2String(ingrid) << endl;

		case DISH:{
			// ���� � ����� ��� �� ���� - �����������
			if(me.ingridsInHands != NONE) use(kitchen.getNearFreeTable(me.x, me.y));
			// ���� ���� ������ �������, ����������� �� ����� �� �� �����
			else if(isExistCell(kitchen.getCellWithIngridStrongly(DISH))) use(kitchen.getCellWithIngridStrongly(DISH));
				 // ���� ���, �� ����� �� �����
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

// �������� �� ����� ���������� ���������
void prepairChpdStrbr(){
	pair<int, int> nearFreeCell = kitchen.getNearFreeTable(me.x, me.y);

	// ���� ���� ������, ����� ������� ���������
	if(me.ingridsInHands == NONE) use(STRBR_CREATE);
	// ���� � ����� ��� �� ����
	else
		// ���� � ����� ������� ���������, ����� ��
		if(me.ingridsInHands == STRBR) use(CHOP_BOARD);
		// ���� � ����� �� ������� ���������, �� ������ ��� �� ����
		else use(nearFreeCell);
};

// �������� �� ����� ���������
void prepairCroissant(){
	pair<int, int> nearFreeCell = kitchen.getNearFreeTable(me.x, me.y);

	// ���� ����� ������
	if(ovenReadyTurn != -1){

		// ���� �������� ����� - ��������
		if(turnsRemaining < ovenReadyTurn){
			use(OVEN);
			// ���� ����� �����, �� ������ ������� ������� � ����� ���������� ���������
			if(me.isOvenNear())
				ovenReadyTurn = -1;
		}
		// ���� ���� ������������� ����� - ���������� ���
		else
		{
			// ���� �� �� � �����, �� ���� � �����
			if(!me.isOvenNear()) use(OVEN);
			// ���� � ����� - ����
			else use("WAIT");
		}
	}
	// ���� ����� ��������
	else
	{
		// ���� ���� ������ - ����� �����
		if(me.ingridsInHands == NONE) use(DOUGH_CREATE);
		// ���� � ����� �����, ������ ��� � ���� � ���������� ���, ����� ����� ������������
		else
			if(me.ingridsInHands == DOUGH)
			{
				use(OVEN);
				// ���� ��������� � ����� � ����� � ��� �������� �����
				if(me.isOvenNear())
					ovenReadyTurn = turnsRemaining - 11;
			}
		// ���� � ����� �� �����, �� ������ ��� �� ����
			else use(nearFreeCell);
	}
};

// �������� �� ����� �����
void prepairTart(){
	pair<int, int> nearFreeCell = kitchen.getNearFreeTable(me.x, me.y);

	//cerr << "PREPAIRING TART..." << endl;

	// ���� � ����� ����, ������ ��� �� ����
	if(me.ingridsInHands == TART)
		use(nearFreeCell);
	else
	{
		// ���� ����� ������
		if(ovenReadyTurn != -1)
		{
			// ���� ���� ����� - ��������
			if(turnsRemaining < ovenReadyTurn){
				use(OVEN);
				// ���� ����� �����, �� ������ ������� ������� � ����� ���������� ���������
				if(me.isOvenNear()) ovenReadyTurn = -1;
			}
			// ���� ���� ������������� ����� - ���������� ���
			else
			{
				// ���� �� �� � �����, �� ���� � �����
				if(!me.isOvenNear()) use(OVEN);
				else use("WAIT");
			}
		}
		// ���� ����� ��������
		else
		{
			switch(me.ingridsInHands){
				// ���� � ����� ����� �����, ������ ��� � �����
				case RAW_TART:
					use(OVEN);
					// ���� ��������� � ����� � ����� � ��� �������� �����
					if(me.isOvenNear())
						ovenReadyTurn = turnsRemaining - 10;
					break;
				// ���� � ����� ������� �����, ��������� ��������
				case CHPD_DOUGH: use(BLBR_CREATE); break;
				// ���� � ����� �����, ����� ���
				case DOUGH: use(CHOP_BOARD); break;
				// ���� ���� ������ - ����� �����
				case NONE: use(DOUGH_CREATE); break;
				// ���� � ����� �� ����� ����� � �� �����, �� ������ ��� �� ����
				default: use(nearFreeCell);
			}
		}
	}
};

// ��� ����������� ���� �� ����, ���� �� �������
void collectDesired(){

	//cerr << "collectDesired" << endl;

	// ���� ��� ��� ����� ����� �� ����������, ��� ��� �� �����
	if(me.needSomething()) me.takeNeededIngrid();
	// ����� ������ ����� �������
	else use(WINDOW);
};

// ��� ����� �� ������������ �� ������, ���� ������� ��, ���� ���
void prepairHardIngrids(){
	
	//cerr << "prepairHardIngrids" << endl;

	// ���� �� ������ ����� CHPD_STRBR � �� ����� �� ���
	if( (me.desiredCollection & CHPD_STRBR) && (!isExistCell(kitchen.getCellWithIngridStrongly(CHPD_STRBR))) )
	{
		//cerr << "PREPAIR " + PRODUCT::ingrid2String(CHPD_STRBR) << endl;
		prepairChpdStrbr();
		return;
	}

	// ���� �� ������ ����� CROISSANT � �� ����� ��� ���
	if((me.desiredCollection & CROISSANT) && (!isExistCell(kitchen.getCellWithIngridStrongly(CROISSANT))))
	{
		//cerr << "PREPAIR " + PRODUCT::ingrid2String(CROISSANT) << endl;
		prepairCroissant();
		return;
	}

	// ���� �� ������ ����� TART � �� ����� ��� ���
	if((me.desiredCollection & TART) && (!isExistCell(kitchen.getCellWithIngridStrongly(TART))))
	{
		//cerr << "PREPAIR " + PRODUCT::ingrid2String(TART) << endl;
		prepairTart();
		return;
	}

	//cerr << "ALL HARD INGRID EXIST IN KITCHEN";
	return;
};

// ������������ ������ �� �������
void makeDesiredCollection(){

	cerr << "desired = " << me.desiredCollection << ", inHands = " << me.ingridsInHands << endl;

	if(kitchen.existAllHardIngrids(me.desiredCollection, me.ingridsInHands))
		collectDesired();
	else
		prepairHardIngrids();
}

// ��������
void makeSomething(){

	// 0. ���� ������ ��� ��������� �����, �� ����� ����� � ������� �������
	if( (me.ingridsInHands == 0 && me.desiredReady)
		|| // ���
			  // 1. ���� ����� ������� ������� ���������, �� me.desiredCollection �������� ������ ������ ������� �������,
		      //    �� ������ me.desiredCollection �� ����� ������� �������
		(me.desiredCollection != clients.curClientVec.at(1).ingridCollection && PRODUCT::is1PartOf2(me.desiredCollection, clients.curClientVec.at(1).ingridCollection)) )
	{
		cerr << "CHANGE ORDER" << endl;
		me.desiredCollection = clients.curClientVec.at(1).ingridCollection;
		me.desiredReady = false;
	}else{
		// 2. ���� ����� ������� ������� ���������, � me.desiredCollection �� �������� ������ ������ ������� �������
		//    �� ��������� me.desiredCollection � dishwasher
		if(me.desiredCollection != clients.curClientVec.at(1).ingridCollection && !PRODUCT::is1PartOf2(me.desiredCollection, clients.curClientVec.at(1).ingridCollection)){
			cerr << "makeSomething case 1" << endl;
			cerr << "	my desiredCollection: " << int(me.desiredCollection) << ", order2 is " << int(clients.curClientVec.at(1).ingridCollection) << endl;

			// ������ �����
			me.desiredCollection = clients.curClientVec.at(1).ingridCollection;
			me.desiredReady = false;
		}
	}

	makeDesiredCollection();
};

int main()
{
	std::srand(unsigned(std::time(0)));

	// ������� ���������� �� ����� ������ �� ���� ����� �����
	distFromMeToProducts = new short*[K_HEIGHT];
	for(int i = 0; i < K_HEIGHT; i++)
		distFromMeToProducts[i] = new short[K_WIDTH];

	// ������
	cin >> clients.allClientNb;
	clients.allClientVec.resize(clients.allClientNb);
	for(auto &client: clients.allClientVec) client.readInput();

	kitchen.initialize();

	while(1){
		readInput();

		makeSomething();
	}
}
