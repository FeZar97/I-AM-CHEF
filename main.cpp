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
	NONE       = 0x01,		// ������ ���� / ������ ����
	DISH       = 0x02,		// ��������
	BLBR       = 0x04,		// ��������
	ICE_C	   = 0x08,		// ���������
	STRBR	   = 0x10,		// ���������
	CHPD_STRBR = 0x20		// ����������� ���������
};

// �������� ������ �����
enum kitchenCellType {
	EMPTY		 = 0x00, // ������
	SOME_PRODUCT = 0x01, // �������, � �.�. ����������
	DISHWASH	 = 0X02, // ����� �������
	WINDOW		 = 0x04, // ����� � ������
	BLUE_CREATE	 = 0x08, // ����� ��������
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
				case  0: kitchen[i][j] = SOME_PRODUCT;  break; // �������, � �.�. ����������
				case 68: kitchen[i][j] = DISHWASH;		break; // ����� �������
				case 87: kitchen[i][j] = WINDOW;		break; // ����� � ������
				case 66: kitchen[i][j] = BLUE_CREATE;	break; // ����� ��������
				case 73: kitchen[i][j] = ICE_CREATE;	break; // ����� ���������
				case 83: kitchen[i][j] = STRAW_CREATE;	break; // ����� ���������
				case 67: kitchen[i][j] = CHOP_BOARD;	break; // ����� ��� �����
			}
		}
	}

};

// ���������� ������� ������
pair<int, int> getCellCoords(kitchenCellType desiredCell){
	for(int i = 0; i < K_HEIGHT; i++)
		for(int j = 0; j < K_WIDTH; j++)
			if(kitchen[i][j] == desiredCell) return pair<int, int>{j, i};
	return pair<int, int>{0, 0};
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

	cin >> curCustomerNb;
	curCustomerVec.resize(curCustomerNb);
	for(auto &cust: curCustomerVec) cust.readInput();
};

// ������������ ������ �� �������
void makeCollection(char desiredCollection){

	/*
	if (me.haveItem("ICE_CREAM")) getItem("WINDOW");
	else
		if (me.haveItem("BLUEBERRIES")) getItem("ICE_CREAM");
		else
			if (me.haveItem("DISH")) getItem("BLUEBERRIES");
			else
				if (me.haveItem("NONE")) getItem("DISH");
	*/
}

// ��������
void makeSomething(){
	// ������ ����� ������� �������
	char desiredCollection = curCustomerVec.at(2).ingridCollection;
	makeCollection(desiredCollection);
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