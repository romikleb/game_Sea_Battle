// ���� "������� ���"
// �����: ������� �����, ������� ������ ���-912
//
#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<clocale>
#include<string.h>
#include <Windows.h>
//
using namespace std;
//
void PrintPreamble(); // ������ ����������
char getChar();  // ���� ������� Char � ���������
int getInt();  // ���� ����� ���� int � ���������
int getInt_WithValueCheck(int Min, int Max); // ���� ����� ���� int � ���������: ������������ �������� ������ �� Min �� Max ������������.
//
bool getCellCoordinate(int*Line, int*Column, int MaxLineNumber, int MaxColumnNumber); // ���� ���������� ������ ���� � ��������� 
					  // ��� ���� "������� ���" �������. �������� '�4' ��� '�9'. ���������� false ���� ������������ ����� '0'.
//
void PrintField(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCells, bool **FleetAlive, bool ReportKill);  // �������� ������� ����. 
//
void SetColor(int text, int bg=0);    // ��������� ����� ����������� ������ � ����. Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, 
								   // Magenta = 5, Brown = 6, LightGray = 7, DarkGray = 8, LightBlue = 9, LightGreen = 10, LightCyan = 11, 
								   //  LightRed = 12, LightMagenta = 13, Yellow = 14, White = 15. ���������� ���� ������ - SetColor(7,0).
//
int FindTakenCells(int **TakenCellsField, int nLines, int nColumns, int **ShipsField, int TouchShip);  // ������� '�������' ������ - ������
				  // ������� ��������� � ShipsField[][] �, ���� ������� � TouchShip, ������ ����� � ���������. ���������� � ���������
				   // � ������� ������� ���������� � TakenCellsField[][]: 0-���������, 1-������ �������, 2-����� � �������. 
				  // ���������� ���������� ��������� �����. ���� ��� ��������� �����, �� ���������� 0.
//
int FindCellsForShot(int **CellsForShot, int nLines, int nColumns, int **VisibleShipsField, int TouchShip, bool **FleetAlive,
	int **Shot, bool ReportKill); //  ���������� � CellsForShot[][] ���������� � �������, ��������� ��� ��������.  
			 // 0-��������� ������,  1-������ �������, 2-������ ��-�� "�������" ������� �� ���������, 3-������, � �� ��� ��������. 
			  //  ���������� ���������� ��������� �����. ���� ��� ��������� �����, �� ���������� 0.
//
bool PlaceShips(int nLines, int nColumns, int **ShipsField, int **Shot, int *ArrShipSize, int ***ArrShape, int ***FleetPosition,
	int nShips, int ShipNumber, int TouchShip, bool Auto, bool **FleetAlive, bool ReportKill); // ����������� �������. ��������� ������� �� ����. ���������� false ���� ���������� �� �������. 
//
bool PlaceOneShipByHand(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill); // ���������� �������� �� ���� �������. ���������� false ���� ���������� �� �������.
//
bool PlaceOneShipAutomatically(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill); // �������������� ���������� �������� �� ����. ���������� false ���� ���������� �� �������.
//
bool PlaceShipInDirection(int nLines, int nColumns, int **ShipsField, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	int ***FleetPosition, int ShipNumber, int StartLine_i, int StartColumn_j, int Direction);  // ��������� ������� � ����������� Direction. 
	// � ������� ShipsField[][] � ������, ������� �������, ���������� ����� ������� ShipNumber. � ������� Fleet[ShipNumber-1]->position[][2] ���������� 
	// ������� ������ ������� [i_������][j_�������] �� ����. ���������� false ���� ���������� ������� �� ����������.
//
bool getAutoCellForShot(int*Line, int*Column, int nLines, int nColumns, int **CellsForShot, int LastShot_Line,
	int LastShot_Column);   // ������������� ���������� ���������� ������ ���� ����� ������ �������. ����� ������ � ������� (�� �������) 
	  // ���������� � Line � Column. ���������� true � ������ ������, false - ���� �� ������� ����� ��������� ������ ��� ��������.
//
inline void PrintSpaces(int n_spaces);  // �������� ������, ��������� �� ��������� ���������� ��������
//
void PauseWithStars(); // ������������� ����� - �������� ������ �� ���������� �������� * 
//
//
int main()
{
	setlocale(LC_ALL, "Russian");
	srand(time(0));
	char Exit{ 0 };  // ���������� ��� ������ �� ���������.
	const char *RussianSymbols[] = { "�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�",
		"�","�","�","�","�","�","�","�","�" }; // ������� ������� ��� ����������� ����� �������� ����.
	int nLines{ 0 }, nColumns{ 0 };  // ������ �������� ����.
	int nShips{ 0 };  //  ���������� ��������.
	int TouchShip{ 0 }; // ����� �� ������� �������� ���� �����: 0-���, 1-����� �������� ������, 2-����� �������� ��� �����������.
	bool ReportKill{ true }; // �������� ��������� "����" ��� ��������� �� ��� ������ �������.
	bool AutoPlace{ false }; // ��������� ���� �������������.
	bool PossiblePlaceShips{ true }; // ����������� ���������� ������� �� ����. ������������ �������� PlaceShipsAutomatically().
	const int StandartN_Ships{ 10 }; // ���������� �������� ��� ������ 1 - "������������".
	const int ArrStandartShipSize[StandartN_Ships]={ 4,3,3,2,2,2,1,1,1,1 }; // ������� �������� ��� ������ 1 - "������������".
	int GameMode{ 0 }; // ����� ���� (1-����������� ���������, 2-���������������� ���������). 
	int MaxFieldLines{ 28 }, MaxFieldColumns{ 38 };  // ������������ ������� �������� ���� ��� ������ � ������ 2 "� �����������".
	int nShipsMax{ 99 }; // ����������� ��������� ���������� �������� � ������ 2 "� �����������".
	int SizeAndShapeMode{ 0 }; // ����� ����� � ������� �������� - ��� ������ 2 "� �����������". 0 - ������������ ����������, ������
	                           //  � ����� ��������; 1 - '�����������' ���������� � ����� ��������; 2 - ���������� � ������������ ������
	                          // �������� �������� �������������, ������ � ����� ������� ������� - ���������.
	int Queue{ 0 };   // ��� ����������� ����, ��� ������ ������ �������.
	struct TheShip {  // ��������� '�������'. ���������� � ������� ��������� � ArrShipSize[], � ����� - � ArrShape[][][].
		bool Alive=true;  // ��������� ������� '���'. ���� ������� ���� (��� ������ ������� �������), �� Alive==false. 
		int **position;  // ������ �������� [ArrShipSize[i]][2] � ������������ (�� ���������) [������][�������] ������ ������� �� ����.
		int *hit;  // ������ � ����������� � ���������� ��������� � ���������� ������ �������. 0 - � ������ �� ���� ���������.
	};
	struct Player {
		int **ShipsField; // ������ ���� � ������� ��������� �������. � ������� ������ ������������ ����� (�� ������) �������.
		int **VisibleShipsField; // �������, '�������' ����������, �.�. ������, � ������� ������. � '�������' ������ ������������ ����� �������.
		int **ShotField;  // ���������� �� ���������� ��������. ��� ���������� ��������� - 0. ������ ����� ������� - ���������� 1.
		int **CellsForShot; // ���������� � �������, ��������� ��� ��������. 0-��������� ������, 1-������ �������,
		                      // 2-������ ��-�� "�������" ������� �� ���������. 
		TheShip **Fleet;  // ���� ������ - ������ ���������� �� ��������� TheShip.
		bool Alive= true; // ��������� ������ '���': ���� ��� ������� �������, �� Alive==false.
		int LastShot_Line{ 0 }, LastShot_Column{ 0 };  // �������� ������ ������ � ������� (�� �������) ������, 
		                                               // � ������� ��������� ��� ������� ���������.
	};
	//
	do {
		{  // ��������� ���� - ��� ����������� � ����� ������ ���� ��������� ��������   
			Exit= 0;
			TouchShip = 0;
			ReportKill = true;
			AutoPlace = false;
			PossiblePlaceShips = true;
			Queue = 0;
			system("cls");
			PrintPreamble();  // �������� ��������� � ����������: "�������� �����: 1 - �����������; 2 - � �����������. ��� ����� (1 ��� 2): "
			GameMode = getInt_WithValueCheck(1, 2);
			// ������� �������� ��� ������ ������� ����
			if (GameMode == 1) {   // ������� ������ 1 - "������������"
				nLines = 10;
				nColumns = 10;
				nShips = StandartN_Ships;
				cout << "\n�������, ����� �� ������� �������� ���� �����" << endl;
				cout << "  0 - �� ��������, " << endl;
				cout << "  1 - ����� �������� ������, " << endl;
				cout << "  2 - ����� �������� ��� �����������." << endl;
				cout << "��� �����: ";
				TouchShip = getInt_WithValueCheck(0, 2);
				cout << "\n������ ��������� \"����\" ��� ��������� �� ��� ������ �������?" << endl;
				cout << " 0 - �� ������," << endl;
				cout << " 1 - ������ \"����\"." << endl;
				cout << "��� �����: ";
				ReportKill = static_cast<bool>(getInt_WithValueCheck(0, 1));
			}
			else if (GameMode == 2) {   // ������� ������ 2 - "����� � ����������� ������"
				cout << "\n������� ������� �������� ����." << endl;
				cout << "  - ���������� ����� (�� 1 �� " << MaxFieldLines << "):";
				nLines = getInt_WithValueCheck(1, MaxFieldLines);
				cout << "  - ���������� �������� (�� 1 �� " << MaxFieldColumns << "):";
				nColumns = getInt_WithValueCheck(1, MaxFieldColumns);
				cout << "\n������ ��������� \"����\" ��� ��������� �� ��� ������ �������?" << endl;
				cout << " 0 - �� ������," << endl;
				cout << " 1 - ������ \"����\"." << endl;
				cout << "��� �����: ";
				ReportKill = static_cast<bool>(getInt_WithValueCheck(0, 1));
				cout << "\n�������, ����� �� ������� �������� ���� �����" << endl;
				cout << "  0 - �� ��������, " << endl;
				cout << "  1 - ����� �������� ������, " << endl;
				cout << "  2 - ����� �������� ��� �����������." << endl;
				cout << "��� �����: ";
				TouchShip = getInt_WithValueCheck(0, 2);
				cout << "������������ \'�����������\' ���������� � ����� ��������?" << endl;
				cout << "  0 - ��, ������������ \'�����������\' ���������� � ����� ��������," << endl;
				cout << "  1 - ���, ���������� ���������� � ����� �������.";
				SetColor(12);
				cout << "(���� ����������)" << endl;
				SetColor(7);
				cout << "��� �����: ";
				SizeAndShapeMode= getInt_WithValueCheck(0, 1);
				if (SizeAndShapeMode == 0) 	nShips = StandartN_Ships; 
				else if (SizeAndShapeMode == 1){
					cout << "\n������� ���������� �������� (�� 1 �� " << nShipsMax << "): ";
					nShips = getInt_WithValueCheck(1, nShipsMax);
				}
			}
			int *ArrShipSize = new int[nShips];  // ������ � ��������� (����������� ������) ������� �������. ����� ��� ����� �������.
			// ������������� ������� �������� (������� � ����� �������� ����� ���������� ��� ����� �������):
			if (GameMode == 1 || (GameMode == 2 && SizeAndShapeMode == 0)) 
				for (int i = 0; i < nShips; i++) ArrShipSize[i] = ArrStandartShipSize[i];  
			else if (GameMode == 2) {
				cout << "\n������� ������� �������� (�� 1 �� " << nLines * nColumns << "). ������� ����� " << endl;
				cout << "����������� ������� � ����� � ����� �������." << endl;
				for (int i = 0; i < nShips; i++) {
					cout << "������� ������ (���������� ������) ������� � " << i + 1 << ": ";
					ArrShipSize[i] = getInt_WithValueCheck(1, nLines* nColumns);
				}
			}
			// ������ ������� ��� ���������� ������ ���� �������� � �������������� ������. ����� �������� ����� ���������� � ����� �������.
			int ***ArrShape = new int**[nShips];
			for (int i = 0; i < nShips; i++) {
				ArrShape[i] = new int*[ArrShipSize[i]];
				for (int j = 0; j < ArrShipSize[i]; j++) {
					ArrShape[i][j] = new int[2];
					ArrShape[i][j][0] = 0;  // ������������� ���������� x ������ j ��� ������� i. 
					ArrShape[i][j][1] = 0;  // ������������� ���������� y ������ j ��� ������� i. 
				}
			}
			system("cls");
			// ������ ������ ������ User
			Player User;
			User.ShipsField = new int *[nLines];
			User.VisibleShipsField = new int *[nLines];
			User.ShotField = new int *[nLines];
			User.CellsForShot = new int *[nLines];
			for (int i = 0; i < nLines; i++) {
				User.ShipsField[i] = new int[nColumns];
				User.VisibleShipsField[i] = new int[nColumns];
				User.ShotField[i] = new int[nColumns];
				User.CellsForShot[i] = new int[nColumns];
				for (int j = 0; j < nColumns; j++) {  // �������������� ������. 
					User.ShipsField[i][j] = 0;
					User.VisibleShipsField[i][j] = 0;
					User.ShotField[i][j] = 0;
					User.CellsForShot[i][j] = 0;
				}
			}
			// ������ ������� ������ User � ���������� �� ���� ������
			User.Fleet = new TheShip*[nShips];
			int ***UserFleetPosition = new int**[nShips];  // ������ � ����������� �� ������� **position ������ User (��� �������� � �������).
			bool **UserFleetAlive = new bool*[nShips];  // ������ � ����������� �� ���������� Alive ������� ������� ������ User (��� �������� � �������).
			for (int i = 0; i < nShips; i++) {
				User.Fleet[i] = new TheShip;  
				User.Fleet[i]->position = new int*[ArrShipSize[i]];  // ������ ������ � ������������ [������][�������] (�� ���������)  ������ ������� �� ����.
				User.Fleet[i]->hit = new int[ArrShipSize[i]]; // ������ ��� ������ ��������� � ������ �������. 
				UserFleetPosition[i] = User.Fleet[i]->position;
				UserFleetAlive[i] = &User.Fleet[i]->Alive; 
				*UserFleetAlive[i] = true;   // �������������� ��� ���� �������� User ���������� Alive.
				for (int j = 0; j < ArrShipSize[i]; j++) {  
					User.Fleet[i]->position[j] = new int[2]; 
					User.Fleet[i]->position[j][0] = 0;  // ���������� '����� ������' (�� ������ �������)  ������ ������ ������� �� ����. 
					User.Fleet[i]->position[j][1] = 0;  // ���������� '����� �������' (�� ������ �������) ������ ������ ������� �� ����. 
					User.Fleet[i]->hit[j] = 0; // ��������� � ��� ������ ���.
				}
			}
			// ����������� ������� ������ User
			if (GameMode == 1 || (GameMode == 2 && SizeAndShapeMode == 0)) {   // ��� ������ 1 - "������������"
						//  ����������� ������� ������ User �� ����:
				cout << "\n  ���������� ������� �� ����:" << endl;
				// ������� '�����������' ����� ��������
				for (int i = 0; i < nShips; i++) {
					for (int j = 0; j < ArrStandartShipSize[i]; j++) {
						ArrShape[i][j][0] = j; // ������������� ����� ������  
						ArrShape[i][j][1] = 0; // ������������� ����� �������
					}
				}
				PrintField(nLines, nColumns, User.ShipsField, User.ShotField, User.CellsForShot, UserFleetAlive, ReportKill);
				cout << "\n\n    ���������� ������� �������������? (0-���; 1-��): ";
				AutoPlace = static_cast<bool>(getInt_WithValueCheck(0, 1));
				if (!PlaceShips(nLines, nColumns, User.ShipsField, User.ShotField, ArrShipSize, ArrShape, UserFleetPosition, nShips, 1,
					TouchShip, AutoPlace, UserFleetAlive, ReportKill)) {    // ��������� ������� ������ User �� ����.  
					cout << "\n�� ������� ���������� �� ���� ������� ������ User."<<endl;  
					cout << "\n��� ������ ������� \'9\', ��� ������ ����� ���� - ����� ������ ������: ";
					Exit = getChar();
					if (Exit == '9')  break;
					else continue;
				}
				else cout << "\n              ���  �������  �������  ���������" << endl;   
				Sleep(3000);
				PauseWithStars();  
			}
			else if (GameMode == 2) { 
				SetColor(12);
				cout << "������ ������� ���� �� �������� " << endl;
				SetColor(7);
				cout << "\n��� ������ ������� \'9\', ��� ������ ����� ���� - ����� ������ ������: ";
				Exit = getChar();
				if (Exit == '9')break;
				else continue;
			}
			//
			// ������ ������ ������ Computer
			Player Computer;
			Computer.ShipsField = new int *[nLines];
			Computer.VisibleShipsField = new int *[nLines];
			Computer.ShotField = new int *[nLines];
			Computer.CellsForShot = new int *[nLines];
			for (int i = 0; i < nLines; i++) {
				Computer.ShipsField[i] = new int[nColumns];
				Computer.VisibleShipsField[i] = new int[nColumns];
				Computer.ShotField[i] = new int[nColumns];
				Computer.CellsForShot[i] = new int[nColumns];
				for (int j = 0; j < nColumns; j++) {  // �������������� ������. 
					Computer.ShipsField[i][j] = 0;
					Computer.VisibleShipsField[i][j] = 0;
					Computer.ShotField[i][j] = 0;
					Computer.CellsForShot[i][j] = 0;
				}
			}
			// ������ ������� ������ Computer � ���������� �� ���� ������
			Computer.Fleet = new TheShip*[nShips];
			int ***ComputerFleetPosition = new int**[nShips];  // ������ � ����������� �� ������� **position ������ Computer ��� �������� � �������.
			bool **ComputerFleetAlive = new bool*[nShips];  // ������ � ����������� �� ���������� Alive ������� ������� ������ Computer (��� �������� � �������).
			for (int i = 0; i < nShips; i++) {
				Computer.Fleet[i] = new TheShip;
				Computer.Fleet[i]->position = new int*[ArrShipSize[i]];  // ������ ������ � ������������ [������][�������] (�� ���������)  ������ ������� �� ����.
				Computer.Fleet[i]->hit = new int[ArrShipSize[i]]; // ������ ��� ������ ��������� � ������ �������. 
				ComputerFleetPosition[i] = Computer.Fleet[i]->position;
				ComputerFleetAlive[i] = &Computer.Fleet[i]->Alive;
				*ComputerFleetAlive[i] = true;   // �������������� ��� ���� �������� Computer ���������� Alive.
				for (int j = 0; j < ArrShipSize[i]; j++) {
					Computer.Fleet[i]->position[j] = new int[2];
					Computer.Fleet[i]->position[j][0] = 0;  // ���������� '����� ������' (�� ������ �������)  ������ ������ ������� �� ����. 
					Computer.Fleet[i]->position[j][1] = 0;  // ���������� '����� �������' (�� ������ �������) ������ ������ ������� �� ����. 
					Computer.Fleet[i]->hit[j] = 0; // ��������� � ��� ������ ���.
				}
			}
			// ����������� ������� ������ Computer 
			if (!PlaceShips(nLines, nColumns, Computer.ShipsField, Computer.ShotField, ArrShipSize, ArrShape, ComputerFleetPosition, nShips, 1, TouchShip, true, ComputerFleetAlive, ReportKill)) {
				cout << "\n�� ������� ���������� �� ���� ������� ������ Computer." << endl;   // ��������� ������� �� ����.  
				cout << "\n��� ������ ������� \'9\', ��� ������ ����� ���� - ����� ������ ������: ";
				Exit = getChar();
				if (Exit == '9')  break;
				else continue;
			}
			system("cls");
			cout << "                        ���� � ������ ��������� " << endl;
			cout << "  ---------------------------------------------------------------------------" << endl;
			PrintField(nLines, nColumns, User.ShipsField, User.ShotField, User.CellsForShot, UserFleetAlive, ReportKill);
			cout << "\n                     ���� ����������  " << endl;
			cout << "  ---------------------------------------------------------------------------" << endl;
			PrintField(nLines, nColumns, Computer.VisibleShipsField, Computer.ShotField, Computer.CellsForShot, ComputerFleetAlive, ReportKill);
			//  ���������, ��� ������ ������ �������
			cout << "\n\n����������, ��� ������ ������ �������." << endl;
			cout << "\n��������� ������� �����: 0 ��� 1. �������� ���." << endl;
			cout << "��� �������: ";
			Queue = getInt_WithValueCheck(0, 1); 
			if (Queue == rand() % 2) {  // ���� User ������ �����, ��
				Queue = 0;  // ������� ������ User
				cout << "�����������! �� �������! ��� ������� ������." << endl;
			}
			else {
				Queue = 1;  // ������� ������ Computer
				cout << "�� �� �������.  ������ �������� ���������." << endl;
			}   
			Sleep(2000);
			PauseWithStars();
			//
			//----------  ������� ����  --------------------------------------------------------------------
			while (true) {
				// ���� ���� �� ������� ����, �� �����
				User.Alive = false;
				Computer.Alive = false;
				for (int i = 0; i < nShips; i++) {
					if (User.Fleet[i]->Alive == true) User.Alive = true;  
					if (Computer.Fleet[i]->Alive == true) Computer.Alive = true;
				}
				if (User.Alive == false || Computer.Alive == false) break; // ���� ���� �� ������� ����, �� ����� ����.
				User.Alive = true;
				Computer.Alive = true;
				system("cls");
				cout << "                        ���� � ������ ��������� " << endl;
				cout << "  ---------------------------------------------------------------------------" << endl;
				PrintField(nLines, nColumns, User.ShipsField, User.ShotField, User.CellsForShot, UserFleetAlive, ReportKill);
				cout << "\n                     ���� ����������  " << endl;
				cout << "  ---------------------------------------------------------------------------" << endl;
				PrintField(nLines, nColumns, Computer.VisibleShipsField, Computer.ShotField, Computer.CellsForShot, ComputerFleetAlive, ReportKill);
				cout << "\n";
				// ---------- ������� ������ User ������ ������� ----------
				if (Queue == 0)  {
					int i_Short{ 0 }, j_Short{ 0 }; // ������� ������ � ������� ������, ���� �������� �������.
					cout << "��� �������." << endl;
					cout << "������� ������ ��� �������� (�������� \'�8\'): ";
					//  ������ ���������� ������, ���� ���������� �������  
					getCellCoordinate(&i_Short, &j_Short, nLines, nColumns); // � i_Short � j_Short ����� ������ (�� ������) ������ � �������.
					i_Short--;  
					j_Short--;
					Computer.ShotField[i_Short][j_Short]++; // ������� ������� � ������ ShotField[][] ������ Computer.
					if (Computer.ShipsField[i_Short][j_Short] != 0) {   // ���� ������� ����� � ������ � �������
						Computer.VisibleShipsField[i_Short][j_Short] = Computer.ShipsField[i_Short][j_Short];  // �� ���������� ���� ����� �������.
						// ������� ��������� � ����������- ������ ������� 
						for (int i = 0; i < ArrShipSize[Computer.ShipsField[i_Short][j_Short] - 1]; i++) {
							if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->position[i][0] == (i_Short + 1) &&
								Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->position[i][1] == (j_Short + 1)) {
								Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->hit[i]++; // �������� ��������� � ���������� ������ �������.
								break;
							}
						}
						// ����������, '����' �� ������� (���� ���� ��������� �� ��� ������ �������)
						if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->hit[0] > 0) {  // ��������� ��������� � ������ ������ �������
							Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->Alive = false;
							for (int i = 1; i < ArrShipSize[Computer.ShipsField[i_Short][j_Short] - 1]; i++) {
								if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->hit[i] == 0) {
									Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->Alive = true;
									break; // ���� ���� ���� ������� ������� �� ����� ���������, �� ��� Alive = true.
								}
							}
						}
						cout << "    ����� !!!   " << endl;
						Sleep(1500);
						if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->Alive == false &&
							ReportKill == true) { // ���� ������� '����' � � ���������� ������� "�������� ����� '����'" 
							SetColor(12);
							cout << "           ���� !!!" << endl;
							SetColor(7);
						}
						// ��������� ������ Computer.CellsForShot[][]. 0-��������� ������, 1-������ �������. ���� � ���������� 
						// ������� "�������� ����� '����'", �� ����� � '������' ������� �������� ������� ������ ��������� 2,
						// �������� ��� ������� �������� TouchShip.
						if (FindCellsForShot(Computer.CellsForShot, nLines, nColumns, Computer.VisibleShipsField, TouchShip,
							ComputerFleetAlive, Computer.ShotField, ReportKill) == 0) {   // ��������� Computer.CellsForShot[][].
																	  // ���� ��� ��������� ��� �������� �����, �� �����.
							cout << "��� ��������� ����� ��� ��������" << endl;
							break;
						}
						Sleep(2000);
						PauseWithStars();
						continue;
					}
					else {  // ���� ������� ����� � ������ ��� �������
						cout << "  ���� " << endl;
						Sleep(2000);
						PauseWithStars();
						Queue = 1; //  ������� ���������� �������� ��������� ������ Computer.
						continue;
					}
				}
				// ----------  ������� ������ Computer ������ �������  ----------
				if (Queue == 1)  {
					int i_Short{ 0 }, j_Short{ 0 }; // ������� ������ � ������� ������, ���� �������� �������.
					cout << "������� ������ ���������." << endl;
					Sleep(1500);
					// ��������� ������ User.CellsForShot[][] ������� � �������, ��������� ��� ��������.
					if(FindCellsForShot(User.CellsForShot, nLines, nColumns, User.VisibleShipsField, TouchShip,
						UserFleetAlive, User.ShotField, ReportKill)==0) {   // ��������� User.CellsForShot[][]. ���� ��� ��������� ��� �������� �����, �� �����.
						cout << "��� ��������� ����� ��� ��������" << endl;
						break;
					}
					//  ������������� ���������� ���������� ������, ���� ���������� �������  
					if (!getAutoCellForShot(&i_Short, &j_Short, nLines, nColumns, User.CellsForShot, User.LastShot_Line,
						User.LastShot_Column)) {
						cout << "��� ��������� ����� ��� ��������" << endl;
						break;
					};
					User.LastShot_Line = i_Short;
					User.LastShot_Column = j_Short;
					i_Short--;
					j_Short--;
					cout << "������� � ������ \'" << RussianSymbols[i_Short] << j_Short + 1 <<"\'"<< endl;
					Sleep(2000);
					User.ShotField[i_Short][j_Short]++; // ������� ������� � ������ ShotField[][] ������ User.
					if (User.ShipsField[i_Short][j_Short] != 0) {   // ���� ������� ����� � ������ � �������
						User.VisibleShipsField[i_Short][j_Short] = User.ShipsField[i_Short][j_Short];  // �� ���������� ���� ����� �������.
						// ������� ��������� � ���������� ������ ������� 
						for (int i = 0; i < ArrShipSize[User.ShipsField[i_Short][j_Short] - 1]; i++) {
							if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->position[i][0] == (i_Short + 1) &&
								User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->position[i][1] == (j_Short + 1)) {
								User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->hit[i]++; // �������� ��������� � ���������� ������ �������.
								break;
							}
						}
						// ����������, '����' �� ������� (���� ���� ��������� �� ��� ������ �������)
						if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->hit[0] > 0) {  // ��������� ��������� � ������ ������ �������
							User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->Alive = false;
							for (int i = 1; i < ArrShipSize[User.ShipsField[i_Short][j_Short] - 1]; i++) {
								if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->hit[i] == 0) {
									User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->Alive = true;
									break; // ���� ���� ���� ������� ������� �� ����� ���������, �� ��� Alive = true.
								}
							}
						}
						cout << "    ����� !!!   " << endl;
						Sleep(1500);
						if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->Alive == false &&
							ReportKill == true) { // ���� ������� '����' � � ���������� ������� "�������� ����� '����'" 
							SetColor(12);
							cout << "           ���� !!!" << endl;
							SetColor(7);
						}
						// ���������� ��������� ��� �������� ������
						if (FindCellsForShot(User.CellsForShot, nLines, nColumns, User.VisibleShipsField, TouchShip, UserFleetAlive,
							User.ShotField, ReportKill) == 0) {   // ��������� User.CellsForShot[][].
							                               // ���� ��� ��������� ��� �������� �����, �� �����.
							cout << "��� ��������� ��� �������� �����" << endl;
							continue;
						}
						Sleep(2000);
						PauseWithStars();
						continue;
					}
					else {  // ���� ������� ����� � ������ ��� �������
						cout << "  ���� " << endl;
						Sleep(2000);
						PauseWithStars();
						Queue = 0; //  ������� ���������� �������� ��������� ������ User.
						continue;
					}
				}
			}
			// ������� ������������ ������:
			for (int i = 0; i < nShips; i++) {
				for (int j = 0; j < ArrShipSize[i]; j++) delete[]Computer.Fleet[i]->position[j];
				delete[]Computer.Fleet[i]->position;
				delete[]Computer.Fleet[i]->hit;
				delete[]Computer.Fleet[i];
			}
			delete[]ComputerFleetAlive;
			delete[]ComputerFleetPosition;
			delete[]Computer.Fleet;
			for (int i = 0; i < nLines; i++) {
				delete[]Computer.ShipsField[i];
				delete[]Computer.VisibleShipsField[i];
				delete[]Computer.ShotField[i];
				delete[]Computer.CellsForShot[i];
			}
			delete[]Computer.CellsForShot;
			delete[]Computer.ShotField;
			delete[]Computer.VisibleShipsField;
			delete[]Computer.ShipsField;
			for (int i = 0; i < nShips; i++) {
				for (int j = 0; j < ArrShipSize[i]; j++) delete[]User.Fleet[i]->position[j];
				delete[]User.Fleet[i]->position;
				delete[]User.Fleet[i]->hit;
				delete[]User.Fleet[i];
			}
			delete[] UserFleetAlive;
			delete[]UserFleetPosition;
			delete[]User.Fleet;
			for (int i = 0; i < nLines; i++) {
				delete[]User.ShipsField[i];
				delete[]User.VisibleShipsField[i];
				delete[]User.ShotField[i];
				delete[]User.CellsForShot[i];
			}
			delete[]User.CellsForShot;
			delete[]User.ShotField;
			delete[]User.VisibleShipsField;
			delete[]User.ShipsField;
			for (int i = 0; i < nShips; i++) {
				for (int j = 0; j < ArrShipSize[i]; j++) {
					delete[]ArrShape[i][j];
				}
				delete[] ArrShape[i];
			}
			delete[] ArrShape;
			//
			Sleep(5000);
			system("cls");
			if (User.Alive != Computer.Alive) {
				cout << "      ********************************************************" << endl;
				cout << endl;
				cout << "                          ���������  ����: " << endl;
				cout << endl;
				if (User.Alive == true) {
					cout << "                        ��  �������� !!!!!  " << endl;
					cout << endl;
					cout << "                          ����������� !!!" << endl;
				}
				if (Computer.Alive == true) cout << "               ������� ���������.  �� ���������  :( " << endl;
				cout << endl;
				cout << "      ********************************************************" << endl;
			}
			cout << "\n\n\n��� ������ ������� \'9\', ��� ������ ����� ���� - ����� ������ ������: ";
			Exit = getChar();
		}
	} while (Exit != '9');
	return 0;  // ����� ������� main()
}
//
//
void PrintPreamble() // ������ ����������
{
	cout << "\n"<<endl;
	cout << "  ------------------------------------------------------------------" << endl;
	cout << "  -----                                                        ----- " << endl;
	cout << "  -----  ����  \"�������  ���  ������  ����������\"  ������ 1.0  ----- " << endl;
	cout << "  -----                                                        ----- " << endl;
	cout << "  ------------------------------------------------------------------" << endl;
	cout << endl;
	cout << "���� ����� ��� ������:" << endl;
	cout << "  - ����� 1 - \'�����������\'" << endl;
	cout << "  - ����� 2 - \'� �����������\'." << endl;
	cout << endl;
	cout << "����� 1 - \'�����������\': " << endl;
	cout << " - ������� ���� 10x10, 10 �������� (���� 4-��������; ��� 3-��������;" << endl;
	cout << "     ��� 2-��������; ������ 1-��������); " << endl;
	cout << " - ������� ����� ����� ������ ����� ��� �������;" << endl;
	cout << " - ����� �����, ����� �� ������� �������� ���� �����;" << endl;
	cout << " - ����� ���������, �������� ��� ��� ��������� \"����\" ����� ��������� " << endl;
	cout << "     �� ��� ������ �������;" << endl;
	cout << " - �������� �������������� ����������� �������� ������." << endl;
	cout << endl;
	cout << "����� 2 - \'� �����������\':" << endl;
	cout << " - �������� ������� ����: ����� N_Str= 1...28,  �������� N_Stolb= 1...38;" << endl;
	cout << " - ������� ���������� ��������: �� 1 �� 99;" ;
	SetColor(12); 
	cout << "(���� ����������)" << endl;
	SetColor(7);
	cout << " - �������� ������� ������� �������: �� 1 �� N_Str*N_Stolb;";
	SetColor(12);
	cout << "(���� ����������)" << endl;
	SetColor(7);
	cout << " - ������������ ����� ������� ������� (������� �������). � ������" << endl;
	cout << "   � � ���������� ���������� ���������� � ����� ��������;";
	SetColor(12);
	cout << "(���� ����������)" << endl;
	SetColor(7);
	cout << " - ���� ������� �� \'�����������\' ����������� � ������ ��������;" << endl;
	cout << " - ������� ����� �������� ���� ����� (������� �������);" << endl;
	cout << " - �������� ��� ��� ��������� \"����\" ������ �����;" << endl;
	cout << endl;
	cout << endl;
	cout << "�������� ����� ����:" << endl;
	cout << "  1 - ����������� �����;" << endl;
	cout << "  2 - ����� � �����������." << endl;
	cout << "��� ����� (1 ��� 2): ";
	return;
}
//
char getChar()  // ���� ������� Char � ���������
{
	while (true) // ���� ������������ �� ��� ���, ���� ������������ �� ����� ���������� ��������
	{
		char a;
		cin >> a;
		// �������� �� ���������� ����������
		if (cin.fail()){ // ���� ���������� ���������� ��������� ���������,
			cin.clear(); // �� ���������� cin � '�������' ����� ������
			cin.ignore(32767, '\n'); // � ������� �������� ����������� ����� �� �������� ������
			cout << "������!!! ���������� ��� ���\n";
		}
		else
		{
			cin.ignore(32767, '\n'); // ������� ������ ��������
			return a;
		}
	}
}
//
int getInt()  // ���� ����� ���� int � ���������
{
	while (true) // ���� ������������ �� ��� ���, ���� ������������ �� ����� ���������� ��������
	{
		int a;
		cin >> a;
		// �������� �� ���������� ����������
		if (cin.fail()) // ���� ���������� ���������� ��������� ���������,
		{
			cin.clear(); // �� ���������� cin � '�������' ����� ������
			cin.ignore(32767, '\n'); // � ������� �������� ����������� ����� �� �������� ������
			cout << "������!!! ������ ���� ����� �����. ���������� ��� ���.\n";
		}
		else
		{
			cin.ignore(32767, '\n'); // ������� ������ ��������
			return a;
		}
	}
}
//
int getInt_WithValueCheck(int Min, int Max) // ���� ����� ���� int � ���������: ������������ �������� ������ �� Min �� Max ������������.
{
	int tmp;
	if (Min > Max) {
		tmp = Min;
		Min = Max;
		Max = tmp;
	}
	while (true) {
		tmp = getInt();
		if (tmp >= Min && tmp <= Max) break;
		cout << "������!!! ����� ������ ���� �� " << Min << " �� " << Max << ". ���������� ��� ���" << endl;
	}
	return tmp;
}
//
bool getCellCoordinate(int*Line, int*Column, int MaxLineNumber, int MaxColumnNumber) // ���� ���������� ������ ���� � ��������� 
					  // ��� ���� "������� ���" �������. �������� '�4' ��� '�9'. ���������� false ���� ������������ ����� '0'.
{
	// ������������ ������� { �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, �, � };  // 28 ��������  
	int RusSymbFromCin[] = { -96,-95,-94,-93,-92,-91,-90,-89,-88,-86,-85,-84,-83,-82,-81,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-19,-18,-17 };  // 28 ��������
	char LineChar{ 0 }; // ������ ����������� ������ ������
	int LineInt{ 0 }, ColumnInt{ 0 };  // ����� ������ � ����� ������� (�� ������ � ��������).
	while (true) {
		LineChar = 0;
		LineInt = 0;
		ColumnInt = 0;
		while (true) {
			LineChar = 0; // ���������� ����������� ������ ������
			ColumnInt = 0;  // ����� ������ � ����� ������� (�� ������ � ��������).
			cin >> LineChar;
			if (LineChar == 48) return false; // ���� ����� ������ '0'.
			cin >> ColumnInt;
			// �������� �� ���������� ����������
			if (cin.fail()) // ���� ���������� ���������� ��������� ���������
			{
				cin.clear(); // �� ���������� cin � '�������' ����� ������
				cin.ignore(32765, '\n'); // � ������� �������� ����������� ����� �� �������� ������
				cout << "������!!! ���������� ��� ���." << endl;
			}
			else {
				cin.ignore(32767, '\n');  // ������� ������ �������� � ������
				break;
			}
		}
		// ���������� ����� ������ �� �������� �����   
		for (int i = 0; i < 28; i++) {
			if (RusSymbFromCin[i] == LineChar) {
				LineInt = i + 1;  // ���������� ����� ������ (�� ������ � �������� ������) �� �������� �����.
				break;
			}
		}
		if (LineInt<1 || LineInt>MaxLineNumber || ColumnInt<1 || ColumnInt>MaxColumnNumber) {
			cout << "������!!! �������� ��������!!! ���������� ��� ���."<< endl;
		}
		else break;
	}
	*Line = LineInt; // ����������� ����� (�� ������) ������ (�� 1 ������ ������� �������).
	*Column = ColumnInt;  // ����������� ����� (�� ������) ������� (�� 1 ������ ������� �������). 
	return true;
}
//
void PrintField(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCells, bool **FleetAlive, bool ReportKill)  // �������� ������� ����. 
{
	if (nLines > 28) nLines = 28;
	if (nLines < 1) nLines = 1;
	if (nColumns > 38) nColumns = 38;
	if (nColumns < 1) nColumns = 1;
	int LeftSideSize = (80 - nColumns * 2 - 3) / 2;
	setlocale(LC_ALL, "C");
	const char *RussianSymbols[] = { "�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�","�"};
	// �������� ������� ������ � �������� ��������
	PrintSpaces(LeftSideSize+2);
	for (int i = 0; i < nColumns; i++) {
		if(i>10 && (i%2!=0)) SetColor(15);
		cout << i+1;
		if (i < 9) cout << " ";
		SetColor(7);
	}
	cout << endl;
	// �������� ������� ����� �������������� 
	PrintSpaces(LeftSideSize + 1);
	cout << static_cast<char>(218);
	for (int i = 0; i < nColumns-1; i++)  cout << static_cast<char>(196) << static_cast<char>(194);
	cout << static_cast<char>(196) << static_cast<char>(191) << endl;
	//
	// �������� ���������� �������������� ����� ������� � ������ �������
	for (int i = 0; i < nLines; i++) {
		// ������ ������ ������ ����� - ������ �� ������� �������� Shot
		setlocale(LC_ALL, "Russian");
		PrintSpaces(LeftSideSize);
		cout << RussianSymbols[i];  //  ������ ����� � ������� ������ (����� �� �������� ����)
		setlocale(LC_ALL, "C");
		cout << static_cast<char>(179);
		for (int j = 0; j < nColumns; j++) {
			// ������ ����������� ������ ���� (������ ��������, ������ ��� ������)
			if (Shot[i][j] == 0) {
				if (TakenCells[i][j] == 0) cout << " ";  //  ���� � ���� ������ ��� �� ��������, �� ������� TakenCells, �� �������� ������.
				if (TakenCells[i][j] != 0) {   // ���� ��� ��������, �� ���� ������� TakenCells, �� ���������:
					if (ShipsField[i][j] != 0) cout << " "; // ���� �������, �� �������� ������.
					else cout << static_cast<char>(251); // ���� ������� ���, �� �������� "����". 
				}
			}
			else if (ShipsField[i][j] == 0) { // ���� ������� ���� ������� - �������� ������� �����. 
				SetColor(12);
				cout << static_cast<char>(249);
				SetColor(7);
			}
			else {
				// ���� ������� � ������� � ������� '����' � ���� �������� �� ����, �� �������� ������� 'X' (����� �� ��������� �������� X ����� ������)
				if (*FleetAlive[ShipsField[i][j] - 1] == false && ReportKill==true) SetColor(12);
				cout << 'X';
				SetColor(7);
			}
			// ������ ������� ������� ������ (��� ������� �������, ��� ������� �������������� ����)
			if (j == nColumns - 1) {
				cout << static_cast<char>(179);  // ���� ����� ������ - �������� ������������ �����
				break;
			}
			else  (ShipsField[i][j] != 0 || ShipsField[i][j + 1] != 0) ? cout << static_cast<char>(179) : cout << " "; // ������ ������������ ������� �������.
		}
		setlocale(LC_ALL, "Russian");
		cout << RussianSymbols[i] << endl;  // ������ ����� (����� ������) ������ �� ������ ������� ��������������
		setlocale(LC_ALL, "C");
		if (i == nLines - 1) break; // ��� ������ ��������� ������ ������ ����� �� ��������
		//
		// ������ ������ ����� ����� - ����� ��� ������� (�.�. ����� ���������� ����� ��������)
		PrintSpaces(LeftSideSize+1);
		cout << static_cast<char>(195);  
		for (int j = 0; j < nColumns; j++) {
			// ������ ������� ������� - �������������� ������� ����� �������� (��� ������� ������� ������ ��� �����)
			(ShipsField[i][j] == 0 && ShipsField[i + 1][j] == 0) ? cout << " ": cout << static_cast<char>(196);
			//
			// ������ ������� ������� - ���� ������ (��� ������ ������� �������������� ����)
			if (j == nColumns - 1) {
				cout << static_cast<char>(180);  // ���� ����� ������ - �������� ������ ������� �������� ����
				break;
			}
				// ������� ������ ������� 197 ("�����") 
			if ((ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0) ||
				(ShipsField[i][j] != 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] != 0) ||
				(ShipsField[i][j] != 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] != 0) ||
				(ShipsField[i][j] == 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] == 0) ||
				(ShipsField[i][j] != 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] == 0) ||
				(ShipsField[i][j] != 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] != 0) ||
				(ShipsField[i][j] == 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] != 0) ||
				(ShipsField[i][j] != 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] != 0)) {
				cout << static_cast<char>(197);
				continue;
			}
		        // ������� ������ ������� 218 ("������� ����� ����") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] != 0) {
				cout << static_cast<char>(218);
				continue;
			}
			    // ������� ������ ������� 191 ("������� ������ ����") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(191);
				continue;
			}
			    // ������� ������ ������� 217 ("������ ������ ����") 
			if (ShipsField[i][j] != 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(217);
				continue;
			}
				// ������� ������ ������� 192 ("������ ����� ����") 
			if ( ShipsField[i][j] == 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0)	{
				cout << static_cast<char>(192);
				continue;
			}
				// ������� ������ ������� 193 ("�������� ������ ������� �������") 
			if (ShipsField[i][j] != 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(193);
				continue;
			}
				// ������� ������ ������� 194 ("�������� ������� ������� �������") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] != 0) {
				cout << static_cast<char>(194);
				continue;
			}
			// ������� ������ ������� 195 ("�������� ����� ������� �������") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] != 0) {
				cout << static_cast<char>(195);
				continue;
			}
			// ������� ������ ������� 180 ("�������� ������ ������� �������") 
			if (ShipsField[i][j] != 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(180);
				continue;
			}
		}
		cout << endl;
	}
	//
	// �������� ������ ����� �������������� 
	PrintSpaces(LeftSideSize + 1);
	cout << static_cast<char>(192);
	for (int i = 0; i < nColumns - 1; i++) {
		cout << static_cast<char>(196) << static_cast<char>(193); 
	}
	cout << static_cast<char>(196) << static_cast<char>(217) << endl;
	//
	// �������� ������ ������ � �������� ��������
	PrintSpaces(LeftSideSize + 2);
	for (int i = 1; i <= nColumns; i++) {
		if (i > 10 && (i % 2 != 0)) SetColor(15);
		cout << i;
		if (i < 10) cout << " ";
		SetColor(7);
	}
	cout << endl;
	setlocale(LC_ALL, "Russian");
}
//
void SetColor(int text, int bg)    // ��������� ����� ����������� ������ � ����. Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, 
								   // Magenta = 5, Brown = 6, LightGray = 7, DarkGray = 8, LightBlue = 9, LightGreen = 10, LightCyan = 11, 
								   //  LightRed = 12, LightMagenta = 13, Yellow = 14, White = 15. ���������� ���� ������ - SetColor(7).
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((bg << 4) | text));
}
//
int FindTakenCells(int **TakenCellsField, int nLines, int nColumns, int **ShipsField, int TouchShip)  // ������� '�������' ������ - 
               // - ������ ������� ��������� � ShipsField[][] �, ���� ������� � TouchShip, ������ ����� � ���������. 
	           // ���������� � ������� ������� ���������� � TakenCellsField[][]: 0-���������, 1-������ �������, 2-����� � �������. 
	           // ���������� ���������� ��������� �����. ���� ��� ��������� �����, �� ���������� 0.
{
	int nFree{ 0 }; // ���������� ��������� ����� �� ����. 
	int CoordNeighborCells[8][2] = { {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0} }; // ������������� ����������
														 // {������, �������} ������, �������� � ������� ������� ����. 
	for (int i = 0; i < nLines; i++) {   // ���� �� �������.
		for (int k = 0; k < nColumns; k++) TakenCellsField[i][k] = 0; // �������������� ������
		for (int j = 0; j < nColumns; j++) {  // ���� �� ��������.
			// ������� ��� ������� ������ ��������� '�������' (�.�. � ��� ������ ������ ������� �������):
			if (ShipsField[i][j] != 0) {  // ���� � ������ ��������� ������� - �� ������ ������.
				TakenCellsField[i][j] = 1;  // �������� ������ 1 ������, ������� �������
				continue;  // ��������� � �������� ��������� ������.
			}
			if (TouchShip == 0) {  // ������� ������, ����� � ������� (� ����� �������) ���� ������� (���� �������� ������ �������� ���� �����):
				int tmp_i{ 0 }, tmp_j{ 0 };
				for (int NeighborCell = 0; NeighborCell < 8; NeighborCell++) {
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // ������ ������ �������� ������.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // ������ ������� �������� ������.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // ������ �� ��������� ���� �� ���������.
					if (ShipsField[tmp_i][tmp_j] != 0) {  // ���� � �������� ������ ��������� �������
						TakenCellsField[i][j] = 2;  // �������� ������ 2 ������, ����� � ������� (� ����� ������� �� ������) ��������� �������.
						break;
					}
				}
			}
			else if (TouchShip == 1) { // ������� ������ �� ��������� ����� � ��������� (����� ������� ����� �������� ���� ����� ������):
				int tmp_i{ 0 }, tmp_j{ 0 };
				// �������� �� ������� �������� ������-�����-������-�����
				for (int NeighborCell = 1; NeighborCell < 8; NeighborCell += 2) {
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // ����� ������ �������� ������.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // ����� ������� �������� ������.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // ������ �� ��������� ���� �� ���������.
					if (ShipsField[tmp_i][tmp_j] != 0) {  // ���� � �������� ������ ������-�����-������-����� ��������� �������
						TakenCellsField[i][j] = 2;  // �������� ������ 2 ������, ����� � ������� (������-�����-������-�����) ���� �������.
						break;
					}
				}
			}
		}
	}
	// ���������� ���������� '���������' ����� nFree
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (TakenCellsField[i][j] == 0) nFree++;
		}
	}
	return nFree;
}
//
int FindCellsForShot(int **CellsForShot, int nLines, int nColumns, int **VisibleShipsField, int TouchShip, bool **FleetAlive, 
	int **Shot, bool ReportKill)  //  ���������� � CellsForShot[][] ���������� � �������, ��������� ��� ��������.  
		     // 0-��������� ������,  1-������ �������, 2-������ ��-�� "�������" ������� �� ���������, 3-������ (� �� ��� ��������). 
			  //  ���������� ���������� ��������� �����. ���� ��� ��������� �����, �� ���������� 0.
{
	int nFree{ 0 }; // ���������� ��������� ����� �� ����. 
	int CoordNeighborCells[8][2] = { {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0} }; // ������������� ����������
														 // {������, �������} ������, �������� � ������� ������� ����. 
	for (int i = 0; i < nLines; i++) {   // ���� �� �������.
		for (int k = 0; k < nColumns; k++) CellsForShot[i][k] = 0; // ��������� ������.
		for (int j = 0; j < nColumns; j++) {  // ���� �� ��������.
			// ������� ��� ������� ������ ��������� '�������' (�.�. � ��� ������ ������ ��������):
			if (VisibleShipsField[i][j] != 0) {  // ���� � ������ ��������� ������� - �� ������ ������.
				CellsForShot[i][j] = 1;  // �������� ������ 1 ������, ������� �������.
				continue;  // ��������� � �������� ��������� ������.
			}
			// ��� ������ �� ������� ������� - ���������, �������� �� � ������ ����� 
			if (Shot[i][j] != 0) {  
				CellsForShot[i][j] = 3;  // �������� ������ 3 ������,  � ������� ��� ��������.
				continue;  // ��������� � �������� ��������� ������.
			};
			// ���������, ���� �� � �������� ������ '������' �������(� ������ ������� TouchShip)
			if (TouchShip == 0) {  // �������� ������ �������� ���� �����. ������� ������, � ����� ������� ������� ���� �������
				int tmp_i{ 0 }, tmp_j{ 0 };
				for (int NeighborCell = 0; NeighborCell < 8; NeighborCell++) {   // ������� �������� ������ ���� (�����)
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // ������ ������ �������� ������.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // ������ ������� �������� ������.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // ���� ������ �� ��������� ����, �� �� ���������.
					if (VisibleShipsField[tmp_i][tmp_j] != 0 && *FleetAlive[VisibleShipsField[tmp_i][tmp_j] - 1] == false &&
						ReportKill == true) { //  ���� � �������� ������ ��������� '������' ������� � ���� �������� �� ����
						CellsForShot[i][j] = 2;  // �������� ������ 2 ������, ����� � ������� (� ����� ������� �� ������) ���� '������' �������.
						break;
					}
				}
			}
			else if (TouchShip == 1) { // ������� ����� �������� ���� ����� ������. ������� ������, � ������� ���� �� � ����� �� 
					                       // 4-� ������ (������, �����, ������ ��� �����) ���� �������
				int tmp_i{ 0 }, tmp_j{ 0 };
				// �������� �� ������� �������� ������-�����-������-�����
				for (int NeighborCell = 1; NeighborCell < 8; NeighborCell += 2) {
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // ����� ������ �������� ������.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // ����� ������� �������� ������.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // ������ �� ��������� ���� �� ���������.
					if (VisibleShipsField[tmp_i][tmp_j] != 0 && *FleetAlive[VisibleShipsField[tmp_i][tmp_j] - 1] == false &&
						ReportKill == true) {  // ���� � �������� ������ (������-�����-������-�����) ��������� '������' �������
						                        // � ���� �������� �� ����
						CellsForShot[i][j] = 2;  // �������� ������ 2 ������, ����� � ������� (������-�����-������-�����) ���� '������' �������.
						break;
					}
				}
			}
		}
	}
	// ���������� ���������� '���������' ��� �������� �����
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (CellsForShot[i][j] == 0) nFree++;
		}
	}
	return nFree;
}
//
bool PlaceShips(int nLines, int nColumns, int **ShipsField, int **Shot, int *ArrShipSize, int ***ArrShape, int ***FleetPosition, 
	int nShips, int ShipNumber, int TouchShip, bool Auto, bool **FleetAlive, bool ReportKill) // ����������� �������. ��������� ������� �� ����. ���������� false ���� ���������� �� �������. 
{
	bool ReturnValue{ false }; // ������������ �������� ��������.   
	// ������ ������ TakenCellsField[][] � ������� � ������� ������� (0-��������� ������, 1-������ �������, 2-������ ��-�� ������� �� ���������):
	int **TakenCellsField = new int*[nLines]; 
	for (int i = 0; i < nLines; i++) {   // ���� �� �������.
		TakenCellsField[i] = new int[nColumns];
		for (int j = 0; j < nColumns; j++) TakenCellsField[i][j] = 0; // �������������� ������
	}
	if (FindTakenCells(TakenCellsField, nLines, nColumns, ShipsField, TouchShip) == 0) { // ���������� � TakenCellsField[][] ������
		      // � ������� ������� � ��������� ������� ��������� �����. ���� ��� ��������� ����� - ���������� false.
		cout << "\n��� ��������� �����." << endl;
		return false;
	}
	// ��������� ����� �������
	if (Auto) {   // ��������� ������� �������������
		if (!PlaceOneShipAutomatically(nLines, nColumns, ShipsField, Shot, TakenCellsField, ArrShipSize, ArrShape, FleetPosition,
			ShipNumber, FleetAlive, ReportKill)) ReturnValue = false;  // ���� �� ������� ������������� ���������� �������.
		else {
			if (ShipNumber >= nShips) ReturnValue = true; // ����� ��������� ���������� ���������� ������� (������� ������ �� ��������).
			else if (PlaceShips(nLines, nColumns, ShipsField, Shot, ArrShipSize, ArrShape, FleetPosition, nShips,
				ShipNumber + 1, TouchShip, Auto, FleetAlive, ReportKill)) ReturnValue = true; // ��������� ��������� ������� - ����������� ����� ����-�� �������. 
			else ReturnValue = false;    // ���� �� ������� ���������� ��������� �������
		} 
	}
	else {    // ��������� ������� �������. 
		if (!PlaceOneShipByHand(nLines, nColumns, ShipsField, Shot, TakenCellsField, ArrShipSize, ArrShape, FleetPosition,
			ShipNumber, FleetAlive, ReportKill)) ReturnValue= false;  // ���� �� ������� ���������� ������� �������.
		else {
			if (ShipNumber >= nShips) ReturnValue = true; // ����� ��������� ���������� ���������� ������� (������� ������ �� ��������).
			else if (PlaceShips(nLines, nColumns, ShipsField, Shot, ArrShipSize, ArrShape, FleetPosition, nShips, ShipNumber + 1,
				 TouchShip, Auto, FleetAlive, ReportKill)) ReturnValue = true; // ��������� ��������� ������� - ����������� ����� ����-�� �������.. 
			else ReturnValue = false;    // ���� �� ������� ���������� ��������� �������
		}
	}
	// ������� ������������ ������
	for (int i = 0; i < nLines; i++) delete[]TakenCellsField[i];
	delete[]TakenCellsField;
	//  
	return ReturnValue;  // ����� �� ������� PlaceShips()
}
//
bool PlaceOneShipByHand(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape, 
	int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill) // ���������� �������� �� ���� �������. ���������� false ���� ���������� �� �������.
{
	int StartLine{ 0 }, StartColumn{ 0 };  // ����� (�� ������) ������ � ������� ������ ('���������') ������ �������.
	int Direction{ 0 }; // ����������� ������� �� ������ ('���������') ������. (1-�����; 2-������; 3-����; 4-�����).
	system("cls");
	cout << "\n                     ------------------------------------ " << endl;
	cout << "                         ����������� �������� �� ����:" << endl;
	cout << "                     ------------------------------------ " << endl;
	cout << "\n\n\n" << endl;
	PrintField(nLines, nColumns, ShipsField, Shot, TakenCellsField, FleetAlive, ReportKill);
	cout << "\n\n";
	cout << "���������� ������� �" << ShipNumber << ".   ������: " << ArrShipSize[ShipNumber - 1] << " ������." << endl;
	while (true) {
		while (true) {
			cout << "������� ��������� ������ ������� (��������: \'�9\') ��� 0 ��� ������: ";
			if (!getCellCoordinate(&StartLine, &StartColumn, nLines, nColumns)) return false; // � StartLine � StartColumn �������� ����� ������ - ����� (�� ������) ������ � �������. 
			if (TakenCellsField[StartLine - 1][StartColumn - 1] == 0) break;
			else if (TakenCellsField[StartLine - 1][StartColumn - 1] == 1) cout << "������!!!  ������ ������ �������." << endl;
			else if (TakenCellsField[StartLine - 1][StartColumn - 1] == 2) cout << "������!!! ������ ��������� ����� � ������ �������." << endl;
		}
		if (ArrShipSize[ShipNumber - 1] == 1) Direction = 1;  // ��� ������� �������� 1 
		else {
			cout << "������� ����������� ������� (1-�����; 2-������; 3-����; 4-�����): ";
			Direction = getInt_WithValueCheck(1, 4);
		}
		if (!PlaceShipInDirection(nLines, nColumns, ShipsField, TakenCellsField, ArrShipSize, ArrShape, FleetPosition, ShipNumber,
			StartLine - 1, StartColumn - 1, Direction)) cout << "�� ������� ���������� �������. ������� ������ ����������." << endl;
		else break;
	}
	return true;  // ����� �� ������� PlaceOneShipByHand()
}
//
bool PlaceOneShipAutomatically(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	 int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill) // �������������� ���������� �������� �� ����. ���������� false ���� ���������� �� �������.
{
	int nFree{ 0 }; // ���������� ��������� ����� �� ����. 
	int RandomCellNumber{ 0 }; // ����� ��������� ������ (�� 1 �� nFree ������������).
	int i_start{ 0 }, j_start{ 0 }; // ������� ������ � ������� ������ ������.
	bool ReturnValue{ false }; // ������������ �������� ��������.
	int DirectionArr[] = { 1,2,3,4 }; // �������� ����������� ��������� �������. ����� ����� ��������� � ��������� �������.
	int DirectionIndex{ 0 }, tmp{ 0 };
	// ������ ������ � ����������� � �������, ���������� ��� ������ ���������� ������ ������� ShipNumber
	int **StartCells = new int*[nLines];
	for (int i = 0; i < nLines; i++) {
		StartCells[i] = new int[nColumns];
		for (int j = 0; j < nColumns; j++) {
			StartCells[i][j] = TakenCellsField[i][j]; // ������� �������������� �������� �������� ������ �� ���������� ��������.
		}
	}
	// ���������� � ��������� ������� ��������� ������ � �������� ��������� �������, ������� � ���� ������. 
	while (true) {
		ReturnValue = false;
		// ���������� ���������� nFree ��������� ��� ������ ����� 
		for (int i = 0; i < nLines; i++) {
			for (int j = 0; j < nColumns; j++) {
				if (StartCells[i][j] == 0) nFree++;
			}
		}
		if (nFree <= 0) break; // ���� ��� ��������� ����� - �����.
		RandomCellNumber = 1 + rand() % nFree;  // ����� ��������� ������ ����� ��������� ����� (�� 1 �� nFree ������������).
		// ���������� ������� ������ � ������� ��� ��������� ������
		for (int i = 0; i < nLines; i++) {
			for (int j = 0; j < nColumns; j++) {
				if (StartCells[i][j] == 0) RandomCellNumber--;
				if (RandomCellNumber == 0) {
					i_start = i;
					j_start = j;
					break;
				}
			}
			if (RandomCellNumber == 0) break;
		}
		// ������������ ����������� � ������� DirectionArr[] � ��������� �������
		for (int i = 0; i < 3; i++) {
			DirectionIndex = i + rand() % (4 - i);
			tmp = DirectionArr[i];
			DirectionArr[i] = DirectionArr[DirectionIndex];
			DirectionArr[DirectionIndex] = tmp;
		}
		// ������������� �������, ������� � ������ � �������� [i_start][j_start] ��������� �� ������� ����������� �� DirectionArr[]
		for (int i = 0; i < 4; i++) {
			if (PlaceShipInDirection(nLines, nColumns, ShipsField, StartCells, ArrShipSize, ArrShape, FleetPosition, ShipNumber,
				i_start, j_start, DirectionArr[i])) {   // ���� ������� �������� �������
				ReturnValue = true;
				break;
			}
		}
		if (ReturnValue == true) break;  // ���� ������� �������� ������� - ������� �� ����� while()
		else {   // ���� ������� �� ��� ��������, �� ��� ��������� ������ �������� �������
			StartCells[i_start][j_start] = 1;
		}
	}
	if (!ReturnValue) {   // ���� �� ������� ���������� ������� �������������, �� �������� ��������� ��������� �������� ����.
		system("cls");  
		cout << "\n\n\n" << endl;
		PrintField(nLines, nColumns, ShipsField, Shot, TakenCellsField, FleetAlive, ReportKill);
	}
	// ������� ������������ ������
	for (int i = 0; i < nLines; i++) delete[]StartCells[i];
	delete[]StartCells;
	//
	return ReturnValue; // ����� �� ������� PlaceOneShipAutomatically()
}
//
bool PlaceShipInDirection(int nLines, int nColumns, int **ShipsField, int **TakenCellsField, int *ArrShipSize, int ***ArrShape, 
	int ***FleetPosition, int ShipNumber, int StartLine_i, int StartColumn_j, int Direction)  // ��������� ������� � ����������� Direction
	// ������� � ������ � ��������� [StartLine_i][StartColumn_j]. � ������� ShipsField[][] � ������, ������� �������, ���������� �����
	// ������� ShipNumber. � ������� Fleet[ShipNumber-1]->position[][2] ���������� ������� ������ ������� [i_������][j_�������] �� ������� ����.
	// ���������� false ���� ���������� ������� �� ����������.
{
	int tmp_i{ 0 }, tmp_j{ 0 };  // ������� ������: tmp_i - ������ ������, tmp_j - ������ �������.
	// ���������, �������� �� ������ ��� �������:
	for (int i = 0; i < ArrShipSize[ShipNumber - 1]; i++) {
		if (Direction == 1) {  // ������ ������� ������ ��� ����������� '�����'
			tmp_i = StartLine_i - ArrShape[ShipNumber - 1][i][0]; // ������ ������ ��� ����������� ������.
			tmp_j = StartColumn_j;  // ������ ������� ��� ����������� ������.
		}
		else if (Direction == 2) {  // ������ ������� ������ ��� ����������� '������'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j + ArrShape[ShipNumber - 1][i][0];
		}
		else if (Direction == 3) {  // ������ ������� ������ ��� ����������� '����'
			tmp_i = StartLine_i + ArrShape[ShipNumber - 1][i][0];
			tmp_j = StartColumn_j;
		}
		else if (Direction == 4) {  // ������ ������� ������ ��� ����������� '������'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j - ArrShape[ShipNumber - 1][i][0];
		}
		else {
			cout << "������!!! �������� ���������� Direction ��� ���������." << endl;
			return false;
		}
		// ��������� ������ � ����������� ��������� [tmp_i][tmp_j] �� ����������� ��������� ���� �������
		if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns) {  // ���� ����� �� ������� ����
			return false;
		}
		else if (TakenCellsField[tmp_i][tmp_j] != 0) {  // ���� ������ �� '�������' ������.
			return false;
		}
	}
	// ���� ����� ��� ������� �������� - ����������� ��� �������
	for (int i = 0; i < ArrShipSize[ShipNumber - 1]; i++) {
		// ���������� ������� ������ � ������� ��� ��������� ������ �������
		if (Direction == 1) {  // ������ ������� ������ ��� ����������� '�����'
			tmp_i = StartLine_i - ArrShape[ShipNumber - 1][i][0]; // ������ ������ ��� ����������� ������.
			tmp_j = StartColumn_j;  // ������ ������� ��� ����������� ������.
		}
		else if (Direction == 2) {  // ������ ������� ������ ��� ����������� '������'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j + ArrShape[ShipNumber - 1][i][0];
		}
		else if (Direction == 3) {  // ������ ������� ������ ��� ����������� '����'
			tmp_i = StartLine_i + ArrShape[ShipNumber - 1][i][0];
			tmp_j = StartColumn_j;
		}
		else if (Direction == 4) {  // ������ ������� ������ ��� ����������� '������'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j - ArrShape[ShipNumber - 1][i][0];
		}
		// ������� ��� ������ ������� � �������
		ShipsField[tmp_i][tmp_j] = ShipNumber; // � ������ �� ����, ������� �������, ���������� ����� (�� ������) �������.
		FleetPosition[ShipNumber - 1][i][0] = tmp_i + 1; //  � ��������� ������� ���������� '����� ������' (�� ������) ������ ������. 
		FleetPosition[ShipNumber - 1][i][1] = tmp_j+1;  //  � ��������� ������� ���������� '����� �������' (�� ������) ������ ������. 
	}
	return true; // ����� �� ������� PlaceShipInDirection()
}
//
bool getAutoCellForShot(int*Line, int*Column, int nLines, int nColumns, int **CellsForShot,  int LastShot_Line,
	int LastShot_Column)   // ������������� ���������� ���������� ������ ���� ����� ������ �������. ����� ������ � ������� (�� �������) 
	  // ���������� � Line � Column. ���������� true � ������ ������, false - ���� �� ������� ����� ��������� ������ ��� ��������.
{
	int nFree{ 0 }; // ���������� ��������� ��� �������� ����� 
	int RandomCellNumber{ 0 }; // ����� ��������� ������ (�� 1 �� nFree ������������).
	bool ReturnValue{ false }; // ������������ �������� ��������.
	// ���������� ���������� ��������� ��� �������� ����� 
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (CellsForShot[i][j] == 0) nFree++;
		}
	}
	if (nFree <= 0) return false; // ���� ��� ��������� ����� - �����.
	RandomCellNumber = 1 + rand() % nFree;  // ����� ��������� ������ ����� ��������� ����� (�� 1 �� nFree ������������).
	// ���������� ����� ������ � ������� (�� �������) ��� ��������� ��������� ������
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (CellsForShot[i][j] == 0) RandomCellNumber--;
			if (RandomCellNumber == 0) {
				*Line = i + 1;
				*Column = j + 1;
				break;
			}
		}
		if (RandomCellNumber == 0) break;
	}
	return true; // ����� �� ������� getAutoCellForShot()
}
//
inline void PrintSpaces(int n_spaces)  // �������� ������, ��������� �� ��������� ���������� ��������
{
	for (int i = 0; i < n_spaces; i++) cout << " ";
}
//
void PauseWithStars() // ������������� ����� - �������� ������ �� ���������� �������� * 
{
	for (int i = 0; i < 79; i++) {
		cout << '*';
		Sleep(10);
	}
	cout << '\r';
	for (int i = 0; i < 80; i++) {
		cout << ' ';
		Sleep(10);
	}
}