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
	NONE       = 0x0000,		// ������ ���� / ������ ����
	DISH       = 0x0001,		// ��������
	BLBR       = 0x0002,		// ��������
	ICE_C	   = 0x0004,		// ���������
	STRBR	   = 0x0008,		// ���������
	CHPD_STRBR = 0x0010,		// ����������� ���������
	DOUGH	   = 0x0020,		// �����
	CROISSANT  = 0x0040,		// ��������

	// ������� WOOD 3
	RECIPE1	   = DISH + ICE_C + BLBR,							// ��������� + ��������
	RECIPE2	   = DISH + ICE_C + CHPD_STRBR,						// ��������� + ���������
	RECIPE3    = DISH + BLBR  + CHPD_STRBR,						// ��������  + ���������

	// ������� WOOD 2
	RECIPE4	   = DISH + ICE_C + BLBR + CHPD_STRBR,				// ��������� + �������� + ���������

	// ������� WOOD 1
	RECIPE5	   = DISH + CROISSANT + ICE_C,						// �������� + ���������
	RECIPE6	   = DISH + CROISSANT + BLBR + ICE_C,				// �������� + �������� + ���������
	RECIPE7	   = DISH + CROISSANT + CHPD_STRBR + BLBR,			// �������� + ��������� + ��������
	RECIPE8	   = DISH + CROISSANT + CHPD_STRBR + ICE_C,			// �������� + ��������� + ���������
	RECIPE9	   = DISH + ICE_C + BLBR + CHPD_STRBR + CROISSANT	// �������� + ��������� + ��������� + ��������
};

// �������������� ������ � ��������� �����������
// ����������� � ������� ��������� ��������
short splitProduct2Ingrid(string productString){

	short result = 0x0;
	size_t hyphPos;

	while(!productString.empty()){

		if(productString.substr(0, 2) == "NO") result |= 0x0000;	// ������ ���� / ������ ����
		if(productString.substr(0, 2) == "DI") result |= 0x0001;	// ��������
		if(productString.substr(0, 2) == "BL") result |= 0x0002;	// ��������
		if(productString.substr(0, 2) == "IC") result |= 0x0004;	// ���������
		if(productString.substr(0, 2) == "ST") result |= 0x0008;	// ���������
		if(productString.substr(0, 2) == "CH") result |= 0x0010;	// ����������� ���������
		if(productString.substr(0, 2) == "DO") result |= 0x0020;	// �����
		if(productString.substr(0, 2) == "CR") result |= 0x0040;	// ��������

		hyphPos = productString.find("-");
		if(hyphPos != string::npos) productString = productString.erase(0, hyphPos + 1);
		else break;
	}
	return result;
};

// ���������� �� ��������� part � ��������� product
// ���� � part ���� ���� �� ���� ������ ����������, �� ������������ ����
bool is1PartOf2(short part, short product){
	return ((part ^ product) & product);
};

// ����� �� ���������� ingrid ��� product 
bool is1NeedFor2(short part, short product){
	return part & product;
};

// �������� ������ �����
enum kitchenCellType {
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
				case 48: // ����� 0
				case 49: // ����� 1
				case 50: // ����� 2
				case 46: kitchen[i][j] = EMPTY;			break; // . ������
				case 68: kitchen[i][j] = DISHWASH;		break; // D ����� �������
				case 87: kitchen[i][j] = WINDOW;		break; // W ����� � ������
				case 66: kitchen[i][j] = BLBR_CREATE;	break; // B ����� ��������
				case 73: kitchen[i][j] = ICE_CREATE;	break; // I ����� ���������
				case 83: kitchen[i][j] = STRBR_CREATE;	break; // S ����� ���������
				case 67: kitchen[i][j] = CHOP_BOARD;	break; // C ����� ��� �����
				case 72: kitchen[i][j] = DOUGH_CREATE;	break; // H ����� �����
				case 79: kitchen[i][j] = OVEN;			break; // O �����
				default: kitchen[i][j] = SOME_PRODUCT;		   // ������� / ������ ����
			}
		}
		cerr << endl;
	}
};
// ������� ������������ � ������
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

// ���������� ������� ������
pair<int, int> getCellWithAppliance(kitchenCellType desiredCell){
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			if(kitchen[i][j] == desiredCell) return pair<int, int>{j, i};
};

// ������� (�� ����� / � �����)
class product{
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
};

// ���������� �� ����� �� ���������� ��������
int existingProductNb;
vector<product> existingProductVec;

// �������� ��������� �� ����������
bool isCoordsValid(pair<int, int> p){
	return p.first != -1 && p.second != -1;
};

// ���������� ���������� ������ ����� � ������� ���� ingrid (�������� � ��� �� ��� ����� ingrid)
pair<int, int> getCellWithIngrid(short ingrid){
	for(auto &p : existingProductVec)
		// ���� ������ ����� �������� � �������� ��, ��� ����������, ����� ��
		if(p.ingridCollection & ingrid) return pair<int, int>{p.x, p.y};
	return pair<int, int>{-1, -1};
};

// ���� ���������� ����������, � �������� ���������� � �������
bool existStrongly(short ingrid){

	for(auto &p : existingProductVec)
		// ���� ������ ����� �������� � �������� ��, ��� ����������
		if(!((p.ingridCollection & ingrid) ^ ingrid))
			return true;
	return false;
};
// ���������� ���������� ������ ����� � ������� ���� ������ ingrid
pair<int, int> getCellWithIngridStrongly(short ingrid){
	// ���� ������ ����� �������� � �������� ��, ��� ����������
	for(auto &p : existingProductVec)
		if(!((p.ingridCollection & ingrid) ^ ingrid))
			return pair<int, int>{p.x, p.y};
};

// ���������� ���������� �����, ���������� � ���������� �����������
pair<int, int> getFreeTableCoord(int x, int y){
	// ����������� 8 ���������� ������
	for(int i = y - 1; i <= y + 1; i++)
		for(int j = x - 1; j <= x + 1; j++)
			// ���� ��� ���� (�������� � ����� �� ���������, ����������� �����)
			if(kitchen[i][j] == SOME_PRODUCT){

				bool isTableFree = true;
				// ����� ����� ����� ����������� ������������ existingProductVec ������ i j
				// ���� � ����������� ������������ ����� ������ ���, �� �� ��� ����� ��� �� ��������
				for(auto &ingrs : existingProductVec)
					if(ingrs.x == j && ingrs.y == i) isTableFree = false;

				if(isTableFree) return pair<int, int>{j, i};
			}
};

// ������ = {�����, �����}
class customer{
public:
	short ingridCollection;
	int price;

	// ���������� �������
	void readInput(){
		string productString;
		cin >> productString >> price;
		ingridCollection = splitProduct2Ingrid(productString);
		//cerr << productString << " is equal " << int(ingridCollection) << endl;
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
	short ingridsInHands; // ��� ����� � �����

	short desiredCollection = 0; // ������, ��������� ��� �������������
	bool desiredReady = true; // ���������� ������

	int ovenReadyTurn = -1;

	void read(){
		string productString;
		cin >> x >> y >> productString;
		ingridsInHands = splitProduct2Ingrid(productString);
	}

	// ���� �� � ����� ����� ����������
	bool haveIngridInHands(short ingrid){
		return ingrid & ingridsInHands;
	}

	// ���� �� � �������� ����� ���������� ingrid
	bool isDesiredConsist(short ingrid){
		return desiredCollection & ingrid;
	}

	// ���� �� ����� �����
	bool isOvenNear(){

		for(int i = y - 1; i <= y + 1; i++)
			for(int j = x - 1; j <= x + 1; j++)
				if(kitchen[i][j] == OVEN)
					return true;
		return false;
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
	for(auto &cust : curCustomerVec){
		cust.readInput();
		//cerr << int(cust.ingridCollection) << endl;
	}
};

// ������� ���������� ��������� ������
void use(kitchenCellType cellType){

	pair<int, int> cellCoords = getCellWithAppliance(cellType);
	cout << "USE " << cellCoords.first << " " << cellCoords.second << endl;
	cerr << "USE " << kitchenCell2String(cellType) << endl;
};

// ������� ���������� ����������
void use(pair<int, int> p){
	if(isCoordsValid(p)){
		cout << "USE " << p.first << " " << p.second << endl;
		cerr << "USE PAIR " << p.first << " " << p.second << endl;
	}
};

// ���������� ��������� �� ���������
void makeChpdStrbr(){
	/*
	// ���� �� ����� ���� ���������� ���������, � � ����� ��� ������ ����� �������/������ ���
	if(isCoordsValid(getCellWithIngridCollection(CHPD_STRBR))
	 && me.ingridsInHands <= DISH){

		// ���� � ����� ���� �������, �� ���� �� ����������
		if(me.haveIngridInHands(DISH))				use(getCellWithIngridCollection(CHPD_STRBR));
		// ����� ����� �������
		else								use(DISHWASH);
	}else{
		// ���� ���������� ��������� ���...

		// ���� � ����� ���������, ���� �� ������
		if(me.ingridsInHands == STRBR)	use(CHOP_BOARD);
		else
			// ���� � ����� ��� �� ���� � ��� �� ���������, �� ������ ��� �� ��������� ������ �����
			if(me.ingridsInHands != NONE) use(getFreeTableCoord(me.x, me.y));
		// ���� � ����� ������ ���, �� ����� ���������
			else							use(STRBR_CREATE);
	}
	*/
};


// �������� + ���������� ���������
void makeCroissantWithChpdStrbr(){
	// ���� �������� ���������� �� �����
	if(existStrongly(CROISSANT) || me.haveIngridInHands(CROISSANT))
	{
		// ���� ���������� ��������� ���������� �� �����
		if(existStrongly(CHPD_STRBR) || me.haveIngridInHands(CHPD_STRBR))
		{
			// ���� � ���� � ����� ��� ���������� ���������
			if(!me.haveIngridInHands(CHPD_STRBR))
			{
				cerr << "i have NOT CHPD_STRBR with DISH in handle" << endl;

				if(me.ingridsInHands > DISH) use(getFreeTableCoord(me.x, me.y)); // ���� � ����� ��� �� ����� �������, �� ����������� ��� �� ����
				else
					if(me.ingridsInHands == NONE) use(DISHWASH); // ���� ��� ������� ����� �������
					else use(getCellWithIngridStrongly(CHPD_STRBR)); // ���� ������� ���� ����� ���������
			}
			// ���� ��������� ��� �� ������� � �����, �������� ������� ��������
			else
			{
				cerr << "i have CHPD_STRBR with DISH in handle" << endl;
				if(!me.haveIngridInHands(CROISSANT)) use(getCellWithIngridStrongly(CROISSANT)); // ���� � ���� � ����� ��� �������� ����� �������
			}
		}
		// ���� ���������� ��������� ��� �� �����, ������ ���������
		else
		{
			// ���� ���� ������, ����� ������� ���������
			if(me.ingridsInHands == NONE) use(STRBR_CREATE);
			// ���� � ����� ��� �� ����
			else
			{
				// ���� � ����� ������� ���������, ����� ��
				if(me.ingridsInHands == STRBR) use(CHOP_BOARD);
				// ���� � ����� �� ������� ���������, �� ������ ��� �� ����
				else use(getFreeTableCoord(me.x, me.y));
			}
		}
	}
	// ���� �������� ���, ������ �������
	else
	{
		// ���� ����� ������
		if(me.ovenReadyTurn != -1){

			// ���� �������� ����� - ��������
			if(turnsRemaining < me.ovenReadyTurn){ use(OVEN); me.ovenReadyTurn = -1; } else{ cout << "WAIT" << endl; cerr << "WAITING FOR CROISSANT" << endl; } // ���� ���� ������������� ����� - ���������� ���
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
						me.ovenReadyTurn = turnsRemaining - 11;
				}
			// ���� � ����� �� �����, �� ������ ��� �� ����
				else use(getFreeTableCoord(me.x, me.y));
		}
	}
};

// ������������ ������ �� �������
void makeDesiredCollection(){

	//cerr << "me.desiredCollection: " << int(me.desiredCollection) << endl;
	cerr << "	my current ingrids: " << int(me.ingridsInHands) << endl;

	bool canCook = true;
	// �����������: ������� - ��������� - ������� - ��������� - ��������
	// ��� ������ ������ ���������� ����������� �� �������, �������� ��� ����� ����������� � desiredCollection

	switch(me.desiredCollection){

		// ������ 1 = ��������� + ��������
		case(RECIPE1):{
			cerr << "i cook ICE + BLBR" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(BLBR))			use(WINDOW);
			//else if(me.haveIngridInHands(ICE_C))		use(BLBR_CREATE);
			//else if(me.haveIngridInHands(DISH))		use(ICE_CREATE);
			//else if(me.ingridsInHands == NONE)		use(DISHWASH);
			break;
		}

		// ������ 2 = ��������� + ���������
		case(RECIPE2):{
			cerr << "i cook CHPD_STRBR + ICE" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(ICE_C))				use(WINDOW);
			//else if(me.haveIngridInHands(CHPD_STRBR)
			//	 && me.haveIngridInHands(DISH))			use(ICE_CREATE);
			//else makeChpdStrbr();
			break;
		}

		// ������ 3 = ��������� + ��������
		case(RECIPE3):{
			cerr << "i cook CHPD_STRBR + BLBR" << endl;
			cout << "WAIT" << endl;

			//if(me.haveIngridInHands(BLBR))				use(WINDOW);
			//else if(me.haveIngridInHands(CHPD_STRBR)
			//	 && me.haveIngridInHands(DISH))			use(BLBR_CREATE);
			//else makeChpdStrbr();
			break;
		}

		// ������ 4 = ��������� + �������� + ���������
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

		// ������ 5 = �������� + ���������
		case(RECIPE5):{
			cerr << "i cook CROISSANT + ICE_C" << endl;
			cout << "WAIT" << endl;
			break;
		}

		// ������ 6 = �������� + �������� + ���������
		case(RECIPE6):{
			cerr << "i cook CROISSANT + BLBR + ICE_C" << endl;
			cout << "WAIT" << endl;
			break;
		}

		// ������ 7 = �������� + ��������� + ��������
		case(RECIPE7):{
			cerr << "i cook CROISSANT + CHPD_STRBR + BLBR" << endl;

			if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR + BLBR)			use(WINDOW);
			else if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR)				use(BLBR_CREATE);
			else makeCroissantWithChpdStrbr();
			break;
		}

		// ������ 8 = �������� + ��������� + ���������
		case(RECIPE8):{
			cerr << "i cook CROISSANT + CHPD_STRBR + ICE_C + " << endl;

			if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR + ICE_C)	use(WINDOW);
			else if(me.ingridsInHands == DISH + CROISSANT + CHPD_STRBR)		use(ICE_CREATE);
			else makeCroissantWithChpdStrbr();
			break;
		}

		// ������ 9 = �������� + ��������� + ��������� + ��������
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

// ��������
void makeSomething(){

	// 0. ���� ������ ��� ��������� �����, �� ����� ����� � ������� �������
	if( (me.desiredCollection == 0 && me.desiredReady)
		|| // ���
			  // 1. ���� ����� ������� ������� ���������, �� me.desiredCollection �������� ������ ������ ������� �������,
		      //    �� ������ me.desiredCollection �� ����� ������� �������
		(me.desiredCollection != curCustomerVec.at(1).ingridCollection && is1PartOf2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)) )
	{
		cerr << "CHANGE ORDER" << endl;
		me.desiredCollection = curCustomerVec.at(1).ingridCollection;
		me.desiredReady = false;
	}else{
		// 2. ���� ����� ������� ������� ���������, � me.desiredCollection �� �������� ������ ������ ������� �������
		//    �� ��������� me.desiredCollection � dishwasher
		if(me.desiredCollection != curCustomerVec.at(1).ingridCollection && !is1PartOf2(me.desiredCollection, curCustomerVec.at(1).ingridCollection)){
			cerr << "makeSomething case 1" << endl;
			cerr << "	my desiredCollection: " << int(me.desiredCollection) << ", order2 is " << int(curCustomerVec.at(1).ingridCollection) << endl;

			// ������ �����
			me.desiredCollection = curCustomerVec.at(1).ingridCollection;
			me.desiredReady = false;
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
