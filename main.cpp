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
		//cerr << "on " << x << " " << y << " exist " << ingrid2String(ingridCollection) << endl;
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

void use(kitchenCellType);
void use(pair<int,int>);
void use(string);

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

	// ����� �������� 7�11
	vector<vector<kitchenCellType>> cellArray;

	// ������ ���������� �� ���� ���� ����� ������������ ����� ������
	short distArrayForMe[K_HEIGHT][K_WIDTH];

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
	pair<int, int> getNearFreeTable(int x, int y, int oppX, int oppY){

		// ����������� 8 ���������� ������
		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				// ���� ��� ���� (�������� � ����� �� ���������, ����������� �����)
				if(cellArray[i][j] == SOME_PRODUCT){

					bool isTableFree = true;
					// ����� ����� ����� ����������� ������������ existingProductVec ������ i j
					// ���� � ����������� ������������ ����� ������ ���, �� �� ��� ����� ��� �� ��������
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

		// ���� ����� �� ����, ������ � ��������� 8 ������� �� ������ ��� ��������� ������
		// ���� ����� ��� ��������� ������ ������ ������ (x,y) � ��� ������ �� ��� ���������� ����� ��������� ���� � ��������� 8 ������

		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
			{
				// ���� ��� ��������� ������ � �� ��� ��� ���������
				// �� ��������� ��� �� ���������
				if(cellArray[i][j] == EMPTY && !(oppX == j && oppY == i)){

					// ����������� 8 ���������� ������ ����� ��������� ��������� ������
					for(int k = i - 1; k <= i + 1; k++)
						for(int l = j - 1; l <= j + 1; l++)
							// ���� ��� ���� (�������� � ����� �� ���������, ����������� �����)
							if(cellArray[k][l] == SOME_PRODUCT){

								bool isTableFree = true;
								// ����� ����� ����� ����������� ������������ existingProductVec ������ i j
								// ���� � ����������� ������������ ����� ������ ���, �� �� ��� ����� ��� �� ��������
								for(auto &ingrs : existingProductVec)
									if(ingrs.x == l && ingrs.y == k) isTableFree = false;

								if(isTableFree) return pair<int, int>{l, k};
							}
				}
			}

		return pair<int, int>{-1, -1};
	};

	// ���� �� ����� ���� ��� �����������, ���������� true,
	// ���� ���� �� ������ ����������� �� ����� ���, �� ���������� false
	bool existAllHardIngrids(short desiredCollection, short ingridsInHands){

		// ���� �� ������ ����� ��������
		if(desiredCollection & CROISSANT)
		{
			cerr << "����� ��������" << endl;
			// ���� ��� ��� �� � �����, �� �������� �������� � ��� ������� � ����������(��� ���� ��������� ����������� ���� ������ ���������)
			
			int productWithDishAndCrois = -1;

			// ����� ���������� ������� � ���������� ����� ��� ������������ ���������
			for(int i = 0; i < existingProductNb; i++)
			{
				if((existingProductVec[i].ingridCollection & CROISSANT)
				   && (existingProductVec[i].ingridCollection & DISH)
				   && (PRODUCT::is1PartOf2(existingProductVec[i].ingridCollection, desiredCollection)))
				{
					cerr << "���������� ������� � ��������� � �� ��� ��� ��, ���������� ��� ��� �����: " << existingProductVec[i].x << " " << existingProductVec[i].y << endl;
					productWithDishAndCrois = i;
				}
			}

			if(!((ingridsInHands & CROISSANT) && (ingridsInHands & DISH)) // � ����� ��� ������� � ���������
				 && !isExistCell(getCellWithIngridStrongly(CROISSANT)) // ��� ������ ����� � ��������� ���������
				 && productWithDishAndCrois == -1)// ���� ��� � �� ����� ������� � ��������� � ��� ��� ��
			{
				return false;
			}
		}

		// ���� (�� ������ ����� CHPD_STRBR) � (�� ����� �� ���) � (� ����� �� ��� �� �������)
		if( (desiredCollection & CHPD_STRBR) && (!isExistCell(getCellWithIngridStrongly(CHPD_STRBR))) && ( !((ingridsInHands & CHPD_STRBR) && (ingridsInHands & DISH))) )
		{
			cerr << "����� ���������" << endl;
			return false;
		}

		// ���� �� ������ ����� TART � �� ����� ��� ��� � � ����� ��� ��� �� �������
		if((desiredCollection & TART) && (!isExistCell(getCellWithIngridStrongly(TART))) && ( !((ingridsInHands & TART) && (ingridsInHands & DISH))) )
		{
			cerr << "����� ����" << endl;
			return false;
		}

		return true;
	};

	// ��������� � ����� ������ ��������� ������ �����
	pair<int, int> nearFreeCell = {-1, -1};

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
				use(kitchen.nearFreeCell);
				return;
			}
			else
			{
				// ���� ������� ���� ��� ������������ �� ����� ���������
				for(auto &product : kitchen.existingProductVec)
					if(PRODUCT::is1PartOf2(product.ingridCollection, desiredCollection) && !PRODUCT::is1PartOf2(product.ingridCollection, ingridsInHands) && (product.ingridCollection & DISH))
					{
						cerr << "���� ������ ������� � ��������" << endl;
						use(pair<int, int>{product.x, product.y});
						return;
					}

				cerr << "��� ������ �����������, ����� �� �����������" << endl;
				// ���� �� ����� � ������������, ���� � �����������
				use(DISHWASH);
				return;
			}
		}
		// ���� ������� � ����� ����, �� ����� �������� ������ �� ��������, ������� ��� �������
		else
		{
			// ���� ����� ��������� � ���� ��� �����

			int minDist = 777, minProductIdx = -1;

			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				// ���� ������������ ������� �������� ������ �������, ������������ �� ������������� �������� ��� ��� � ����� � �������� �� �������� �������
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
				cerr << "���� �������, ���������� ����� �������..." << endl;
				use(pair<int, int>{kitchen.existingProductVec[minProductIdx].x, kitchen.existingProductVec[minProductIdx].y});
				return;
			}

			/*
			for(auto &product : kitchen.existingProductVec)
				// ���� ������������ ������� �������� ������ �������, ������������ �� ������������� �������� ��� ��� � ����� � �������� �� �������� �������
				if((product.ingridCollection & desiredCollection) && !(product.ingridCollection & ingridsInHands) && !(product.ingridCollection & DISH))
				{
					cerr << product.ingridCollection << " " << desiredCollection << " " << ingridsInHands << endl;
					cerr << "���� �������, ���������� ����� �������..." << endl;
					use(pair<int, int>{product.x, product.y});
					return;
				}
				*/
		}

		// ���� ����� �� ����, ��� �������� ��� �������� ����� ������ ����������� �� �������: ��������/���������
		if((BLBR & desiredCollection) && !(BLBR & ingridsInHands))
		{
			cerr << "����� ��������" << endl;
			use(BLBR_CREATE);
		}
		else
		{
			if((ICE_C & desiredCollection) && !(ICE_C & ingridsInHands))
			{
				cerr << "����� ���������" << endl;
				use(ICE_CREATE);
			}
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

// ������ ���������� �� ����� ����� ������ �� ���� ������ �����
void calcDistances(){

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
				if( (kitchen.distArrayForMe[i][j] < 222) && (kitchen.cellArray[i][j] == EMPTY) )
				{
					//cerr << "(" << j << "," << i << ") = " << kitchen.distArrayForMe[i][j] << endl;

					// ����������� ������ �������4 ������
					// ���� ���������� �� ������ ��� �� ��������, ��� ��������� � � ��� ��� ���������, �� ���������

					// �����
					if( (kitchen.distArrayForMe[i][j - 1] > 333) && (kitchen.cellArray[i][j - 1] == EMPTY) && !(i == opponent.y && (j - 1) == opponent.x) ){
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
					if(kitchen.distArrayForMe[i + 1][j] > 333 && kitchen.cellArray[i + 1][j] == EMPTY && !( (i + 1) == opponent.y && j == opponent.x)){
						kitchen.distArrayForMe[i + 1][j] = kitchen.distArrayForMe[i][j] + 1;
						nonChekedCellsNb--;
						//cerr << "	4. (" << j << "," << i + 1 << ") = " << kitchen.distArrayForMe[i + 1][j] << endl;
					}
				}
		/*
		cerr << "���������� �� ��������� �����:" << endl;
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

		needPrepairing = PREPAIRING_FLAG;
		me.usedOven = false;
		prepChpdStrw = 0;
		prepCrois = 0;
		prepTarts = 0;
	}

	// ���� ��� ��������� ���� �������
	if(needPrepairing && prepChpdStrw == 2 && prepCrois == 1) needPrepairing = false;

	me.read();
	opponent.read();

	// ������������ ������� ����������� �� �����
	cin >> kitchen.existingProductNb;
	kitchen.existingProductVec.clear();
	kitchen.existingProductVec.resize(kitchen.existingProductNb);
	for(auto &prod: kitchen.existingProductVec) prod.readInput();

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
	kitchen.nearFreeCell = kitchen.getNearFreeTable(me.x, me.y, opponent.x, opponent.y);
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

void use(string command){
	cout << command + "; " + get_random_spell() << endl;
	cerr << command << endl;
};

// �������� �� ����� ���������� ���������
void prepairChpdStrbr(){

	cerr << "������� ���������" << endl;

	// ���� ���� ������, ����� ������� ���������
	if(me.ingridsInHands == NONE) use(STRBR_CREATE);
	// ���� � ����� ������� ���������, ����� ��
	else if(me.ingridsInHands == STRBR) use(CHOP_BOARD);
	// ���� � ����� �� ������� ���������, �� ������ ��� �� ����
	else
	{
		// ���� � ����� ���������� ���������, �� ����������� ������� �������������� ���������
		if(me.ingridsInHands == CHPD_STRBR) prepChpdStrw++;
		use(kitchen.nearFreeCell);
	};
};

// �������� �� ����� ���������
void prepairCroissant(){

	// ���� � ��� ����������� �����, �� ���� ���� ����� ������������� ���������, ���� ������� ������� ���� �� �����
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

				pair<int, int> dishCell = kitchen.findNearest(DISH);

				if(me.ingridsInHands == NONE)
				{
					cerr << "� ����� ������ ���" << endl;

					int minAward = 0;
					int prodIdx = -1;

					// ���� ����� ������� � ����������� �������������, ���, ���� ����� ���, ����� ������
					for(int i = 0; i < kitchen.existingProductNb; i++)
					{
						if((PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection)) // ���� ��� ����� ����� ������
						   && (kitchen.existingProductVec[i].ingridCollection & DISH)  // ���� ������� �������� �������
						   && (kitchen.existingProductVec[i].ingridCollection > minAward)) // ����� ������ �������
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// ���� ����� ������� � ��� �� ������
					if(prodIdx != -1)
					{
						cerr << "���� ����������� ������� � ����������..." << endl;
						use(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
					// ���� �� ����� ������� � ��� �� ����������, ����� ������
					else
						use(dishCell);

					return;
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "� ����� �������, ���� � �����" << endl;
					use(OVEN);
					return;
				} else
				{
					use(kitchen.nearFreeCell);
				}

				break;
			}
				// ���� � ����� ������� �������, ���� ��� �������
			case CROISSANT:
			case TART:
				cerr << "������� ������, ���� �������" << endl;
				// ���� � ����� ��� ������� / �� ������� ��� ���� ��������
				if( !(me.ingridsInHands & DISH) || (me.ingridsInHands & CROISSANT) )
					use(kitchen.nearFreeCell);
				// ���� ���� ������, �� ���� ������� �������
				else
					use(OVEN);
				
				break;

				// ���� ����� ������, � ���� me.usedOven == true, �� ��� ��������, ��� ���� �������� � ��� ������
			case NONE:
				cerr << "������� � �����, ������ �� �� ����" << endl;
				// ���� � ����� �������������� ��������, �� ������� �������������� ���������� �������������
				if(me.ingridsInHands == CROISSANT) ++prepCrois;
				// ���� � ����� �������������� ����, �� ������� �������������� ���������� �������������
				else if(me.ingridsInHands == TART) ++prepTarts;

				// ������������ ���� ������������� ���� �����
				me.usedOven = false;
				use(kitchen.nearFreeCell);
				break;

			default:
				use(OVEN);
		}
	}
	// ���� � ��� �� ����� ������������ �����
	else
	{
		cerr << "����� ��� �� ������������" << endl;

		// ���� ����� � ����� ��� �� ���� ��������, �� ���� ����� ��� ����� � �������� � �����

		// ���� ���� ��� �� ������
		if(me.ingridsInHands != NONE)
		{
			// ���� � ����� ��� ���� �����, �� ���� ����� � �����
			if(me.ingridsInHands == DOUGH)
			{
				cerr << "� ����� ���� �����" << endl;
				// ���� ����� ����� �����
				if(me.isOvenNear())
				{
					cerr << "����� ����� �����" << endl;
					// ���� ������� ��������, � � ����� �������, ������� ����� ������, �� ��� ����� �� ��������
					if(kitchen.ovenContents == NONE && (me.ingridsInHands == DOUGH || me.ingridsInHands == RAW_TART))
					{
						cerr << "�������� �����, ������� �������" << endl;
						me.usedOven = true;
						use(OVEN);
					}
					// ���� � ����� ��� �� ����, �� ����������� �����
					else
						use(kitchen.nearFreeCell);
				}
				// ���� �� � �������
				else
				{
					cerr << "����� �� � �����, ���� � �����" << endl;
					use(OVEN);
				}
			}
			// ���� � ����� ��� �� ������ - ����������� �� �������� ����
			else
			{
				cerr << "� ����� ��� �� ������..." << endl;
				// ���� �������� �������, ����������� �������
				if(me.ingridsInHands == CROISSANT) ++prepCrois;
				// ���� �������� �������, ����������� �������
				else if(me.ingridsInHands == TART) ++prepTarts;
				use(kitchen.nearFreeCell);
			}
		}
		// ���� � ����� ������ ���, ���� ����� �����
		else
		{
			// ���� ����� � �����, � � ����� ������� �������, �� ���� �� ������� ������
			if(me.isOvenNear() && (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT))
			{
				cerr << "����� � �������, ������� ������, ��������" << endl;
				use(OVEN);
			}
			// ���� � ����� ��� ������ ��������
			else
			{
				// ���� ������������ ����� / ������� �����
				use(kitchen.findNearest(DOUGH));
			}
		}
	}
};

// �������� �� ����� �����
void prepairTart(){

	// ���� � ��� ����������� �������, �� ���� ���� ����� ������������� �����, ���� ������� ���� ���� �� �����
	if(me.usedOven)
	{
		cerr << "����� ��� ������������" << endl;

		pair<int, int> dishCell = kitchen.findNearest(DISH);

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
						   && (kitchen.existingProductVec[i].ingridCollection > minAward)  ) // ����� ������ �������
						{
							minAward = kitchen.existingProductVec[i].ingridCollection;
							prodIdx = i;
						}
					}

					// ���� ����� ������� � ��� �� ������
					if(prodIdx != -1)
					{
						cerr << "���� ����������� ������� � ����������..." << endl;
						use(pair<int, int>{kitchen.existingProductVec[prodIdx].x, kitchen.existingProductVec[prodIdx].y});
					}
					// ���� �� ����� ������� � ��� �� ����������, ����� ������
					else
						use(dishCell);

					return;
				}
				else if(me.ingridsInHands & DISH)
				{
					cerr << "� ����� �������, ���� � �����" << endl;
					use(OVEN);
					return;
				} else
				{
					use(kitchen.nearFreeCell);
					return;
				}

				break;
			}

			// ���� � ����� ������� �������, ���� ��� �������
			case CROISSANT:
			case TART:
			{
				cerr << "������� ������, ���� �������" << endl;
				// ���� � ����� ���� ������� � ��� ��, � �� ������� ��� ���� ����
				// ���� ���� � ����� ��� �� ����� �������
				if(me.ingridsInHands > DISH && me.ingridsInHands & TART)
					use(kitchen.nearFreeCell);
				// ���� ���� ������, �� ���� ������� �������
				else
					use(OVEN);

				break;
			}
			// ���� ����� ������, � ���� me.usedOven == true, �� ��� ��������, ��� ���� ���� � ��� ������
			case NONE:
				cerr << "������� � �����, ������ �� �� ����" << endl;
				// ���� � ����� �������������� ����, �� ������� �������������� ������ �������������
				if(me.ingridsInHands == CROISSANT) ++prepCrois;
				// ���� � ����� �������������� ����, �� ������� �������������� ���������� �������������
				else if(me.ingridsInHands == TART) ++prepTarts;

				// ������������ ���� ������������� ���� �����
				me.usedOven = false;
				use(kitchen.nearFreeCell);
				break;

			default:
				use(OVEN);
		}
	}
	// ���� � ��� �� ����� ������������ �����
	else
	{
		cerr << "����� ��� �� ������������" << endl;
		// ���� ����� � ����� ��� �� ���� ��������, �� ���� ����� ��� ����� � �������� � �����

		// ���� ���� ��� �� ������
		if(me.ingridsInHands != NONE){

			switch(me.ingridsInHands){
				
				// ���� � ����� ����� - ����� �� �����
				case DOUGH:
					use(CHOP_BOARD);
					break;

				// ���� � ����� �������� �����, ���� ��� ���������
				case CHPD_DOUGH: 
					use(BLBR_CREATE);
					break;

				// ���� ����� ����
				case RAW_TART:
					// ���� ����� � �����
					if(me.isOvenNear())
					{
						cerr << "����� � �����" << endl;
						// ���� ������� ��������, �� ����� ��������
						if(kitchen.ovenContents == NONE)
						{
							cerr << "�������� �����, ������� �������" << endl;
							me.usedOven = true;
							use(OVEN);
						}
						// ���� ����� ������
						else
						{
							cerr << "����� ������, � ����� ����� ����, ���������� �����" << endl;
							use(kitchen.nearFreeCell);
						}
					}
					// ���� ����� �� � ������� - �� ���� � �������
					else
					{
						cerr << "����� �� � �����, ���� � �����" << endl;
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
		// ���� � ����� ������ ���, ���� �����
		else
		{
			// ���� ����� � �����, � � ����� ������� �������, �� ���� �� ������� ������
			if(me.isOvenNear() && (kitchen.ovenContents == TART || kitchen.ovenContents == CROISSANT))
			{
				cerr << "����� � �������, ������� ������, ��������" << endl;
				use(OVEN);
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
					use(cellWithRawTart);
				else if(isExistCell(cellWithChpdDough))
					use(cellWithChpdDough);
				else use(cellWithDough);
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
		use(WINDOW);
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

		// ���� � ����� ��� ���� �������, �� ����� ������ ������ �������� ������������ �����
		if(me.ingridsInHands & DISH)
		{
			cerr << "		� ����� ���� �������" << endl;

			// ���� � ����� ��� ����� � �� ����� ��� �������� ������������� �����, �� �������� ������ �����
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
			int productWithDishAndTart = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & TART)			// ���� � �������� ���� ����
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// ����� ���� �������
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection))) // ������� �������� ������ ����� ������
				{
					//cerr << "		���������� ������� � ��������� � �� ��� ��� ��, ���������� ��� ��� �����: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
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
			} else
			{
				cerr << "		�� �������� ������ ����" << endl;
			}
		}
	}
	else
	{
		cerr << "	���� �� ������ �� �����" << endl;
	}


	// ���� �� ������ ����� ��������
	if(me.desiredCollection & CROISSANT)
	{
		cerr << "	�� ������ ����� ��������" << endl;

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
			int productWithDishAndCrois = -1;
			for(int i = 0; i < kitchen.existingProductNb; i++)
			{
				if((kitchen.existingProductVec[i].ingridCollection & CROISSANT)		// ���� � �������� ���� ��������
					&& (kitchen.existingProductVec[i].ingridCollection & DISH)		// ����� ���� �������
					&& (PRODUCT::is1PartOf2(kitchen.existingProductVec[i].ingridCollection, me.desiredCollection))) // ������� �������� ������ ����� ������
				{
					//cerr << "		���������� ������� � ��������� � �� ��� ��� ��, ���������� ��� ��� �����: " << kitchen.existingProductVec[i].x << " " << kitchen.existingProductVec[i].y << endl;
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
			}
		}
	} else
	{
		cerr << "	�������� �� ������ �� �����" << endl;
	}

	// ���� �� ������ ����� ������� ���������
	if(me.desiredCollection & CHPD_STRBR)
	{
		cerr << "	�� ������ ����� ���������� ���������" << endl;

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
	} else
	{
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
			for(int i = 0; i < 3; i++)
				if(clients.curClientVec[i].ingridCollection == me.desiredCollection)
					desiredIdx = i;

			// ���� ������, ������� �������� ��� ����� ���, ������ �������� ������ ������ ���� �����
			if(desiredIdx == -1){

				cerr << "������������ ������ ������ ���..." << endl;

				// ���� � ���� ���� ������� � ������ �� �������������, �� ���� ���� ����� �����,
				// ��� �������� ���������� ���� ������� ��������,
				// ����, ���� ���������� ������� �� �������� �� ��� ������ ������, �� ������ ��
				if(me.ingridsInHands > DISH){

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
						if(me.ingridsInHands == STRBR || me.ingridsInHands == DOUGH) use(CHOP_BOARD);

						// ���� ��� ������� �����, ������ �� ���� ����� �����
						else if(me.ingridsInHands == CHPD_DOUGH) use(BLBR_CREATE);

						// ���� ��� ����� �����, ������ ��� � �������
						else if(me.ingridsInHands == RAW_TART) use(OVEN);

						else use(DISHWASH);
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
