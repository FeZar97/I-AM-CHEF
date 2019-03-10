#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

const int K_HEIGHT = 7;
const int K_WIDTH = 11;

// ������ ������������
enum INGRID{
	NONE       = 0x00,		// ������ ���� / ������ ����
	DISH       = 0x01,		// ��������
	BLBR       = 0x02,		// ��������
	ICE_C	   = 0x04,		// ���������
	STRBR	   = 0x08,		// ���������
	CHPD_STRBR = 0x10,		// ����������� ���������

	// �������
	RECIPE1	   = DISH + ICE_C + BLBR,			  // ��������� � ��������
	RECIPE2	   = DISH + ICE_C + CHPD_STRBR,       // ��������� � ���������
	RECIPE3	   = DISH + ICE_C + BLBR + CHPD_STRBR // ��������� + �������� + ���������
};

// �������������� ������ � ��������� �����������
// ����������� � ������� ��������� ��������
char splitProduct2Ingrid(string productString){

	char result = 0x0;
	size_t hyphPos;

	while(!productString.empty()){

		if(productString[0] == 'N') result |= 0x01;	// ������ ���� / ������ ����
		if(productString[0] == 'D') result |= 0x02;	// ��������
		if(productString[0] == 'B') result |= 0x04;	// ��������
		if(productString[0] == 'I') result |= 0x08;	// ���������
		if(productString[0] == 'S') result |= 0x10;	// ���������
		if(productString[0] == 'C') result |= 0x20;	// ����������� ���������

		hyphPos = productString.find("-");
		if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
		else break;
	}

	return result;
};

// ���������� �� ��������� part � ��������� product
// ���� ���� ���� �� ���� ������ ����������, �� false
bool is1ContainedIn2(char part, char product){
	return !((part ^ product) & product);
};

// �������� ������ �����
enum kitchenCellType {
	EMPTY		 = 0x00, // ������
	SOME_PRODUCT = 0x01, // �������, � �.�. ����������
	DISHWASH	 = 0X02, // ����� �������
	WINDOW		 = 0x04, // ����� � ������
	BLBR_CREATE	 = 0x08, // ����� ��������
	ICE_CREATE	 = 0x10, // ����� ���������
	STRAW_CREATE = 0x20, // ����� ���������
	CHOP_BOARD	 = 0x40	 // ����� ��� �����
};
// ����� �������� 7�11
vector<vector<kitchenCellType>> kitchen;
// ������������� �����
void initializeKitchen(){

	string kitchenStroke;
	kitchen.resize(K_HEIGHT);

	for(int i = 0; i < K_HEIGHT; i++){

		cin >> kitchenStroke;
		kitchen[i].resize(K_WIDTH);

		for(int j = 0; j < K_WIDTH; j++){
			switch(int(kitchenStroke[j])){
				case 46: kitchen[i][j] = EMPTY;			break; // ������
				case 68: kitchen[i][j] = DISHWASH;		break; // ����� �������
				case 87: kitchen[i][j] = WINDOW;		break; // ����� � ������
				case 66: kitchen[i][j] = BLBR_CREATE;	break; // ����� ��������
				case 73: kitchen[i][j] = ICE_CREATE;	break; // ����� ���������
				case 83: kitchen[i][j] = STRAW_CREATE;	break; // ����� ���������
				case 67: kitchen[i][j] = CHOP_BOARD;	break; // ����� ��� �����
				default: kitchen[i][j] = SOME_PRODUCT;
			}
		}
	}
};

// ���������� ������� ������
pair<int, int> getCellCoords(kitchenCellType desiredCell){
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			if(kitchen[i][j] == desiredCell) return pair<int, int>{j, i};
};

// ������� (�� ����� / � �����)
class product{
public:
	int x;
	int y;
	char ingridCollection;

	// ���������� �������
	void readInput(){
		string productString;
		cin >> x >> y >> productString;
		ingridCollection = splitProduct2Ingrid(productString);
	}
};

// ���������� �� ����� �� ���������� ��������
int existingProductNb;
vector<product> existingProductVec;

// �������� ��������� �� ����������
bool isCoordsValid(pair<int, int> p){
	return p.first != -1 && p.second != -1;
};

// ���������� ���������� ������ ����� � ������� ���� ��������� ������������ ingridCollection
pair<int, int> getCellWithIngridCollection(char ingridCollection){
	
	for(auto &p : existingProductVec)
		if(is1ContainedIn2(ingridCollection, p.ingridCollection)) return pair<int, int>{p.x, p.y};
	return pair<int, int>{-1, -1};
};

// ���������� ���������� �����, ���������� � ���������� �����������
pair<int, int> getFreeTableCoord(int x, int y){
	// ����������� 8 ���������� ������
	for(int i = y - 1; i <= y + 1; i++)
		for(int j = x - 1; j <= x + 1; j++)
			// ���� ��� �� ������� �����
			if(kitchen[i][j] == SOME_PRODUCT)
				// ����� ����� ����� ����������� ������������ existingProductVec
				for(auto &ingrs: existingProductVec)
					if(ingrs.x != j && ingrs.y != x)
						return pair<int, int>{j, i};
};

// ������ = {�����, �����}
class customer{
public:
	char ingridCollection;
	int price;

	// ���������� �������
	void readInput(){
		string productString;
		cin >> productString >> price;
		ingridCollection = splitProduct2Ingrid(productString);
	}
};

// ������� �� ���� ����
int allCustomerNb;
vector<customer> allCustomerVec;

// ������� �������
int curCustomerNb;
vector<customer> curCustomerVec;

// �����
class player{
public:
	int x;
	int y;
	char ingridCollection; // ��� ����� � �����

	char desiredCollection = 0; // ������, ��������� ��� �������������
	bool desiredReady = true; // ���������� ������

	void read(){
		string productString;
		cin >> x >> y >> productString;
		ingridCollection = splitProduct2Ingrid(productString);
	}

	// ���� �� � ��������� ����� ����������
	bool haveIngrid(char ingrid){
		return ingrid & ingridCollection;
	}

	// ���������� ����������� ���������
	char needingIngrids(char desiredProduct){
		return (desiredProduct & ingridCollection) ^ desiredProduct;
	}

} me, opponent;

// ���������� ���-�� �����
int turnsRemaining;

// ���������� �������
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

	// ������� �������
	cin >> curCustomerNb;
	curCustomerVec.resize(curCustomerNb);
	for(auto &cust : curCustomerVec) cust.readInput();
};

// ������� ���������� ��������� ������
void use(kitchenCellType cellType){

	pair<int, int> cellCoords = getCellCoords(cellType);
	cout << "USE " << cellCoords.first << " " << cellCoords.second << endl;
};

// ������� ���������� ����������
void use(pair<int, int> p){
	if(isCoordsValid(p)) cout << "USE " << p.first << " " << p.second << endl;
};

// ������������ ������ �� �������
void makeDesiredCollection(){

	switch(me.desiredCollection){

		// ������ 1 = ��������� � ���������
		case(RECIPE1):
			//cerr << "i will made rec1" << endl;
			if(is1ContainedIn2(BLBR , me.ingridCollection))  use(WINDOW);
			if(is1ContainedIn2(ICE_C, me.ingridCollection))  use(BLBR_CREATE);
			if(is1ContainedIn2(DISH,  me.ingridCollection))  use(ICE_CREATE);
			if(me.ingridCollection == NONE)  use(DISHWASH);
			break;

		// ������ 2 = ��������� � ����������
		case(RECIPE2):
			//cerr << "i will made rec2" << endl;
			if(is1ContainedIn2(ICE_C     , me.ingridCollection)) use(WINDOW);
			if(is1ContainedIn2(CHPD_STRBR, me.ingridCollection)) use(ICE_CREATE);

			// ���� ���� ������ � ��������� ����������
			if(isCoordsValid(getCellWithIngridCollection(CHPD_STRBR))){

				// ���� � ����� ���� �������, �� ���� �� ����������
				if(is1ContainedIn2(DISH, me.ingridCollection)) use(getCellWithIngridCollection(CHPD_STRBR));
				// ����� ����� �������
				else use(DISHWASH);
			}else{
			// ���� ���������� ��������� ���...

				// ���� � ����� ���������, ���� �� ������
				if(me.ingridCollection == STRBR) use(CHOP_BOARD);
				else
					// ���� � ����� ��� �� ���� � ��� �� ���������, �� ������ ��� �� ��������� ������ �����
					if(me.ingridCollection != NONE) use(getFreeTableCoord(me.x, me.y));
					// ���� � ����� ������ ���, �� ����� ���������
					else use(STRAW_CREATE);
			}
			break;

		// ������ 3 = ��������� � ��������� � ����������
		case(RECIPE3):
			//cerr << "i will made rec3" << endl;
			if(is1ContainedIn2(BLBR, me.ingridCollection)) use(WINDOW);
			if(is1ContainedIn2(ICE_C, me.ingridCollection)) use(BLBR_CREATE);
			if(is1ContainedIn2(CHPD_STRBR, me.ingridCollection)) use(ICE_CREATE);

			// ���� ���� ������ � ��������� ����������
			if(isCoordsValid(getCellWithIngridCollection(CHPD_STRBR))){

				// ���� � ����� ���� �������, �� ���� �� ����������
				if(is1ContainedIn2(DISH, me.ingridCollection)) use(getCellWithIngridCollection(CHPD_STRBR));
				// ����� ����� �������
				else use(DISHWASH);
			} else{
				// ���� ���������� ��������� ���...

				// ���� � ����� ���������, ���� �� ������
				if(me.ingridCollection == STRBR) use(CHOP_BOARD);
				else
					// ���� � ����� ��� �� ���� � ��� �� ���������, �� ������ ��� �� ��������� ������ �����
					if(me.ingridCollection != NONE) use(getFreeTableCoord(me.x, me.y));
				// ���� � ����� ������ ���, �� ����� ���������
					else use(STRAW_CREATE);
			}
			break;
		default:
			cout << "WAIT" << endl;
	}
}

// ��������
void makeSomething(){

	// 0. ���� ������ ��� ��������� �����, �� ����� ����� � ������� �������
	if( (me.desiredCollection == 0 && me.desiredReady)
		|| // ���
			  // 1. ���� ����� ������� ������� ���������, �� me.desiredCollection �������� ������ ������ ������� �������,
		      //    �� ������ me.desiredCollection �� ����� ������� �������
		(me.desiredCollection != curCustomerVec.at(1).ingridCollection && is1ContainedIn2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)) )
	{
		me.desiredCollection = curCustomerVec.at(1).ingridCollection;
		me.desiredReady = false;
	}else{
		// 2. ���� ����� ������� ������� ���������, � ��������� me.desiredCollection �� �������� �� ��� ������ �� ������� �������,
		//    �� ��������� me.desiredCollection � dishwasher
		if(me.desiredCollection != curCustomerVec.at(1).ingridCollection && !is1ContainedIn2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)){
			use(DISHWASH);
			return;
		}
	}

	makeDesiredCollection();
};

int main()
{
	// ������
	cin >> allCustomerNb;
	allCustomerVec.resize(allCustomerNb);
	for(auto &client : allCustomerVec) client.readInput();

	initializeKitchen();

	while(1){
		readInput();

		makeSomething();
	}
}