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
	RAW_TART   = 0x0100,		// ����� �����
	TART	   = 0x0200,		// ������� ����

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
	}

	// �������������� ������ � ��������� �����������
	static short splitProduct2Ingrid(string productString){

		short result = 0x0;
		size_t hyphPos;

		while(!productString.empty()){
			if(productString.substr(0, 2) == "NO") result |= NONE;			// ������ ���� / ������ ����
			if(productString.substr(0, 2) == "DI") result |= DISH;			// ��������
			if(productString.substr(0, 2) == "BL") result |= BLBR;			// ��������
			if(productString.substr(0, 2) == "IC") result |= ICE_C;			// ���������
			if(productString.substr(0, 2) == "ST") result |= STRBR;			// ���������
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "ST") result |= CHPD_STRBR;	// ����������� ���������
			if(productString.substr(0, 2) == "DO") result |= DOUGH;			// �����
			if(productString.substr(0, 2) == "CR") result |= CROISSANT;		// ��������
			if(productString.substr(0, 2) == "CH" 
			&& productString.substr(8, 2) == "DO") result |= CHPD_DOUGH;	// �������� �����
			if(productString.substr(0, 2) == "RA") result |= RAW_TART;		// ����� �����
			if(productString.substr(0, 2) == "TA") result |= TART;			// ������� �����

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
	return ( (p.first != -1) && (p.second != -1) && (p.first < K_WIDTH) && (p.second < K_HEIGHT) );
};

void use(pair<int,int>);
void moveAsTank(pair<int, int>);

int prepChpdStrw, prepCrois, prepTarts;

// �����
class KITCHEN{
public:

	// ��� ��� �����
	short ovenContents;
	int ovenTimer;

	// ���������� ��������� / ���������
	pair<int, int> ovenCell{-1,-1};
	pair<int, int> chopCell{-1,-1};
	pair<int, int> dishCell{-1,-1};
	pair<int, int> winCell{-1,-1};
	pair<int, int> doughCell{-1,-1};
	pair<int, int> strbrCell{-1,-1};
	pair<int, int> blbrCell{-1,-1};
	pair<int, int> iceCell{-1,-1};

	// ��������� � ����� ������ ��������� ������ �����
	pair<int, int> nearFreeCell = {-1, -1};

	// ����� �������� 7�11
	vector<vector<kitchenCellType>> cellArray;

	short distArrayForMe[K_HEIGHT][K_WIDTH]; // ������ ���������� �� ���� ���� ����� ������������ ����� ������ � ������ ������������� ���������
	short distArrayForWithoutOpponent[K_HEIGHT][K_WIDTH]; // ��� �� ������ ���������, �� ��� ����� ������������� ������ ���������

	short distArrayForOpponent[K_HEIGHT][K_WIDTH]; // ������ ���������� �� ���� ���� ����� ������������ ������ ��������� � ������ ������������� ���������

	// ������������� �����
	void initialize(){

		string kitchenStroke;
		cellArray.resize(K_HEIGHT);

		for(int i = 0; i < K_HEIGHT; i++){

			cin >> kitchenStroke;
			cellArray[i].resize(K_WIDTH);

			for(int j = 0; j < K_WIDTH; j++){

				switch(int(kitchenStroke[j])){
					case 48: // ����� 0
					case 49: // ����� 1
					case 50: // ����� 2
					case 46: cellArray[i][j] = EMPTY;								break; // . ������
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
	short existingProductMap[K_HEIGHT][K_WIDTH]; // ������ � �������� ���������� � ���� ����� �����

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
	
	// ���������� ��������� �������� ����������
	// � ���������� ���������� �� ����������� �����
	pair<int, int> getNearFreeTable(){

		// ���������� �� ��������� ����������
		// ������������� �� 1 �� ��� ��� ���� �� ����� ������� ��������� ����������
		int distToFreeCell = 0;

		while(true){

			// �������� ����������� ���������
			for(int i = 2; i <= 4; i++){
				for(int j = 2; j <= 8; j++){
					// ���� ��� ���� (�������� � ����� �� ���������, ����������� �����)
					// � ���������� �� ���� ������ ����� distToFreeCell
					if(existingProductMap[i][j] == NONE && distArrayForMe[i][j] == distToFreeCell && cellArray[i][j] != EMPTY){
						return pair<int, int>{j, i};
					}
				}
			}

			// �������� ������������ ������

			// �������� �������������� ���������
			for(int i = 0; i < K_WIDTH; i++){

				// �������� ������� ����������
				if(existingProductMap[0][i] == NONE && distArrayForMe[0][i] == distToFreeCell)
					return pair<int, int>{i, 0};

				// �������� ������ ����������
				else if(existingProductMap[6][i] == NONE && distArrayForMe[6][i] == distToFreeCell)
					return pair<int, int>{i, 6};
			}

			// �������� ������������ ���������
			for(int i = 0; i < K_HEIGHT; i++)
			{
				// �����
				if(existingProductMap[i][0] == NONE && distArrayForMe[i][10] == distToFreeCell)
					return pair<int, int>{0, i};

				// ������
				else if(existingProductMap[i][10] == NONE && distArrayForMe[i][10] == distToFreeCell)
					return pair<int, int>{10, i};
			}

			distToFreeCell++;
		}

		return pair<int, int>{-1, -1};
	};

	// ����� ���������� ����������� ingrid
	pair<int,int> findNearest(short ingrid){

		pair<int, int> ingridCoords = getCellWithIngridStrongly(ingrid);

		// ���� ����� ���������� ������ ���������� �� �����
		if(isExistCell(ingridCoords)){

			// ����� ���� ����� ��������� ���� ������� ���������
			// ���� ������� ������� �������� ����� �� ������, �� ���� ��������� � ���������� �� ��������

			short minDist = 777, minIngridIdx = -1;

			switch(ingrid){
				case DISH:  minDist = distArrayForMe[dishCell.second][dishCell.first];   minIngridIdx = -2; break;
				case BLBR:  minDist = distArrayForMe[blbrCell.second][blbrCell.first];   minIngridIdx = -2; break;
				case STRBR: minDist = distArrayForMe[strbrCell.second][strbrCell.first]; minIngridIdx = -2; break;
				case DOUGH: minDist = distArrayForMe[doughCell.second][doughCell.first]; minIngridIdx = -2; break;
				default: minDist = 777, minIngridIdx = -1;
			}

			// ���� ����� ���������� ���� � ������ ������������ �� ����� ������������, �� ���� ������� ����� ���������
			for(int i = 0; i < existingProductNb; i++){

				// ���� ���� ����� ������� � ��������� ������ ��������, �� ��������� ���
				if((existingProductVec[i].ingridCollection == ingrid) && (distArrayForMe[existingProductVec[i].y][existingProductVec[i].x] < minDist))
				{
					minDist = distArrayForMe[existingProductVec[i].y][existingProductVec[i].x];
					minIngridIdx = i;
				}
			}

			// ���� minIngridIdx == -1, �� ��� ������ �����������
			if(minIngridIdx == -1)
			{
				return pair<int, int>{-1, -1};
			}
			// ���� != -1, �� ���� ����������� �������
			// ���� �������, ���� �������� ������� �������
			else
			{
				// ���� ������� - ������������ ���������� ��������
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
				// ���� �� �������, � ��������� �������
				else
				{
					return pair<int, int>{existingProductVec[minIngridIdx].x, existingProductVec[minIngridIdx].y};
				}
			}
		}
		// ���� ������ ����������� �� ����������
		else
		{
			// ���� ingrid - ������� �������, �� ������������ ���������� ��������
			// ���� ������� - ������������ (-1,-1)
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

// ������� ������� � ��� ����������� �������
class CLIENTS{
public:
	// ������ = {�����, �����}
	class CLIENT{
	public:
		short ingridCollection;
		int award;

		// ���������� �������
		void readInput(){
			string productString;
			cin >> productString >> award;
			ingridCollection = PRODUCT::splitProduct2Ingrid(productString);
		}
	};

// --------------------------------------------------------��� �������-------------------------------------------------------------
	int allClientNb;
	vector<CLIENT> allClientVec;

	// ����� ���������� ������� ���������
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

	// ���������� ������� �� ����� ���-�� ��������
	void readAllClientsInput(){

		cin >> allClientNb;
		allClientVec.resize(allClientNb);
		for(auto &client : allClientVec) client.readInput();

		calcAllHardProductNb();
	}

	// --------------------------------------------------------������� �������-------------------------------------------------------------
	int curClientNb;
	vector<CLIENT> curClientVec;

	// ������� ���������� ������� ���������
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

	// ���������� ������� �� ������� ���-�� ��������
	void readCurClientsInput(){

		curClientVec.clear();

		cin >> curClientNb;
		curClientVec.resize(curClientNb);

		for(auto &client : curClientVec) client.readInput();

		calcCurHardProductNb();
	}

	// ���������� ������ �������� ������, �������� ���������� ���������
	short getCurClientIdxWithMaxAward(){

		short idxWithMaxAward = 0;

		for(int i = 1; i < 3; i++)
			if(curClientVec[i].award > curClientVec[idxWithMaxAward].award)
				idxWithMaxAward = i;
		return idxWithMaxAward;
	}

} clients;

// ��� �����
class CHEF{
public:
	int x;
	int y;
	short ingridsInHands; // ��� ����� � �����
	short desiredCollection = -1; // ������, ��������� ��� �������������
	short servedClientIdx = -1; // ������ �������, ��� ����� ������ ������ �����
	bool usedOven = false; // ����������� �� ��� �����
	short whatNeedMake = -1;

	int howLongIStateOnPlace = 0; // ���������� �����, ������� ��� ����� ����� �� �����
	pair<int, int> previousStateCoords = {-1,-1}; // ���������� ����������� �������

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

	// ����� �� ����� �����������, ���������� � ������ � ������� ��� ����� �� �������
	void takeNeededIngrid(){

		// ��� ������������ ������� �������� �� ����� ������������ � ���� ������ � �������������� �� ���������
		// ����� ���������� ����� ������� �������, ������� �������� �� �������
		PRODUCT temp;

		// �������������� ������� ������������ ���������
		for(int i = 0; i < kitchen.existingProductNb - 1; i++){

			int prodIdxWithMaxAward = i;

			for(int j = i + 1; j < kitchen.existingProductNb; j++)
				if(kitchen.existingProductVec[j].ingridCollection > kitchen.existingProductVec[prodIdxWithMaxAward].ingridCollection)
					prodIdxWithMaxAward = j;

			temp = kitchen.existingProductVec[i];
			kitchen.existingProductVec[i] = kitchen.existingProductVec[prodIdxWithMaxAward];
			kitchen.existingProductVec[prodIdxWithMaxAward] = temp;
		}

		cerr << "��������������� ������� ��������:" << endl;
		for(auto &product : kitchen.existingProductVec)
			cerr << "(" << product.x << "," << product.y << ") ";
		cerr << endl;

		// ������ ������ �������� ������������ �� ��������

		// ����� ��� ��� ������ �������� ����� ������ ��������, ���� ���������, ���� �� ����� �������
		// ���� � ����� ������� ���, �� ������� ���� ������ ���� ������� �������� �� �������, ���� ��������� �������,
		// ���� ���� ��� �� 1 �� 2 �� ���� �� ����� �������

		// ���� ������� ���, ���� �������� � ���������
		if(!(ingridsInHands & DISH))
		{
			cerr << "����� ������� � �������� / �������" << endl;

			// ���� � ����� ���� ����� �� ���������� ��� �������, ������ ��� �� ����
			if(ingridsInHands != NONE)
			{
				cerr << "� ����� �� �������, � ����� ������� - ����������" << endl;
				moveAsTank(kitchen.nearFreeCell);
				return;
			}
			else
			{
				// ���� ������� ���� ��� ������������ �� ����� ���������
				for(auto &product : kitchen.existingProductVec)
					if(PRODUCT::is1PartOf2(product.ingridCollection, desiredCollection) && !PRODUCT::is1PartOf2(product.ingridCollection, ingridsInHands) && (product.ingridCollection & DISH))
					{
						cerr << "���� ������ ������� � ��������" << endl;
						moveAsTank(pair<int, int>{product.x, product.y});
						return;
					}

				cerr << "��� ������ �����������, ����� �� �����������" << endl;
				// ���� �� ����� � ������������, ���� � �����������
				moveAsTank(kitchen.dishCell);
				return;
			}
		}
		// ���� ������� � ����� ����, �� ����� �������� ������ �� ��������, ������� ��� �������
		else
		{
			// ���� ����� ��������� �������, ������ �� �������

			pair<int, int> cellWithClosestProduct{-1,-1};

			// ������� ����������� ��������, ��� ������������ �� �����
			int minDist = 999, minProductIdx = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				// ���� ������������ ������� �������� ������ ������� � ��� ��� ��� � �����
				if( (kitchen.existingProductVec[i].ingridCollection & desiredCollection) && !(kitchen.existingProductVec[i].ingridCollection & ingridsInHands) )
				{
					if(kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] < minDist){

						minDist = kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x];
						minProductIdx = i;
					}
				}
			}
			// ���� ��� �� ����� - ���������
			if(minProductIdx != -1)
			{
				cellWithClosestProduct.first = kitchen.existingProductVec[minProductIdx].x;
				cellWithClosestProduct.second = kitchen.existingProductVec[minProductIdx].y;
			}

		// ����� ���������� ��������
			// ���� ����� ���������, � � ����� �� ���, � ���������� �� �������� ��������� ������ ��� �� �������� ��������, �� ��������� 
			if(desiredCollection & ICE_C && !(ingridsInHands & ICE_C) && kitchen.distArrayForMe[kitchen.iceCell.second][kitchen.iceCell.first] < minDist)
			{
				cellWithClosestProduct = kitchen.iceCell;
				minDist = kitchen.distArrayForMe[kitchen.iceCell.second][kitchen.iceCell.first];
			}

			// ���� ����� ��������, � � ����� �� ���,  � ���������� �� �������� �������� ������ ��� �� �������� ��������, �� ��������� 
			if(desiredCollection & BLBR && !(ingridsInHands & BLBR) && kitchen.distArrayForMe[kitchen.blbrCell.second][kitchen.blbrCell.first] < minDist)
			{
				cellWithClosestProduct = kitchen.blbrCell;
				minDist = kitchen.distArrayForMe[kitchen.blbrCell.second][kitchen.blbrCell.first];
			}
			
			if(isExistCell(cellWithClosestProduct))
			{
				cerr << "���� �������, ���������� ����� �������..." << endl;
				moveAsTank(cellWithClosestProduct);
				return;
			}
		}
	};

	// ���������� �� ���������� �������
	// ���� ����� ����������� ���� ��� ���������� ����������, ������������ true
	// ����� - false
	bool isStateCoordsChange(){

		if(x != previousStateCoords.first && y != previousStateCoords.second)
			return true;
		else
			return false;
	};

	// ���������� ��������� ����� ����
	void updateCoords(){
		
		// ���� ���������� ����������, �� ������������ ������� ������������
		// ����������� ���������� ���������� �������
		if(isStateCoordsChange()){

			howLongIStateOnPlace = 0;
			previousStateCoords = pair<int,int>{x, y};
		}
		// ���� ���������� �� ���������
		// �� ������������� ������� ������������
		else
		{
			howLongIStateOnPlace++;
		}
	};

} me, opponent;

// ���������� ���-�� �����
int turnsRemaining;

// ���, �� ������� ���������� � ����� ������������
// ���� == -1, �� ���� ��������
// ����� - ���� ������ ������ � ��������� ovenReadyTurn ������������ ����, �� ������� ����� ������� ����������
int ovenReadyTurn = -1;

// ���� ������ ������, ����� ���� ������� ����� ���������
// ���� ���� ������ � false ������ ����� ������� ��������� ���-�� ���������
bool needPrepairing = PREPAIRING_FLAG;

// ������ ��������� �� ���� �� ���� ������ ����� � ������ ������������� ��������� ��� ��������
void calcDistancesWithOpponent(){

	// nonChekedCellsNb - ���������� ������������� ��������� �����
	int i, j, nonChekedCellsNb = 0, k, l;

	// ���������� ��������� �����, �� ������� ���� ����� ����������
	nonChekedCellsNb = 29;

	// ��������� ������������� 777
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			kitchen.distArrayForMe[i][j] = 777;

	// ������ �� ���� - ���������, ���������� == 0
	kitchen.distArrayForMe[me.y][me.x] = 0;

	// ���� �������� ��������� ���� � �����, �� ���� ������ �� ��������� ������, ������� �� ���������

	// �� ���� ��������� �������
	// ���� �� ���� ���� ��������� ������ � ����������� ����������
	int counter = 22;
	//while(nonChekedCellsNb > 1)
	while(counter > 0)
	{
		for(i = 1; i < K_HEIGHT - 1; i++)
			for(j = 1; j < K_WIDTH - 1; j++)
				// ���� ������� ��������� ������, ��� ������� �������� ���������� �� ���, ��������� �������� ���� ������� ��������� �����
				// ��� ������ ������ ���������� �� ��� ������, �� ������� ����� ��������
				if((kitchen.distArrayForMe[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY))
				{
					//cerr << "(" << j << "," << i << ") = " << kitchen.distArrayForMe[i][j] << endl;

					// ����������� ������ �������4 ������
					// ���� ���������� �� ������ ��� �� ��������, ��� ��������� � � ��� ��� ���������, �� ���������

					// �����
					if((kitchen.distArrayForMe[i][j - 1] > 333) && (kitchen.cellArray[i][j - 1] == EMPTY) && !(i == opponent.y && (j - 1) == opponent.x)){
						kitchen.distArrayForMe[i][j - 1] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	1. (" << j - 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j - 1] << endl;
					}

					// ������
					if(kitchen.distArrayForMe[i][j + 1] > 333 && kitchen.cellArray[i][j + 1] == EMPTY && !(i == opponent.y && (j + 1) == opponent.x)){
						kitchen.distArrayForMe[i][j + 1] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	2. (" << j + 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j + 1] << endl;
					}

					// �������
					if(kitchen.distArrayForMe[i - 1][j] > 333 && kitchen.cellArray[i - 1][j] == EMPTY && !((i - 1) == opponent.y && j == opponent.x)){
						kitchen.distArrayForMe[i - 1][j] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	3. (" << j << "," << i - 1 << ") = " << kitchen.distArrayForMe[i - 1][j] << endl;
					}

					// ������
					if(kitchen.distArrayForMe[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY && !((i + 1) == opponent.y && j == opponent.x)){
						kitchen.distArrayForMe[i + 1][j] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	4. (" << j << "," << i + 1 << ") = " << kitchen.distArrayForMe[i + 1][j] << endl;
					}
				}
		counter--;
	}

	// ���������� ���������� �� ���� ����� �����
	// �������� �� ���� ������� ����� � ���� ������ �� ���������, �� ��������� �� ��� == ������������ �� �������8 ���������
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			// ���� ��� ������ �����
			if(kitchen.cellArray[i][j] != EMPTY){

				//cerr << endl << "check coord " << j << " " << i << endl;

				int minDist = 777;

				// ��������� �������8
				// ���� ��������� � ����������� �����������
				for(k = i - 1; k <= i + 1; k++)
					for(l = j - 1; l <= j + 1; l++)
						// �������� �� ����������
						if(isExistCell(pair<int, int>{l, k}))
						{
							// ���� ��� ��������� ������ � ���������� �� ��� ������ ��������, �� ��������� ��� ���������� ��� ����� �������
							if(kitchen.cellArray[k][l] == EMPTY && kitchen.distArrayForMe[k][l] < minDist && kitchen.distArrayForMe[k][l] != -1)
							{
								minDist = kitchen.distArrayForMe[k][l];
								//cerr << "	coord " << l << " " << k << " is empty" << endl;
							}
						}

				//cerr << " dist to " << j << " " << i << " = " << minDist << endl;

				// ���� ������ ����� ������������� ����������� ��������� ����������
				kitchen.distArrayForMe[i][j] = minDist;
			}

	/*
	cerr << "����������:" << endl;
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

// ������ ��������� �� ���� �� ���� ������ ����� ��� ����� ���������
void calcDistancesWithoutOpponent(){

	// nonChekedCellsNb - ���������� ������������� ��������� �����
	int i, j, k, l;

	// ��������� ������������� 777
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			kitchen.distArrayForWithoutOpponent[i][j] = 777;

	// ������ �� ���� - ���������, ���������� == 0
	kitchen.distArrayForWithoutOpponent[me.y][me.x] = 0;

	// �� ���� ��������� �������
	// ���� �� ���� ���� ��������� ������ � ����������� ����������
	int counter = 21;
	//while(nonChekedCellsNb > 1)
	while(counter > 0)
	{
		for(i = 1; i < K_HEIGHT - 1; i++)
			for(j = 1; j < K_WIDTH - 1; j++)
				// ���� ������� ��������� ������, ��� ������� �������� ���������� �� ���, ��������� �������� ���� ������� ��������� �����
				// ��� ������ ������ ���������� �� ��� ������, �� ������� ����� ��������
				if((kitchen.distArrayForWithoutOpponent[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY))
				{
					// ����������� ������ �������4 ������
					// ���� ���������� �� ������ ��� �� �������� � ��� ���������, �� ���������

					// �����
					if( kitchen.distArrayForWithoutOpponent[i][j - 1] > 333 && kitchen.cellArray[i][j - 1] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i][j - 1] = kitchen.distArrayForWithoutOpponent[i][j] + 1;

					// ������
					if( kitchen.distArrayForWithoutOpponent[i][j + 1] > 333 && kitchen.cellArray[i][j + 1] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i][j + 1] = kitchen.distArrayForWithoutOpponent[i][j] + 1;

					// �������
					if( kitchen.distArrayForWithoutOpponent[i - 1][j] > 333 && kitchen.cellArray[i - 1][j] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i - 1][j] = kitchen.distArrayForWithoutOpponent[i][j] + 1;

					// ������
					if( kitchen.distArrayForWithoutOpponent[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY )
						kitchen.distArrayForWithoutOpponent[i + 1][j] = kitchen.distArrayForWithoutOpponent[i][j] + 1;
				}
		counter--;
	}

	// ���������� ���������� �� ���� ����� �����
	// �������� �� ���� ������� ����� � ���� ������ �� ���������, �� ��������� �� ��� == ������������ �� �������8 ���������
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			// ���� ��� ������ �����
			if(kitchen.cellArray[i][j] != EMPTY){

				int minDist = 777;

				// ��������� �������8
				// ���� ��������� � ����������� �����������
				for(k = i - 1; k <= i + 1; k++)
					for(l = j - 1; l <= j + 1; l++)
						// �������� �� ����������
						if(isExistCell(pair<int, int>{l, k}))
						{
							// ���� ��� ��������� ������ � ���������� �� ��� ������ ��������, �� ��������� ��� ���������� ��� ����� �������
							if(kitchen.cellArray[k][l] == EMPTY && kitchen.distArrayForWithoutOpponent[k][l] < minDist && kitchen.distArrayForWithoutOpponent[k][l] != -1)
							{
								minDist = kitchen.distArrayForWithoutOpponent[k][l];
							}
						}

				//cerr << " dist to " << j << " " << i << " = " << minDist << endl;

				// ���� ������ ����� ������������� ����������� ��������� ����������
				kitchen.distArrayForWithoutOpponent[i][j] = minDist;
			}
	/*
	cerr << "���������� ��� ����� ���������:" << endl;
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

// ������ ���������� ��� ���������
void calcOpponentDistances(){

	// nonChekedCellsNb - ���������� ������������� ��������� �����
	int i, j, nonChekedCellsNb = 0, k, l;

	// ���������� ��������� �����, �� ������� ���� ����� ����������
	nonChekedCellsNb = 29;

	// ��������� ������������� 777
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			kitchen.distArrayForOpponent[i][j] = 777;

	// ������ �� ���� - ���������, ���������� == 0
	kitchen.distArrayForOpponent[opponent.y][opponent.x] = 0;

	// �� ���� ��������� �������
	// ���� �� ���� ���� ��������� ������ � ����������� ����������
	int counter = 22;
	//while(nonChekedCellsNb > 1)
	while(counter > 0)
	{
		for(i = 1; i < K_HEIGHT - 1; i++)
			for(j = 1; j < K_WIDTH - 1; j++)
				// ���� ������� ��������� ������, ��� ������� �������� ���������� �� ���, ��������� �������� ���� ������� ��������� �����
				// ��� ������ ������ ���������� �� ��� ������, �� ������� ����� ��������
				if((kitchen.distArrayForOpponent[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY))
				{
					//cerr << "(" << j << "," << i << ") = " << kitchen.distArrayForMe[i][j] << endl;

					// ����������� ������ �������4 ������
					// ���� ���������� �� ������ ��� �� ��������, ��� ��������� � � ��� ��� ���������, �� ���������

					// �����
					if((kitchen.distArrayForOpponent[i][j - 1] > 333) && (kitchen.cellArray[i][j - 1] == EMPTY) && !(i == me.y && (j - 1) == me.x)){
						kitchen.distArrayForOpponent[i][j - 1] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	1. (" << j - 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j - 1] << endl;
					}

					// ������
					if(kitchen.distArrayForOpponent[i][j + 1] > 333 && kitchen.cellArray[i][j + 1] == EMPTY && !(i == me.y && (j + 1) == me.x)){
						kitchen.distArrayForOpponent[i][j + 1] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	2. (" << j + 1 << "," << i << ") = " << kitchen.distArrayForMe[i][j + 1] << endl;
					}

					// �������
					if(kitchen.distArrayForOpponent[i - 1][j] > 333 && kitchen.cellArray[i - 1][j] == EMPTY && !((i - 1) == me.y && j == me.x)){
						kitchen.distArrayForOpponent[i - 1][j] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	3. (" << j << "," << i - 1 << ") = " << kitchen.distArrayForMe[i - 1][j] << endl;
					}

					// ������
					if(kitchen.distArrayForOpponent[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY && !((i + 1) == me.y && j == me.x)){
						kitchen.distArrayForOpponent[i + 1][j] = kitchen.distArrayForOpponent[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	4. (" << j << "," << i + 1 << ") = " << kitchen.distArrayForMe[i + 1][j] << endl;
					}
				}
		counter--;
	}

	// ���������� ���������� �� ���� ����� �����
	// �������� �� ���� ������� ����� � ���� ������ �� ���������, �� ��������� �� ��� == ������������ �� �������8 ���������
	for(i = 0; i < K_HEIGHT; i++)
		for(j = 0; j < K_WIDTH; j++)
			// ���� ��� ������ �����
			if(kitchen.cellArray[i][j] != EMPTY){

				//cerr << endl << "check coord " << j << " " << i << endl;

				int minDist = 777;

				// ��������� �������8
				// ���� ��������� � ����������� �����������
				for(k = i - 1; k <= i + 1; k++)
					for(l = j - 1; l <= j + 1; l++)
						// �������� �� ����������
						if(isExistCell(pair<int, int>{l, k}))
						{
							// ���� ��� ��������� ������ � ���������� �� ��� ������ ��������, �� ��������� ��� ���������� ��� ����� �������
							if(kitchen.cellArray[k][l] == EMPTY && kitchen.distArrayForOpponent[k][l] < minDist && kitchen.distArrayForOpponent[k][l] != -1)
							{
								minDist = kitchen.distArrayForOpponent[k][l];
								//cerr << "	coord " << l << " " << k << " is empty" << endl;
							}
						}

				//cerr << " dist to " << j << " " << i << " = " << minDist << endl;

				// ���� ������ ����� ������������� ����������� ��������� ����������
				kitchen.distArrayForOpponent[i][j] = minDist;
			}

	/*
	cerr << "����������:" << endl;
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

// ������ ���������� �� ����� ����� ������ �� ���� ������ �����
void calcDistances(){

	calcDistancesWithOpponent();
	calcDistancesWithoutOpponent();
	calcOpponentDistances();
};

// ���������� ������ �������� ���������� ������, ������� ������ �������
void estimateOpponentOrderIdx(){

	// ���� � ���� � ����� ���� �� ������ �������, �� �������� �� �����������
	if(opponent.ingridsInHands > DISH){

		// ���������� ������� ������� ��������, ��� ������� �������� ���������� ������� ���������
		int isPartsOfCollectNb = 0, idx = -1;

		for(int i = 0; i < 3; i++)
			if(PRODUCT::is1PartOf2(opponent.ingridsInHands, clients.curClientVec.at(i).ingridCollection)){
				isPartsOfCollectNb++;
				idx = i;
			}

		/// ���� ����� ����� ������ ����, �� �� � �������� �����������
		// ���� ����� ����� ����, �� � ��������� ����������� �������-���������
		if(isPartsOfCollectNb > 0){
			opponent.servedClientIdx = idx;
		}
		// ���� � ����� �� �����, �� ��� ����� �� ����������
		else
		{
			// ���� � ����� ����� ���� ��� ���������� �����, �� ���� ����� ����� � ������
			if(opponent.ingridsInHands == RAW_TART || CHPD_DOUGH){
				for(int i = 0; i < 3; i++)
					if(clients.curClientVec.at(i).ingridCollection & TART)
						opponent.servedClientIdx = i;
			}
			// ���� � ����� �����, �� ���� ����� ����� � ������/���������
			else if(opponent.ingridsInHands == DOUGH){
				for(int i = 0; i < 3; i++)
					if(clients.curClientVec.at(i).ingridCollection & TART || clients.curClientVec.at(i).ingridCollection & CROISSANT)
						opponent.servedClientIdx = i;
			}
			// ���� � ����� ���������, �� ���� ����� ����� � ������� ����������
			else if(opponent.ingridsInHands == STRBR){
				for(int i = 0; i < 3; i++)
					if(clients.curClientVec.at(i).ingridCollection & CHPD_STRBR)
						opponent.servedClientIdx = i;
			}
		}
	}
	// ���� ������� ��� ��� ��� ������, �� ������ ������� ��� ������ ��������,
	// �� ����� ������������, ��� �� ������ ����� ������� �����
	else
	{
		opponent.servedClientIdx = clients.getCurClientIdxWithMaxAward();
	}
};

// ���������� �������
void readInput(){
	kitchen.existingProductVec.clear();
	clients.curClientVec.clear();

	// ���� ����� ������ �������, ������� ���������
	if(turnsRemaining == 200 || turnsRemaining == 199 || turnsRemaining == 198){

		cerr << "������� ����� �����" << endl;

		needPrepairing = PREPAIRING_FLAG;

		me.usedOven = false;
		me.desiredCollection = -1;
		me.servedClientIdx = -1;

		prepChpdStrw = 0;
		prepCrois = 0;
		prepTarts = 0;
	}

	// ���� ��� ��������� ���� �������
	if(needPrepairing && prepChpdStrw == 2 && prepCrois == 1) needPrepairing = false;

	me.read();
	opponent.read();

	// ����� ����� ������������ ���������
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			kitchen.existingProductMap[i][j] = NONE;
	// �������� ����������� � ����
	kitchen.existingProductMap[kitchen.iceCell.second][kitchen.iceCell.first]     = ICE_C;
	kitchen.existingProductMap[kitchen.dishCell.second][kitchen.dishCell.first]   = DISH;
	kitchen.existingProductMap[kitchen.doughCell.second][kitchen.doughCell.first] = DOUGH;
	kitchen.existingProductMap[kitchen.strbrCell.second][kitchen.strbrCell.first] = STRBR;
	kitchen.existingProductMap[kitchen.blbrCell.second][kitchen.blbrCell.first]   = BLBR;
	kitchen.existingProductMap[kitchen.ovenCell.second][kitchen.ovenCell.first]   = -1;
	kitchen.existingProductMap[kitchen.chopCell.second][kitchen.chopCell.first]   = -1;
	kitchen.existingProductMap[kitchen.winCell.second][kitchen.winCell.first]	  = -1;


	// ������������ ������� ����������� �� �����
	cin >> kitchen.existingProductNb;
	kitchen.existingProductVec.clear();
	kitchen.existingProductVec.resize(kitchen.existingProductNb);
	for(auto &prod : kitchen.existingProductVec){
		prod.readInput();
		kitchen.existingProductMap[prod.y][prod.x] = prod.ingridCollection;
	}

	// ��� ��� �����
	string ingridInOven;
	cin >> ingridInOven >> kitchen.ovenTimer;
	kitchen.ovenContents = PRODUCT::splitProduct2Ingrid(ingridInOven);

	// ������ ������� ��������
	clients.readCurClientsInput();

	// ������ ��������� �� ���� ���� �����
	calcDistances();

	// �����������, ����� ����� ������ ��������
	estimateOpponentOrderIdx();

	// ��������� ��������� ����� �� ������
	kitchen.nearFreeCell = kitchen.getNearFreeTable();

	me.updateCoords();
	opponent.updateCoords();

	//cerr << "��������� ��������� ������: " << kitchen.nearFreeCell.first << " " << kitchen.nearFreeCell.second << endl;
};

// ������������� ������� ��������
void use(pair<int, int> p){
	if(isExistCell(p)){
		cout << "USE " << p.first << " " << p.second << + "; " + get_random_spell() << + ";" << endl;
		cerr << "USE PAIR " << p.first << " " << p.second << endl;
	}
};

// �������� � ��������� �����
// ���� ���������� �� ����������� ���� <= 6, �� ����� ������������� ��������� � ���� ��������, ��� �������� ��� �����
void moveAsTank(pair<int, int> p)
{
	// ���� ���������� � ������ ��������� ����� ���������� ��� ����� ���������, �� �� �� ������ � ����� ���� ������� ����
	if(kitchen.distArrayForMe[p.second][p.first] == kitchen.distArrayForWithoutOpponent[p.second][p.first])
	{
		use(p);
	}
	// ������ ���� �� �����
	// ������������� ���� ������ �����������, ����� ���������� � ������ ��������� ������ ���������� ��� ����� ���������
	else
	{
		// �������� �������� ������ ���� ���������� <= 8 � �������� �� � �����
		if(kitchen.distArrayForWithoutOpponent[p.second][p.first] <= 8 && !opponent.isOvenNear() && opponent.howLongIStateOnPlace < BLOCK_TRESHOLD)
		{
			cerr << "�������� ���� ��������" << endl;

			// ���� ���������� ���������� ������ ����, � ������� � ���� �������, ����� ��������� ���������� �� �������� ����

			// ������� ������������ ��� ������ ���� ����� �� ���������
			// �� ���� ���� ������ ���������� ������, ���������� �� ������� ���������� � �������� ��� � ��� ������

			pair<int, int> point{-1,-1};
			int minDist = 777;

			for(int i = 0; i < K_HEIGHT; i++)
				for(int j = 0; j < K_WIDTH; j++)
					// ���� ��� ������ ���� � ���������� �� ���� ������ ������ ��������
					if(kitchen.cellArray[i][j] == EMPTY && kitchen.distArrayForMe[i][j] < minDist)
					{
						//cerr << "������ " << j << " " << i << " ����� ���������� " << kitchen.distArrayForMe[i][j] << endl;

						// ���� ��� ������� ���� ������ ������
						if( ((i == opponent.y - 1) && (j == opponent.x)) || ((i == opponent.y + 1) && (j == opponent.x))
						   // ���� ����� ��� ������
						 || ((i == opponent.y) && (j == opponent.x - 1)) || ((i == opponent.y) && (j == opponent.x + 1)) )
						{
							point = pair<int, int>{j, i};
							minDist = kitchen.distArrayForMe[i][j];
						}
					}
			cerr << "�������� �������� � ������ " << point.first << " " << point.second << endl;
			cout << "MOVE " << point.first << " " << point.second << endl;
		}
		// ���� ���������� > 8 ��� �������� � �����, �� ����� ���� � �����
		else
		{
			use(p);
		}
	}
};

// ���� � �����  ����������, �� �������� ���� �������, ���� ��� ����� ��������
// ���� � ����� ��������� ����������, ���� ��� �����������, �� ������ ���� �������������� �����
void findWherePut(short ingrid){

	cerr << "��� ���� ���� ���������� �� ���..." << endl;

	switch(ingrid){

		// ���� � ����� ������� ����, �� ���� ����� ������� � ��� ��, � ������� ����� �������� ����
		// ��� ���� ��������� ���������� ������� ������ ����� ������������� ������ �� ������������ �������
		case TART:
		{
			// ����� ����� ���������� 
			if(me.whatNeedMake == TART)
				me.whatNeedMake = -1;

			// ������ ������������ �������, ���� � ����� �� ����� ����� -1, �� ���� ������ �������� ���� �����
			int existingDishWhereCanPutTort = -1,
				minDist = 666;

			// ����� ���� ������������ ��������� ������ �������, � ������� ����� �������� ����
			for(int i = 0; i < kitchen.existingProductNb; i++){

				if(kitchen.existingProductVec[i].ingridCollection & DISH // ���� ��� ������� � ��������
					&& !(kitchen.existingProductVec[i].ingridCollection & TART) // ���� ������� �� �������� ����
					&& (
						PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | TART), clients.curClientVec[0].ingridCollection) // ��� ������� � ������ ����� ��������� ��� 0-�� ������
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | TART), clients.curClientVec[1].ingridCollection) // ��� ������� � ������ ����� ��������� ��� 1-�� ������
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | TART), clients.curClientVec[2].ingridCollection) // ��� ������� � ������ ����� ��������� ��� 2-�� ������
						)
					&& kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] < minDist
				   )
				{
					existingDishWhereCanPutTort = i;
					minDist = kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x];
				}
			}

			// ���� ���������� ������� ������� - ���������� ��
			if(existingDishWhereCanPutTort != -1)
			{
				cerr << "	���������� �������, � ������� ����� �������� ������� ����, ��� � ���" << endl;
				moveAsTank(pair<int, int>{kitchen.existingProductVec[existingDishWhereCanPutTort].x, kitchen.existingProductVec[existingDishWhereCanPutTort].y});
			}
			// ���� ����� ������� ���, �� ������ �� ��������� ��������� ����
			else
			{
				cerr << "	�� ���������� �������, � ������� ����� �������� ������� ����, ����� �� ��������� ����" << endl;
				moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		// ���� � ����� ������� ��������, �� ���� ����� ������� � ��� ��, � ������� ����� �������� ��������
		// ��� ���� ��������� ���������� ������� ������ ����� ������������� ������ �� ������������ �������
		case CROISSANT:
		{
			// ����� ����� ���������� 
			if(me.whatNeedMake == CROISSANT)
				me.whatNeedMake = -1;

			// ������ ������������ �������, ���� � ����� �� ����� ����� -1, �� ���� ������ �������� �������� �����
			int existingDishWhereCanPutCroissant = -1;

			// ����� ���� ������������ ��������� ������ �������, � ������� ����� �������� ��������
			for(int i = 0; i < kitchen.existingProductNb; i++){

				if(		 kitchen.existingProductVec[i].ingridCollection & DISH // ���� ��� ������� � ��������
					&& !(kitchen.existingProductVec[i].ingridCollection & CROISSANT) // ���� ������� �� �������� ��������
					&& (
						   PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CROISSANT), clients.curClientVec[0].ingridCollection) // ��� ������� � ���������� ����� ��������� ��� 0-�� ������
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CROISSANT), clients.curClientVec[1].ingridCollection) // ��� ������� � ���������� ����� ��������� ��� 1-�� ������
						|| PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CROISSANT), clients.curClientVec[2].ingridCollection) // ��� ������� � ���������� ����� ��������� ��� 2-�� ������
						)
				   )
					existingDishWhereCanPutCroissant = i;
			}

			// ���� ���������� ������� ������� - ���������� ��
			if(existingDishWhereCanPutCroissant != -1)
			{
				cerr << "	���������� �������, � ������� ����� �������� ������� ��������, ��� � ���" << endl;
				moveAsTank(pair<int, int>{kitchen.existingProductVec[existingDishWhereCanPutCroissant].x, kitchen.existingProductVec[existingDishWhereCanPutCroissant].y});
			}
			// ���� ����� ������� ���, �� ������ �� ��������� ��������� ����
			else
			{
				cerr << "	�� ���������� �������, � ������� ����� �������� ������� ��������, ����� �� ��������� ����" << endl;
				moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}
	
		// ���� � ����� ���������� ���������, �� ���� ����� ������� � ��� ��, � ������� ����� �������� ���������� ���������
		// ��� ���� ��������� ���������� ������� ������ ����� ������������� ������ �� ������������ �������
		case CHPD_STRBR:
		{
			// ����� ����� ���������� 
			if(me.whatNeedMake == CHPD_STRBR)
				me.whatNeedMake = -1;

			// ������ ������������ �������, ���� � ����� �� ����� ����� -1, �� ���� ������ �������� ���������� ��������� �����
			int existingDishWhereCanPutChpdStrbr = -1,
				minAward = 0;

			// ����� ���� ������������ ��������� ������ �������, � ������� ����� �������� ���������� ���������
			for(int i = 0; i < kitchen.existingProductNb; i++){

				// ���� ��� ������� � �������� � ������� �� �������� ���������� ���������
				if(kitchen.existingProductVec[i].ingridCollection & DISH && !(kitchen.existingProductVec[i].ingridCollection & CHPD_STRBR))
				{
					// ���� ���������, ����� �� ������� � ���������� ���������� ������������� ������ ���� ������
					for(int j = 0; j < clients.curClientNb; j++)
					{
						// ���� ������� � ���������� ���������� ����� ��������� ��� j-�� ������, ����������
						if(   PRODUCT::is1PartOf2((kitchen.existingProductVec[i].ingridCollection | CHPD_STRBR), clients.curClientVec[j].ingridCollection)
						   && kitchen.existingProductVec[i].ingridCollection > minAward) // �������� ������
						{
							existingDishWhereCanPutChpdStrbr = i;
							minAward = kitchen.existingProductVec[i].ingridCollection;
						}
					}
				}
						   
			}

			// ���� ���������� ������� ������� - ���������� ��
			if(existingDishWhereCanPutChpdStrbr != -1)
			{
				cerr << "	���������� �������, � ������� ����� �������� ���������� ���������, ��� � ���" << endl;
				moveAsTank(pair<int, int>{kitchen.existingProductVec[existingDishWhereCanPutChpdStrbr].x, kitchen.existingProductVec[existingDishWhereCanPutChpdStrbr].y});
			}
			// ���� ����� ������� ���, �� ������ �� ��������� ��������� ����
			else
			{
				cerr << "	�� ���������� �������, � ������� ����� �������� ���������� ���������, ����� �� ��������� ����" << endl;
				moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}
	
		// ���� � ����� ���������, �� �� ���� ���� ��������, ���� �������� �� ��������� ����
		// ���� ����������� ����� � ��������� ������������ �� 1 ���, �� ���� ������
		// ����� - ������ �� ��������� ����
		case STRBR:
		{
			if(kitchen.distArrayForMe[kitchen.chopCell.second][kitchen.chopCell.first] <= 4)
				moveAsTank(kitchen.chopCell);
			else
				moveAsTank(kitchen.nearFreeCell);
			break;
		}

		// ���� � ����� �����, �� ���� �������� ��� ������: ���� ��� ��������
		// ���� ���������� �����, ������� ����� ��������� ��� �����
		// ���� �� ������ ����� ����, � �� ����� ��� ���������� ����� / ����� �� �������, �� ���� �������� ����
		// ���� �� ������ ����� ��������, � �� ����� ��� ���������� �������� / �������� �� �������, �� ���� �������� ��������
		case DOUGH:
		{

			// � ����������� �� ����, ��� ������ ����� ������ �� �����
			switch(me.whatNeedMake){

				// ���� ����� ���� - ����� ����� �� �����
				case TART:
					moveAsTank(kitchen.chopCell);
					break;

				// ���� ����� �������� - ����� ����� � �������
				case CROISSANT:

					// ���� �� ����� ����� � ������
					if(me.isOvenNear())
					{
						// ���� ����� �������� - ���������� � ��� ������� � ��������, ��� ����� ���� ������������
						if(kitchen.ovenContents == NONE)
						{
							me.usedOven = true;
							moveAsTank(kitchen.ovenCell);
						}
						// ���� � ����� ��� �� ����, �� ���� �������� ���� �� ��� � ������� ���������� �����
						else
						{
							moveAsTank(kitchen.nearFreeCell);
						}
					}
					// ���� �� � ����� - ���� � �����
					else
					{
						moveAsTank(kitchen.ovenCell);
					}

					break;

				// ���� ����� �� ����� �� ��� ����, �� ����������� ��� �����
				default:
					moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		// ���� � �������� ����� � �������� � �������� ������ ����
		// �� ���������� �����
		// ���� �������� ������ - ����������� �������� �����
		case CHPD_DOUGH:
		{
			// � ����������� �� ����, ��� ������ ����� ������ �� �����
			switch(me.whatNeedMake){

				// ���� ����� ���� - ����� ����� �� �������
				case TART:
					moveAsTank(kitchen.blbrCell);
					break;

				// ���� ���� �� �����, �� ��� ������ ���������� � ���� �������, �����������
				default:
					moveAsTank(kitchen.nearFreeCell);
			}

			break;
		}

		// ���� � ����� ����� �����
		case RAW_TART:
		{
			// � ����������� �� ����, ��� ������ ����� ������ �� �����
			switch(me.whatNeedMake){

				// ���� ����� ���� - ����� ����� �� ��������
				case TART:

					// ���� �� ����� ����� � ������
					if(me.isOvenNear())
					{
						// ���� ����� �������� - ���������� � ��� ������� � ��������, ��� ����� ���� ������������
						if(kitchen.ovenContents == NONE)
						{
							me.usedOven = true;
							moveAsTank(kitchen.ovenCell);
						}
						// ���� � ����� ��� �� ����, �� ���� �������� ���� �� ��� � ������� ���������� �����
						else
						{
							moveAsTank(kitchen.nearFreeCell);
						}
					}
					// ���� �� � ����� - ���� � �����
					else
					{
						moveAsTank(kitchen.ovenCell);
					}

					break;

				// ���� ���� �� �����, �� ��� ������ ���������� � ���� �������, �����������
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
			// ���� � ����� ������� � ��� ��, �� ���� �� ����������� ��������������
			if(ingrid > DISH)
			{

				cerr << "� ����� ������� � ��� ��, ������� ������� ������� �����" << endl;

				// ���� �� ������ ����� �������/����/���������, � � ����� ���� ������� �� ��� ������ �� �� ���� ������������, �� ���� �������� �� ����� ����� �� �����������
				pair<int, int>  cellWithTart      = kitchen.findNearest(TART),
								cellWithCroissant = kitchen.findNearest(CROISSANT),
								cellWithChpdStrbr = kitchen.findNearest(CHPD_STRBR);

				// ���� �� ������ ���� �����, �� ������� ��� ��� � ���� ������ � ������, �� ���� ��� �����
				if(me.desiredCollection & TART && !(ingrid & TART) && isExistCell(cellWithTart))
				{
					cerr << "	����� �������� �� ������� ����" << endl;
					moveAsTank(cellWithTart);
				}
				// ���� �� ������ �������� �����, �� ������� ��� ��� � ���� ������ � ����������, �� ���� ��� �����
				else if(me.desiredCollection & CROISSANT && !(ingrid & CROISSANT) && isExistCell(cellWithCroissant))
				{
					cerr << "	����� �������� �� ������� ��������" << endl;
					moveAsTank(cellWithCroissant);
				}
				// ���� �� ������ ���������� ��������� �����, �� ������� �� ��� � ���� ������ � ���������� ����������, �� ���� �� �����
				else if(me.desiredCollection & CHPD_STRBR && !(ingrid & CHPD_STRBR) && isExistCell(cellWithChpdStrbr))
				{
					cerr << "	����� �������� �� ������� ���������� ���������" << endl;
					moveAsTank(cellWithChpdStrbr);
				}
				// ���� � ����� ���� ������� �������, ������� ����� �������� �� �������
				else if(kitchen.ovenContents == CROISSANT || kitchen.ovenContents == TART){

					// ���� �� ������� ��� ��� ����������� ����� � ���� ���������� ������ � ����������� ����� �������� ��� ���� �� �������, �� ����� ���������� ����� � ������ �����
					if(   !(me.ingridsInHands & kitchen.ovenContents)
						&& (   PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[0].ingridCollection)
							|| PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[1].ingridCollection)
							|| PRODUCT::is1PartOf2(me.ingridsInHands | kitchen.ovenContents, clients.curClientVec[2].ingridCollection)
						   )
					   )
					{
						moveAsTank(kitchen.ovenCell);
					}
					// ���� � ����� ��� ���� ���������� �����, �� ���� �������� ��� �� ��� � ������� ������� ������� �� �����
					else
					{
						cerr << "		� ����� ������� �������, �� ���� �� �������� �� ���� �������, ����� ������� �� ����" << endl;
						moveAsTank(kitchen.nearFreeCell);
					}
				}
				// ���� � � ����� ��� ������ ��������, � ��� �� ������ �������� �����������, ������� ����� ���� �� �������� � �������, �� ����������� �� ����
				else
				{
					cerr << "	� ������� ������ ��������, ����� �� �� ����" << endl;
					moveAsTank(kitchen.nearFreeCell);
				}
			}
			else
			{
				moveAsTank(kitchen.nearFreeCell);
			}
	}
};

// ������� ������ �������
// ���� �������� �������� ���������� true
// ���� �� ����� ��������, �� ���������� false
bool saveBaking(){

	// ���� � ������� ������� �������, ��������� �� ���� ����������, �� ������� ���
	if( (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT) )
	{
		// ���� ���������� �� ����� ��������� ������ ������� �������
		if( kitchen.distArrayForMe[kitchen.ovenCell.second][kitchen.ovenCell.first] <= (kitchen.ovenTimer / 2 - 1) * 4 )
		{
			cerr << "		� ������� ���� ������� �������, ���������� ��������� �� �������" << endl;

			// ���� �������� ������ �� ������� ��� ���� �� � ����, �� ���� �������
			if(kitchen.distArrayForOpponent[kitchen.ovenCell.second][kitchen.ovenCell.first] > 4 || opponent.howLongIStateOnPlace > 5)
			{
				cerr << "			�������� ������ �� �������, ���� �������" << endl;

				// ���� ����� �����
				if(me.isOvenNear())
				{
					cerr << "			������� �����, ������� ������� �������" << endl;

					// ���� ����� ������� - ��������
					if(me.ingridsInHands == NONE // ���� ���� ������
					   // ���� ���� � ����� ���� �������, �� ������� ��� ����������� �������, � ��� ���������� ����������� ������� �� ������� ����� ����� ������������ ������ �� ��������
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
					// ���� � ����� ��� �� ������
					else
						moveAsTank(kitchen.nearFreeCell);

					return true;
				} else
				{
					cerr << "			������� �� �����, ��� � ���" << endl;
					moveAsTank(kitchen.ovenCell);
					return true;
				}
			}
			else
			{
				cerr << "			�������� ������ � �������, ������� �������" << endl;
				return false;
			}
		}
		// ���� �� ������ ������� �������
		else
		{
			cerr << "		� ������� ���� ������� �������, �� � �� ����� �� �������" << endl;
			return false;
		}
	}
	// ���� � ����� ��� ������� �������
	else
		return false;
};

// �������� �� ����� ���������� ���������
void prepairChpdStrbr(){

	me.whatNeedMake = CHPD_STRBR;

	cerr << "������� ���������" << endl;

	// ���� ���� ������, ����� ������� ���������
	if(me.ingridsInHands == NONE)
		moveAsTank(kitchen.strbrCell);
	// ���� � ����� ������� ���������, ����� ��
	else if(me.ingridsInHands == STRBR)
		moveAsTank(kitchen.chopCell);
	// ���� ��� �� ������ - �����������
	else
		findWherePut(me.ingridsInHands);
};

// �������� �� ����� ���������
void prepairCroissant(){

	me.whatNeedMake = CROISSANT;

	// ���� � ��� ����������� �����, �� ���� ���� ����� ������������� ���������, ���� ������� ������� ���� �� �����
	if(me.usedOven)
	{
		cerr << "� ��� ����� ������������ �����" << endl;

		switch(kitchen.ovenContents){

			// ���� � ����� ��������� ����� � �� ��� ������������� ��� ����� �������, �� ����� �������� ������ �� �����...
			case RAW_TART:
			case DOUGH:
			{
				cerr << "	���� ��������� ��� �������, ���� �������� �������" << endl;

				if(me.ingridsInHands == NONE)
				{
					cerr << "		� ����� ������ ���" << endl;

					// ������� �������� ����� �������, � ������� ����� ����� �������� ��������

					int minAward = 0;
					int prodIdx = -1;

					// ���� ����� ������� � ����������� �������������, ���, ���� ����� ���, ����� ������
					for(int i = 0; i < kitchen.existingProductNb; i++)
					{
						if((PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // ���� ��� ����� ����� ������
						   && (kitchen.existingProductVec[i].ingridCollection & DISH)  // ���� ������� �������� �������
						   && (kitchen.existingProductVec[i].ingridCollection > minAward) // ����� ������ �������
						   && (kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] <= (kitchen.ovenTimer / 2 - 1) * 4)) // ����������� �� ����������
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// ���� �� ����� �������, ���������� � ������ ������, ���������, ����� ���� �������, ��������������� ������� ������, � ������� ����� �������� ��������
					if(prodIdx == -1)
					{

						int newOrderIdxWithCroissant = -1;

						for(int i = 0; i < kitchen.existingProductNb; i++)
						{
							if( (kitchen.existingProductVec[i].ingridCollection & DISH)  // ���� ������� �������� �������
								&& !(kitchen.existingProductVec[i].ingridCollection & CROISSANT) // � � ������� ���� ���������
							    && (kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] <= (kitchen.ovenTimer / 2 - 1) * 4)) // ����������� �� ����������
							{
								for(int j = 0; j < clients.curClientNb; j++)
								{
									// ���� ������� ����� ���������� � ��� ����� ����� ��������� ��� ������, �� ����� ��� �������
									if(PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection | CROISSANT, clients.curClientVec[j].ingridCollection))
									{
										minAward = kitchen.existingProductVec[i].ingridCollection;
										prodIdx = i;
										newOrderIdxWithCroissant = j;
									}
								}
							}
						}

						// ���� � ��� �������� �� ���� �����������, �� ���� ����� ������ �������
						if(prodIdx == -1)
						{
							// ���� ��������� ������ �������
							pair<int, int> dishCell = kitchen.findNearest(DISH);

							// ���� ������ ����� �������, ���� ��������� �����
							if(kitchen.distArrayForMe[dishCell.second][dishCell.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
							{
								moveAsTank(dishCell);
							}
							// ���� ���������� �� ������ ������� > (kitchen.ovenTimer/2 - 2), �� ���� ������� ��� �� ������
							else
							{
								// ���� ��������� �������, ������� ����� ������
								pair<int, int> cellWithRawTart = kitchen.findNearest(RAW_TART);
								pair<int, int> cellWithDough   = kitchen.findNearest(DOUGH);

								// ���� ���������� ����� �����, �� ���� ���������, ����� �� �� ��� �����
								if(isExistCell(cellWithRawTart))
								{
									cerr << "		�� ����� ��������� ����� �����..." << endl;
									// ���� ����� ����� � �������� ������������
									if(kitchen.distArrayForMe[cellWithRawTart.second][cellWithRawTart.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		���������� �� ������ ������ ��������� ��� �����" << endl;
										moveAsTank(cellWithRawTart);
										return;
									}
									// ���� �� ������� ������, ������� ����� �����
									else
									{
										cerr << "		�� ������ ������ ������� ������ ����" << endl;
									}
								}

								// ���� ���������� �����, �� ���� ���������, ����� �� �� ��� �����
								if(isExistCell(cellWithDough))
								{
									cerr << "		�� ����� ��������� �����..." << endl;
									// ���� ����� � �������� ������������
									if(kitchen.distArrayForMe[cellWithDough.second][cellWithDough.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		���������� �� ����� ��������� ��� �����" << endl;
										moveAsTank(cellWithDough);
										return;
									}
									// ���� �� ������� ������, ������� ����� �����
									else
									{
										cerr << "		�� ����� ������� ������ ����" << endl;
									}
								}

								// ���� ��� �� ������ �� �����, �� ������ ���� � �����
								moveAsTank(kitchen.ovenCell);
							}
						}
						// ���� ����� �������, � ������� �������� �� �������, �� ������ ����� �� ���, ��� ������������ ���� ������� � ����������
						else
						{
							// ���� ����� ����������, �� ����� ��� � �������� ������������
							// � ���� �� ��������
							if(newOrderIdxWithCroissant != -1)
							{
								cerr << "����� ������, ���� ������� ������� � ��������� ������������ ������� " << newOrderIdxWithCroissant << endl;

								me.desiredCollection = clients.curClientVec[newOrderIdxWithCroissant].ingridCollection;
								me.servedClientIdx = newOrderIdxWithCroissant;

								moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
							}
							// ���� ������ ������ ���, �� ������ �� ����
							else
							{
								cerr << "����� �� ����� ������� ������" << endl;
								moveAsTank(kitchen.nearFreeCell);
							}
						}
					}
					// ���� ����� ������� � ��� ���������� ��� ����� ������
					else
					{
						cerr << "		���� ����������� ������� � ���������� � ����� ������ �� ��� �����" << endl;
						moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "� ����� �������, ���� � �����" << endl;
					moveAsTank(kitchen.ovenCell);
				}
				else
				{
					//moveAsTank(kitchen.nearFreeCell);
					findWherePut(me.ingridsInHands);
				}

				break;
			}

			// ���� � ����� ������� ��������, ���� ��� �������
			case CROISSANT:
			{
				cerr << "�������� �����, ���� ������� �� �����" << endl;

				// ����� �������� �� �����, ���� � ����� ������� ��� ��������
				// ���� ���� � ����� ������ ������ ���
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & CROISSANT)) )
				{
					// ������������ ���� ������������� ���� �����
					me.usedOven = false;

					// ����� ����� ���������� 
					if(me.whatNeedMake == CROISSANT)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// ���� � ����� ��� �� ������ - ���������� �� ����
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}
				break;
			}

			// ���� � ����� ������� ����, ���� ��� �������
			case TART:
			{
				cerr << "���� �����, ���� ������� �� �����" << endl;

				// ����� �������� �� �����, ���� � ����� ������� ��� �����
				// ���� ���� � ����� ������ ������ ���
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & TART)) )
				{
					// ������������ ���� ������������� ���� �����
					me.usedOven = false;

					// ����� ����� ���������� 
					if(me.whatNeedMake == TART)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// ���� � ����� ��� �� ������ - ���������� �� ����
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}

				break;
			}

			// ���� ����� ������, � ���� me.usedOven == true, �� ��� ��������, ��� ���� ������� � ��� ������
			case NONE:
			{
				findWherePut(me.ingridsInHands);

				// ������������ ���� ������������� ���� �����
				me.usedOven = false;
				break;
			}

			default:
				findWherePut(me.ingridsInHands);
		}
	}
	// ���� � ��� �� ����� ������������ �����
	else
	{
		cerr << "����� ��� �� ������������" << endl;

		switch(me.ingridsInHands)
		{
			// ���� � ����� ������ ���, �� ���� ����� ��������� �����
			case NONE:

				// ���� ����� � �����, � � ����� ������� �������, �� ���� �� ������� ������
				if(me.isOvenNear())
				{
					switch(kitchen.ovenContents)
					{
						// ���� � ����� �����, ����� � �������, � � ������� ��������, �� �������� �������� � ���������� ���� ���������� ���������
						case CROISSANT:
							me.whatNeedMake = -1;
							cerr << "����� � �������, ������� ������, ��������" << endl;
							moveAsTank(kitchen.ovenCell);
							break;

						// ���� � ����� �����, � � ����� ������� ����, �� ���� ��� ��������
						case TART:
							moveAsTank(kitchen.ovenCell);
							break;

						// ���� � ����� �����, ���� ��� ����� �����/�����, �� ���������� �� ����
						case DOUGH:
						case RAW_TART:
						case NONE:
							// ���� ������������ ����� / ������� �����
							moveAsTank(kitchen.findNearest(DOUGH));
							break;
					}
				}
				// ���� � ����� ��� ������ ��������
				// ���� �� � �������, � � ����� ������ ���, �� ���� ����� �����
				else
				{
					// ���� ������������ ����� / ������� �����
					moveAsTank(kitchen.findNearest(DOUGH));
				}
				break;

			default:
				// ���� � ����� ��� �� ����
				findWherePut(me.ingridsInHands);
		}
	}
};

// �������� �� ����� �����
void prepairTart(){

	me.whatNeedMake = TART;

	// ���� � ��� ����������� �������, �� ���� ���� ����� ������������� �����, ���� ������� ���� ���� �� �����
	if(me.usedOven)
	{
		cerr << "� ��� ����� ������������ �����" << endl;

		switch(kitchen.ovenContents){

			// ���� � ����� ��������� ����� � �� ��� ������������� ��� ����� �������, �� ����� �������� ������ �� �����...
			case RAW_TART:
			case DOUGH:
			{
				cerr << "���� ��������� ��� �������, ���� �������� �������" << endl;

				// ���� � ����� ��� �� ���� ����� ������� - ����������

				if(me.ingridsInHands == NONE)
				{
					cerr << "� ����� ������ ���" << endl;

					int minAward = 0;
					int prodIdx = -1;

					// ���� ����� ������� � ����������� �������������, ���, ���� ����� ���, ����� ������
					for(int i = 0; i < kitchen.existingProductNb; i++)
					{
						if( (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // ���� ��� ����� ����� ������
						   && (kitchen.existingProductVec[i].ingridCollection & DISH)  // ���� ������� �������� �������
						   && (kitchen.existingProductVec[i].ingridCollection > minAward) // ����� ������ �������
						   && (kitchen.distArrayForMe[kitchen.existingProductVec[i].x][kitchen.existingProductVec[i].y] <= (kitchen.ovenTimer / 2 - 1) * 4) )
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// ���� �� ����� �������, ���������� � ������ ������, ���������, ����� ���� �������, ��������������� ������� ������, � ������� ����� �������� ����
					if(prodIdx == -1)
					{

						int newOrderIdxWithTart = -1;

						for(int i = 0; i < kitchen.existingProductNb; i++)
						{
							if(     (kitchen.existingProductVec[i].ingridCollection & DISH)  // ���� ������� �������� �������
							    && !(kitchen.existingProductVec[i].ingridCollection & TART) // � � ������� ���� �����
							    && (kitchen.distArrayForMe[kitchen.existingProductVec[i].y][kitchen.existingProductVec[i].x] <= (kitchen.ovenTimer / 2 - 1) * 4)) // ����������� �� ����������
							{
								for(int j = 0; j < clients.curClientNb; j++)
								{
									// ���� ������� ����� ���������� � ��� ����� ����� ��������� ��� ������, �� ����� ��� �������
									if(PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection | TART, clients.curClientVec[j].ingridCollection))
									{
										minAward = kitchen.existingProductVec[i].ingridCollection;
										prodIdx = i;
										newOrderIdxWithTart = j;
									}
								}
							}
						}

						// ���� � ��� �������� �� ���� �����������, �� ���� ����� ������ �������
						if(prodIdx == -1)
						{
							// ���� ��������� ������ �������
							pair<int, int> dishCell = kitchen.findNearest(DISH);

							// ���� ������ ����� �������, ���� ��������� �����
							if(kitchen.distArrayForMe[dishCell.second][dishCell.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
							{
								moveAsTank(dishCell);
							}
							// ���� ���������� �� ������ ������� > (kitchen.ovenTimer/2 - 2), �� ���� ������� ��� �� ������
							else
							{
								// ���� ��������� �������, ������� ����� ������
								pair<int, int> cellWithRawTart = kitchen.findNearest(RAW_TART);
								pair<int, int> cellWithDough = kitchen.findNearest(DOUGH);

								// ���� ���������� ����� �����, �� ���� ���������, ����� �� �� ��� �����
								if(isExistCell(cellWithRawTart))
								{
									cerr << "		�� ����� ��������� ����� �����..." << endl;
									// ���� ����� ����� � �������� ������������
									if(kitchen.distArrayForMe[cellWithRawTart.second][cellWithRawTart.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		���������� �� ������ ������ ��������� ��� �����" << endl;
										moveAsTank(cellWithRawTart);
										return;
									}
									// ���� �� ������� ������, ������� ����� �����
									else
									{
										cerr << "		�� ������ ������ ������� ������ ����" << endl;
									}
								}

								// ���� ���������� �����, �� ���� ���������, ����� �� �� ��� �����
								if(isExistCell(cellWithDough))
								{
									cerr << "		�� ����� ��������� �����..." << endl;
									// ���� ����� � �������� ������������
									if(kitchen.distArrayForMe[cellWithDough.second][cellWithDough.first] <= (kitchen.ovenTimer / 2 - 1) * 4)
									{
										cerr << "		���������� �� ����� ��������� ��� �����" << endl;
										moveAsTank(cellWithDough);
										return;
									}
									// ���� �� ������� ������, ������� ����� �����
									else
									{
										cerr << "		�� ����� ������� ������ ����" << endl;
									}
								}

								// ���� ��� �� ������ �� �����, �� ������ ���� � �����
								moveAsTank(kitchen.ovenCell);
							}
						}
						// ���� ����� �������, � ������� �������� �� ����, �� ������ ����� �� ���, ��� ������������ ���� ������� � ����������
						else
						{
							// ���� ����� ����������, �� ����� ��� � �������� ������������
							// � ���� �� ��������
							if(newOrderIdxWithTart != -1)
							{
								cerr << "����� ������, ���� ������� ������� � ������ ������������ ������� " << newOrderIdxWithTart << endl;

								me.desiredCollection = clients.curClientVec[newOrderIdxWithTart].ingridCollection;
								me.servedClientIdx = newOrderIdxWithTart;

								moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
							}
							// ���� ������ ������ ���, �� ������ �� ����
							else
							{
								cerr << "����� �� ����� ������� ������" << endl;
								moveAsTank(kitchen.nearFreeCell);
							}
						}
					}
					// ���� ����� ������� � ��� ���������� ��� ����� ������
					else
					{
						cerr << "		���� ����������� ������� � ���������� � ����� ������ �� ��� �����" << endl;
						moveAsTank(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "� ����� �������, ���� � �����" << endl;
					moveAsTank(kitchen.ovenCell);
				}
				else
				{
					findWherePut(me.ingridsInHands);
				}

				break;
			}

			// ���� � ����� ������� ��������, ���� ��� �������
			case CROISSANT:
			{
				cerr << "�������� �����, ���� ������� �� �����" << endl;

				// ����� �������� �� �����, ���� � ����� ������� ��� ��������
				// ���� ���� � ����� ������ ������ ���
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & CROISSANT)) )
				{
					// ������������ ���� ������������� ���� �����
					me.usedOven = false;

					// ����� ����� ���������� 
					if(me.whatNeedMake == CROISSANT)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// ���� � ����� ��� �� ������ - ���������� �� ����
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}
				break;
			}

			// ���� � ����� ������� ����, ���� ��� �������
			case TART:
			{
				cerr << "���� �����, ���� ������� �� �����" << endl;

				// ����� �������� �� �����, ���� � ����� ������� ��� �����
				// ���� ���� � ����� ������ ������ ���
				if( me.ingridsInHands == NONE || (me.ingridsInHands & DISH && !(me.ingridsInHands & TART)) )
				{
					// ������������ ���� ������������� ���� �����
					me.usedOven = false;

					// ����� ����� ���������� 
					if(me.whatNeedMake == TART)
						me.whatNeedMake = -1;

					moveAsTank(kitchen.ovenCell);
				}
				// ���� � ����� ��� �� ������ - ���������� �� ����
				else
				{
					moveAsTank(kitchen.nearFreeCell);
				}

				break;
			}

			// ���� ����� ������, � ���� me.usedOven == true, �� ��� ��������, ��� ���� ������� � ��� ������
			case NONE:
			{
				findWherePut(me.ingridsInHands);

				// ������������ ���� ������������� ���� �����
				me.usedOven = false;
				break;
			}

			default:
				moveAsTank(kitchen.ovenCell);
		}
	}
	// ���� � ��� �� ����� ������������ �����
	else
	{
		cerr << "����� ��� �� ������������" << endl;

		// ���� ���� ��� �� ������
		if(me.ingridsInHands != NONE)
		{
			findWherePut(me.ingridsInHands);
		}
		// ���� � ����� ������ ���, ���� �����
		else
		{
			// ���� ����� � �����, � � ����� ������� �������, �� ���� �� ������� ������
			if(me.isOvenNear() && (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT))
			{
				cerr << "����� � �������, ������� ������, ��������" << endl;
				moveAsTank(kitchen.ovenCell);
			}
			// ���� � ����� ��� ������ ��������
			else
			{
				// ������� ���� �������� ����� �����
				// ����� �������� �������� �����
				// ���� ��� �� ������ ������ �� ��������� ����� �� ���� �� ������

				pair<int, int> cellWithRawTart   = kitchen.findNearest(RAW_TART);
				pair<int, int> cellWithChpdDough = kitchen.findNearest(CHPD_DOUGH);
				pair<int, int> cellWithDough     = kitchen.findNearest(DOUGH);

				// ���� ���������� ����� ����, ���� �� ���
				if(isExistCell(cellWithRawTart))
					moveAsTank(cellWithRawTart);
				else if(isExistCell(cellWithChpdDough))
					moveAsTank(cellWithChpdDough);
				else moveAsTank(cellWithDough);
			}
		}
	}
};

// ��� ����������� ���� �� ����, ���� �� �������
void collectDesired(){

	// ���� ��� ��� ����� ����� �� ����������, ��� ��� �� �����
	if(me.needSomething())
	{
		cerr << "   ���� ��� ��� �� ������� �� �����" << endl;
		me.takeNeededIngrid();
	}
	// ����� ������ ����� �������
	else
	{
		cerr << "   ����� �������� � ����" << endl;
		moveAsTank(kitchen.winCell);
	}
};

// ��� ����� �� ������������ �� ������, ���� ������� ��, ���� ���
// ���� ���������� ������� ������ �� �������� �����������, ������������ true
// ���� �������� ������ �� �����, ������������ false
bool prepairHardIngrids(){

	cerr << "-------------------------------------" << endl;
	cerr << "���������� ������� ������������" << endl;

	// ���� �� ������ ����� ����
	if(me.desiredCollection & TART)
	{
		cerr << "	�� ������ ����� ����" << endl;

		// ���� � ����� ������� ����, �� ��� ���� �������� ���� �� ������� � ���� ��, ���� �� ������ ����
		if(me.ingridsInHands == TART){

			cerr << "� ���� � ����� ������� ����" << endl;

			findWherePut(TART);
			return true;
		}

		// ���� � ����� ��� ���� �������, �� ����� ������ ������ �������� ������������ �����
		if(me.ingridsInHands & DISH)
		{
			cerr << "		� ����� ���� �������" << endl;

			// ���� � ����� ��� ����� � �� ����� ��� �������� ������������� �����, �� �������� ������ ����
			if( !(me.ingridsInHands & TART) && !isExistCell(kitchen.getCellWithIngridStrongly(TART)) )
			{
				cerr << "		�������� ������ ����" << endl;
				prepairTart();
				return true;
			}
			else
			{
				cerr << "		�� �������� ������ ����" << endl;
			}
		}
		// ���� � ����� ������� ���, �� ����� ������ � ���� �� �������
		else
		{
			cerr << "		� ����� ��� �������" << endl;

			// ����� ���������� ������� � ������ ����� ��� ������������ ���������
			int productWithDishAndTart = -1, minAward = 0;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & TART)			// ���� � �������� ���� ����
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// ����� ���� �������
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // ������� �������� ������ ����� ������
				    && kitchen.existingProductVec[i].ingridCollection > minAward) // ����� ������
				{
					minAward = kitchen.existingProductVec[i].ingridCollection;
					productWithDishAndTart = i;
				}
			}

			if(!((me.ingridsInHands & TART) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ������
				 && !isExistCell(kitchen.getCellWithIngridStrongly(TART)) // ��� ������ ����� � ��������� ������
				 && productWithDishAndTart == -1)// ���� ��� � �� ����� ������� � ������ � ��� ��� ��
			{
				cerr << "		�������� ������ ����" << endl;
				prepairTart();
				return true;
			}
			else
			{
				// ���� ���� ������� � ������ � ��� ��� ��, ����������� �� ������,
				// �� ���� ���������, ����� �� ������� ����� ���� � ��� ��������� ������� ��������

				// ���� �� ������ ����� ���������, � �� ������� � ������ �������� ���, �� ���� ������� ��������
				if(!((me.ingridsInHands & TART) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ������
						&& productWithDishAndTart != -1 // ���� ������� � ������ �� �����
						&& me.desiredCollection & CROISSANT // �� ������ ����� ��������
						&& !(kitchen.existingProductVec[productWithDishAndTart].ingridCollection & CROISSANT) // ������� � ������ �� �������� ��������
					)
				{
					cerr << "		���� ������� � ������, �� �� ��� ��� ��������, ������� ����� �������" << endl;
					prepairCroissant();
					return true;
				}
				// ���� �� ������ ����� ���������, � �� ������� � ������ ��������� ���, �� ���� ������� ���������
				else if(!((me.ingridsInHands & TART) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ������
						&& productWithDishAndTart != -1 // ���� ������� � ������ �� �����
						&& me.desiredCollection & CHPD_STRBR // �� ������ ����� ��������
						&& !(kitchen.existingProductVec[productWithDishAndTart].ingridCollection & CHPD_STRBR) // ������� � ������ �� �������� ���������
					)
				{
					cerr << "		���� ������� � ������, �� �� ��� ��� ���������, ������� ����� ���������" << endl;
					prepairChpdStrbr();
					return true;
				}
				cerr << "		�� �������� ������ ����" << endl;
			}
		}
	}
	else
	{
		me.whatNeedMake = -1;
		cerr << "	���� �� ������ �� �����" << endl;
	}

	// ���� �� ������ ����� ��������
	if(me.desiredCollection & CROISSANT)
	{
		cerr << "	�� ������ ����� ��������" << endl;

		// ���� � ����� ������� ����, �� ��� ���� �������� ���� �� ������� � ���� ��, ���� �� ������ ����
		if(me.ingridsInHands == CROISSANT){

			cerr << "� ���� � ����� ������� ��������" << endl;
			findWherePut(CROISSANT);
			return true;
		}

		// ���� � ����� ��� ���� �������, �� ����� ������ ������ �������� ������������ ���������
		if(me.ingridsInHands & DISH)
		{
			cerr << "		� ����� ���� �������" << endl;

			// ���� � ����� ��� ��������� � �� ����� ��� �������� ������������� ���������, �� �������� ������ ��������
			if( !(me.ingridsInHands & CROISSANT) && !isExistCell(kitchen.getCellWithIngridStrongly(CROISSANT)) )
			{
				cerr << "		�������� ������ ��������" << endl;
				prepairCroissant();
				return true;
			} else
			{
				cerr << "		�� �������� ������ ��������" << endl;
			}
		}
		// ���� � ����� ������� ���, �� ����� ������ � �������� �� �������
		else
		{
			cerr << "		� ����� ��� �������" << endl;

			// ����� ���������� ������� � ���������� ����� ��� ������������ ���������
			int productWithDishAndCrois = -1, minAward = 0;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & CROISSANT)		// ���� � �������� ���� ��������
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// ����� ���� �������
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // ������� �������� ������ ����� ������
					&& kitchen.existingProductVec[i].ingridCollection > minAward) // ����� ������
				{
					//cerr << "		���������� ������� � ��������� � �� ��� ��� ��, ���������� ��� ��� �����: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
					minAward = kitchen.existingProductVec[i].ingridCollection;
					productWithDishAndCrois = i;
				}
			}

			if(!((me.ingridsInHands & CROISSANT) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ����������
				 && !isExistCell(kitchen.getCellWithIngridStrongly(CROISSANT)) // ��� ������ ����� � ��������� ����������
				 && productWithDishAndCrois == -1) // ���� ��� � �� ����� ������� � ���������� � ��� ��� ��
			{
				cerr << "		�������� ������ ��������" << endl;
				prepairCroissant();
				return true;
			} else
			{
				cerr << "		�� �������� ������ ��������" << endl;

				// ���� ���� ������� � ��������� � ��� ��� ��, ����������� �� ������,
				// �� ���� ���������, ����� �� ������� ����� ���� � ��� ��������� ������� ��������

				// ���� �� ������ ����� ����, � �� ������� � ��������� ����� ���, �� ���� ������� ����
				if(!((me.ingridsInHands & CROISSANT) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ���������
						&& productWithDishAndCrois != -1 // ���� ������� � ��������� �� �����
						&& me.desiredCollection & TART // �� ������ ����� ����
						&& !(kitchen.existingProductVec[productWithDishAndCrois].ingridCollection & TART) // ������� � ��������� �� �������� �����
					)
				{
					cerr << "		���� ������� � ���������, �� �� ��� ��� �����, ������� ����� ����" << endl;
					prepairTart();
					return true;
				}
				// ���� �� ������ ����� ���������, � �� ������� � ��������� ��������� ���, �� ���� ������� ���������
				else if(!((me.ingridsInHands & CROISSANT) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ���������
						&& productWithDishAndCrois != -1 // ���� ������� � ��������� �� �����
						&& me.desiredCollection & CHPD_STRBR // �� ������ ����� ���������
						&& !(kitchen.existingProductVec[productWithDishAndCrois].ingridCollection & CHPD_STRBR) // ������� � ��������� �� �������� ���������
					)
				{
					cerr << "		���� ������� � ���������, �� �� ��� ��� ���������� ���������, ������� ����� ���������" << endl;
					prepairChpdStrbr();
					return true;
				}
			}
		}
	}
	else
	{
		me.whatNeedMake = -1;
		cerr << "	�������� �� ������ �� �����" << endl;
	}

	// ���� �� ������ ����� ������� ���������
	if(me.desiredCollection & CHPD_STRBR)
	{
		cerr << "	�� ������ ����� ���������� ���������" << endl;

		// ���� � ����� ���������� ���������, �� �� ���� �������� ���� �� ������� � ���� ��, ���� �� ������ ����
		if(me.ingridsInHands == CHPD_STRBR){

			cerr << "� ���� � ����� ���������� ���������" << endl;

			findWherePut(CHPD_STRBR);
			return true;
		}

		// ���� � ����� ��� ���� �������, �� ����� ������ ������ �������� ������������ �������� ���������
		if(me.ingridsInHands & DISH)
		{
			cerr << "		� ����� ���� �������" << endl;

			// ���� � ����� ��� ���������� ��������� � �� ����� ��� �������� ������������ ���������� ���������, �� �������� ������ ���������� ���������
			if(!(me.ingridsInHands & CHPD_STRBR) && !isExistCell(kitchen.getCellWithIngridStrongly(CHPD_STRBR)))
			{
				cerr << "		�������� ������ ���������� ���������" << endl;
				prepairChpdStrbr();
				return true;
			} else
			{
				cerr << "		�� �������� ������ ���������� ���������" << endl;
			}
		}
		// ���� � ����� ������� ���, �� ����� ������ ���������� ��������� � �� �������
		else
		{
			cerr << "		� ����� ��� �������" << endl;

			// ����� ���������� ������� � ���������� ��������� ����� ��� ������������ ���������
			int productWithDishAndChpadStrbr = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & CHPD_STRBR)	// ���� � �������� ���� ���������� ���������
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// ����� ���� �������
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection))) // ������� �������� ������ ����� ������
				{
					//cerr << "		���������� ������� � ���������� ���������� � �� ��� ��� ��, ���������� ��� ��� �����: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
					productWithDishAndChpadStrbr = i;
				}
			}

			if(!((me.ingridsInHands & CHPD_STRBR) && (me.ingridsInHands & DISH)) // � ����� ��� ������� � ���������� ����������
				 && !isExistCell(kitchen.getCellWithIngridStrongly(CHPD_STRBR)) // ��� ������ ����� � ��������� ���������� ����������
				 && productWithDishAndChpadStrbr == -1) // ���� ��� � �� ����� ������� � ���������� ���������� � ��� ��� ��
			{
				cerr << "		�������� ������ ���������� ���������" << endl;
				prepairChpdStrbr();
				return true;
			} else
			{
				cerr << "		�� �������� ������ ���������� ���������" << endl;
			}
		}
	}
	else
	{
		me.whatNeedMake = -1;
		cerr << "	���������� ��������� �� ������ �� �����" << endl;
	}

	return false;
};

// ������������ ������ �� �������
void makeDesiredCollection(){

	cerr << "-------------------------------------" << endl;
	cerr << "������� ������ �����" << endl;
	
	// ���� �� ��������� ������������� ������� ���������
	if(!prepairHardIngrids())
		// �� ��������� � ��������� ���������
		collectDesired();
}

// ��������� �� �����
void makePrepairings(){

	if(prepChpdStrw < HOW_MUCH_STRWBR_PREP) prepairChpdStrbr();
	else if(prepCrois < HOW_MUCH_CROIS_PREP) prepairCroissant();
	else if(prepTarts < HOW_MUCH_TARTS_PREP) prepairTart();
};

// ���� ������������� �����������, �� ����� �������� �������� �������
void makeSomeOrder(){

	// ���� ���� ������� �������, �� ������ ������� ���� ������ �� ����
	if(saveBaking())
		return;

	// ���� ����� ������� ���������, � � ����� ����� ������ ���� ������� � ������ �� �������������
	// �� ���� ���� ������� ����� ����������� �����, � ������� ������ ��� ���������� � �������
	//		   ����, ���� ������� � ������ ������������� �� �������� �� ��� ������ �� ������� �������,
	//				 �� ������� ������� � �����������

	// ���� ��� �� ������ ����� ��� ����������, �� ����� ����� � ������� �������
	if(me.desiredCollection == -1){
		me.servedClientIdx = clients.getCurClientIdxWithMaxAward();

		// ���� �������� ���� ������ ������� �������, �� ����� ����� � �������� �������
		//if(opponent.servedClientIdx == 1)
		//	me.servedClientIdx = 0;

		me.usedOven = false;
		me.whatNeedMake = -1;

		cerr << "����� �� ������. ������� " << me.servedClientIdx << "-�" << endl;

		// ���������� ���������� ������ � ���������� � ����������
		me.desiredCollection = clients.curClientVec[me.servedClientIdx].ingridCollection;

		makeDesiredCollection();
	}
	// ���� ���� ��������� �����
	else
	{
		// ���� ����� �������������� ������� ���������
		// ����� ����� ����������, ���� �������� �������� ��� �� �����
		// ���� ���� ��� ����� ���� ������� �����
		if(clients.curClientVec.at(me.servedClientIdx).ingridCollection != me.desiredCollection)
		{
			// ���� ����� ��� �����, ������� ��������� ��� �����
			int desiredIdx = -1;
			for(int i = 0; i < clients.curClientNb; i++)
			{
				if(clients.curClientVec[i].ingridCollection == me.desiredCollection)
					desiredIdx = i;
			}

			// ���� ������, ������� �������� ��� ����� ���, ������ �������� ������ ������ ���� �����
			if(desiredIdx == -1){

				cerr << "������������ ������ ������ ���..." << endl;
				me.usedOven = false;
				me.whatNeedMake = -1;

				// ���� � ���� ���� ������� � ������ �� �������������, �� ���� ���� ����� �����,
				// ��� �������� ���������� ���� ������� ��������,
				// ����, ���� ���������� ������� �� �������� �� ��� ������ ������, �� ������ ��
				if(me.ingridsInHands & DISH && me.ingridsInHands > DISH){

					// ��������� ��� ������� ������ �� ���������� � ���� �������������, ��� ���� � ���� � �������
					int orderWithSameIngrids = -1;
					for(int i = 0; i < 3; i++)
						if(PRODUCT::is1PartOf2(me.ingridsInHands, clients.curClientVec.at(i).ingridCollection))
							orderWithSameIngrids = i;

					// ���� ��� ������ �� ������� ���������� ���� ������� ��������, ������ ����� ������� ��������
					if(orderWithSameIngrids != -1){
						cerr << "��� ����������� �������� ��� ������ " << orderWithSameIngrids << endl;
						me.servedClientIdx = orderWithSameIngrids;
						me.desiredCollection = clients.curClientVec.at(me.servedClientIdx).ingridCollection;
						makeDesiredCollection();
					}
					// ���� ��� �������, �������� ������� �� ��� �����������, ���� �������
					else
					{
						cerr << "��� ����������� �� �������� �� ��� ������ ������" << endl;
						// ��� ����� ���� ����� ���� ����� ���������� (���������/�����/���������� �����/����� �����)

						// ���� ��� ���������/�����, ������������
						if(me.ingridsInHands == STRBR || me.ingridsInHands == DOUGH) moveAsTank(kitchen.chopCell);

						// ���� ��� ������� �����, ������ �� ���� ����� �����
						else if(me.ingridsInHands == CHPD_DOUGH) moveAsTank(kitchen.blbrCell);

						// ���� ��� ����� �����, ������ ��� � �������
						else if(me.ingridsInHands == RAW_TART) moveAsTank(kitchen.ovenCell);

						else moveAsTank(kitchen.dishCell);
					}
				}
				// ���� � ����� (��� �������) / (������ �������), �� ���� ������� ����� �����
				else
				{
					// ���� � ��������� � ����� (������ ���) / (������ �������), �� ����� ������� ����� �����
					if(opponent.ingridsInHands <= DISH){

						// ������ ��� ����� ����� ������� �������
						me.servedClientIdx = clients.getCurClientIdxWithMaxAward();
						me.desiredCollection = clients.curClientVec[me.servedClientIdx].ingridCollection;
						cerr << "� ��������� ��� ������, ����� ����� ������� �����: " << me.servedClientIdx << endl;
						makeDesiredCollection();
					}
					// ���� � ��������� ���� ��� �� � �����, �������� ������� ��� �� ������ � �������� ����� ������ �����
					else
					{
						estimateOpponentOrderIdx();
						cerr << "� ��������� ����� " << opponent.servedClientIdx << ", ����� ��������� �� ���" << endl;
						me.servedClientIdx = (opponent.servedClientIdx + 1) % clients.curClientNb;
						me.desiredCollection = clients.curClientVec[me.servedClientIdx].ingridCollection;
						makeDesiredCollection();
					}
				}
			}
			// ���� �����, ������� ��������� ��� ����� ��� ��� ����, �� ���������� ����� ������� � ����������
			else
			{
				cerr << "����� ������������, ������ �� ����� ������ " << desiredIdx << endl;
				me.servedClientIdx = desiredIdx;
				makeDesiredCollection();
			}
		}
		// ���� ����� �� ���������, ������ ��� �� ��������� �����
		else
		{
			cerr << "����� �� ���������, ����� ��� �� " << me.servedClientIdx << endl;
			makeDesiredCollection();
		}
	}
};

// ��������
void makeSomething(){

	// ���� ���� ������� ��������� �� �����
	if(needPrepairing)
	{
		cerr << "���� ������� ���������" << endl;
		makePrepairings();
	}
	// ���� ��� ��������� ���� ������� - �������� ������
	else
	{
		cerr << "���� ������ ������" << endl;
		makeSomeOrder();
	}
};

int main()
{
	std::srand(unsigned(std::time(0)));

	// ��� ������ �� �����
	clients.readAllClientsInput();

	// ������������� �����
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
