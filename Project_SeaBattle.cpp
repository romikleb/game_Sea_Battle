// Игра "Морской Бой"
// Автор: Лебедев Роман, учебная группа ВПД-912
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
void PrintPreamble(); // Печать вступления
char getChar();  // Ввод символа Char с проверкой
int getInt();  // Ввод числа типа int с проверкой
int getInt_WithValueCheck(int Min, int Max); // Ввод числа типа int с проверкой: возвращаются значения только от Min до Max включительно.
//
bool getCellCoordinate(int*Line, int*Column, int MaxLineNumber, int MaxColumnNumber); // Ввод координаты клетки поля в привычном 
					  // для игры "Морской бой" формате. Например 'г4' или 'д9'. Возвращает false если пользователь введёт '0'.
//
void PrintField(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCells, bool **FleetAlive, bool ReportKill);  // Печатает игровое поле. 
//
void SetColor(int text, int bg=0);    // Изменение цвета печатаемого текста и фона. Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, 
								   // Magenta = 5, Brown = 6, LightGray = 7, DarkGray = 8, LightBlue = 9, LightGreen = 10, LightCyan = 11, 
								   //  LightRed = 12, LightMagenta = 13, Yellow = 14, White = 15. Стандарный цвет текста - SetColor(7,0).
//
int FindTakenCells(int **TakenCellsField, int nLines, int nColumns, int **ShipsField, int TouchShip);  // Находит 'занятые' ячейки - ячейки
				  // занятые кораблями в ShipsField[][] и, если указано в TouchShip, ячейки рядом с кораблями. Информацию о свободных
				   // и занятых ячейках записывает в TakenCellsField[][]: 0-свободная, 1-занята кораблём, 2-рядом с кораблём. 
				  // Возвращает количество свободных ячеек. Если нет свободных ячеек, то возвращает 0.
//
int FindCellsForShot(int **CellsForShot, int nLines, int nColumns, int **VisibleShipsField, int TouchShip, bool **FleetAlive,
	int **Shot, bool ReportKill); //  Записывает в CellsForShot[][] информацию о ячейках, свободных для выстрела.  
			 // 0-свободная ячейка,  1-занята кораблём, 2-занята из-за "Убитого" корабля по соседству, 3-занята, в неё уже стреляли. 
			  //  Возвращает количество свободных ячеек. Если нет свободных ячеек, то возвращает 0.
//
bool PlaceShips(int nLines, int nColumns, int **ShipsField, int **Shot, int *ArrShipSize, int ***ArrShape, int ***FleetPosition,
	int nShips, int ShipNumber, int TouchShip, bool Auto, bool **FleetAlive, bool ReportKill); // Рекурсивная функция. Размещает корабли на поле. Возвращает false если разместить не удалось. 
//
bool PlaceOneShipByHand(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill); // Размещение кораблей на поле вручную. Возвращает false если разместить не удалось.
//
bool PlaceOneShipAutomatically(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill); // Автоматическое размещение кораблей на поле. Возвращает false если разместить не удалось.
//
bool PlaceShipInDirection(int nLines, int nColumns, int **ShipsField, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	int ***FleetPosition, int ShipNumber, int StartLine_i, int StartColumn_j, int Direction);  // Размещает корабль в направлении Direction. 
	// В массиве ShipsField[][] в клетки, занятые кораблём, записываем номер корабля ShipNumber. В массиве Fleet[ShipNumber-1]->position[][2] записываем 
	// индексы клеток корабля [i_Строки][j_Столбца] на поле. Возвращает false если разместить корабль не получилось.
//
bool getAutoCellForShot(int*Line, int*Column, int nLines, int nColumns, int **CellsForShot, int LastShot_Line,
	int LastShot_Column);   // Автоматически определяет координаты ячейки куда будет сделан выстрел. Номер строки и столбца (не индексы) 
	  // записывает в Line и Column. Возвращает true в случае успеха, false - если не удалось найти свободную ячейку для выстрела.
//
inline void PrintSpaces(int n_spaces);  // Печатает строку, состоящую из заданного количества пробелов
//
void PauseWithStars(); // Анимированная пауза - печатает строку из исчезающих звёздочек * 
//
//
int main()
{
	setlocale(LC_ALL, "Russian");
	srand(time(0));
	char Exit{ 0 };  // Переменная для выхода из программы.
	const char *RussianSymbols[] = { "а","б","в","г","д","е","ж","з","и","к","л","м","н","о","п","р","с","т","у",
		"ф","х","ц","ч","ш","щ","э","ю","я" }; // Русские символы для обозначения строк игрового поля.
	int nLines{ 0 }, nColumns{ 0 };  // Размер игрового поля.
	int nShips{ 0 };  //  Количество кораблей.
	int TouchShip{ 0 }; // Могут ли корабли касаться друг друга: 0-нет, 1-могут касаться углами, 2-могут касаться без ограничений.
	bool ReportKill{ true }; // Выдавать сообщение "Убит" при попадании во все клетки корабля.
	bool AutoPlace{ false }; // Размещать суда автоматически.
	bool PossiblePlaceShips{ true }; // Возможность разместить корабли на поле. Возвращается функцией PlaceShipsAutomatically().
	const int StandartN_Ships{ 10 }; // Количество кораблей для Режима 1 - "Стандартного".
	const int ArrStandartShipSize[StandartN_Ships]={ 4,3,3,2,2,2,1,1,1,1 }; // Размеры кораблей для Режима 1 - "Стандартного".
	int GameMode{ 0 }; // Режим игры (1-стандартные настройки, 2-пользовательские настройки). 
	int MaxFieldLines{ 28 }, MaxFieldColumns{ 38 };  // Максимальные размеры игрового поля при выборе в Режиме 2 "С настройками".
	int nShipsMax{ 99 }; // Максимально возможное количество кораблей в Режиме 2 "С настройками".
	int SizeAndShapeMode{ 0 }; // Режим формы и размера кораблей - для Режима 2 "С настройками". 0 - произвольное количество, размер
	                           //  и форма кораблей; 1 - 'стандартное' количество и форма кораблей; 2 - количество и максимальный размер
	                          // кораблей задаются пользователем, размер и форма каждого корабля - случайная.
	int Queue{ 0 };   // Для определения того, кто делает первый выстрел.
	struct TheShip {  // Структура 'Корабль'. Информация о размере находится в ArrShipSize[], о форме - в ArrShape[][][].
		bool Alive=true;  // Состояние корабля 'Жив'. Если корабль убит (все клетки корабля подбиты), то Alive==false. 
		int **position;  // Массив размером [ArrShipSize[i]][2] с координатами (не индексами) [Строка][Столбец] клеток корабля на поле.
		int *hit;  // Массив с информацией о количестве попаданий в конкрентые клетки корабля. 0 - в клетку не было попаданий.
	};
	struct Player {
		int **ShipsField; // Клетки поля в которых находятся корабли. В занятые клетки записывается номер (не индекс) корабля.
		int **VisibleShipsField; // Корабли, 'видимые' противнику, т.е. клетки, в которые попали. В 'видимую' клетку записывается номер корабля.
		int **ShotField;  // Полученные от противника выстрелы. Нет полученных выстрелов - 0. Каждый новый выстрел - прибавляем 1.
		int **CellsForShot; // Информация о ячейках, свободных для выстрела. 0-свободная ячейка, 1-занята кораблём,
		                      // 2-занята из-за "Убитого" корабля по соседству. 
		TheShip **Fleet;  // Флот игрока - массив указателей на структуры TheShip.
		bool Alive= true; // Состояние игрока 'Жив': если все корабли подбиты, то Alive==false.
		int LastShot_Line{ 0 }, LastShot_Column{ 0 };  // Значения номера строки и столбца (не индексы) ячейки, 
		                                               // в которую последний раз стрелял противник.
	};
	//
	do {
		{  // Вложенный блок - для уничтожения в конце каждой игры созданных структур   
			Exit= 0;
			TouchShip = 0;
			ReportKill = true;
			AutoPlace = false;
			PossiblePlaceShips = true;
			Queue = 0;
			system("cls");
			PrintPreamble();  // Печатает преамбулу с окончанием: "Выберите режим: 1 - Стандартный; 2 - С настройками. Ваш выбор (1 или 2): "
			GameMode = getInt_WithValueCheck(1, 2);
			// Задание настроек для разных режимов игры
			if (GameMode == 1) {   // Задание РЕЖИМА 1 - "Стандартного"
				nLines = 10;
				nColumns = 10;
				nShips = StandartN_Ships;
				cout << "\nУкажите, могут ли корабли касаться друг друга" << endl;
				cout << "  0 - не касаются, " << endl;
				cout << "  1 - могут касаться углами, " << endl;
				cout << "  2 - могут касаться без ограничений." << endl;
				cout << "Ваш выбор: ";
				TouchShip = getInt_WithValueCheck(0, 2);
				cout << "\nПисать сообщение \"Убит\" при попадании во все клетки корабля?" << endl;
				cout << " 0 - не писать," << endl;
				cout << " 1 - писать \"Убит\"." << endl;
				cout << "Ваш выбор: ";
				ReportKill = static_cast<bool>(getInt_WithValueCheck(0, 1));
			}
			else if (GameMode == 2) {   // Задание РЕЖИМА 2 - "Режим с настройками игрока"
				cout << "\nВведите размеры игрового поля." << endl;
				cout << "  - количество строк (от 1 до " << MaxFieldLines << "):";
				nLines = getInt_WithValueCheck(1, MaxFieldLines);
				cout << "  - количество столбцов (от 1 до " << MaxFieldColumns << "):";
				nColumns = getInt_WithValueCheck(1, MaxFieldColumns);
				cout << "\nПисать сообщение \"Убит\" при попадании во все клетки корабля?" << endl;
				cout << " 0 - не писать," << endl;
				cout << " 1 - писать \"Убит\"." << endl;
				cout << "Ваш выбор: ";
				ReportKill = static_cast<bool>(getInt_WithValueCheck(0, 1));
				cout << "\nУкажите, могут ли корабли касаться друг друга" << endl;
				cout << "  0 - не касаются, " << endl;
				cout << "  1 - могут касаться углами, " << endl;
				cout << "  2 - могут касаться без ограничений." << endl;
				cout << "Ваш выбор: ";
				TouchShip = getInt_WithValueCheck(0, 2);
				cout << "Использовать \'стандартное\' количество и форму кораблей?" << endl;
				cout << "  0 - Да, использовать \'стандартные\' количество и форму кораблей," << endl;
				cout << "  1 - Нет, установить количество и форму вручную.";
				SetColor(12);
				cout << "(Пока недоступно)" << endl;
				SetColor(7);
				cout << "Ваш выбор: ";
				SizeAndShapeMode= getInt_WithValueCheck(0, 1);
				if (SizeAndShapeMode == 0) 	nShips = StandartN_Ships; 
				else if (SizeAndShapeMode == 1){
					cout << "\nВведите количество кораблей (от 1 до " << nShipsMax << "): ";
					nShips = getInt_WithValueCheck(1, nShipsMax);
				}
			}
			int *ArrShipSize = new int[nShips];  // Массив с размерами (количеством клеток) каждого корабля. Общий для обоих игроков.
			// Устанавливаем размеры кораблей (размеры и форма кораблей будут одинаковые для обоих игроков):
			if (GameMode == 1 || (GameMode == 2 && SizeAndShapeMode == 0)) 
				for (int i = 0; i < nShips; i++) ArrShipSize[i] = ArrStandartShipSize[i];  
			else if (GameMode == 2) {
				cout << "\nЗадайте размеры кораблей (от 1 до " << nLines * nColumns << "). Корабли будут " << endl;
				cout << "одинакового размера и формы у обоих игроков." << endl;
				for (int i = 0; i < nShips; i++) {
					cout << "Введите размер (количество клеток) корабля № " << i + 1 << ": ";
					ArrShipSize[i] = getInt_WithValueCheck(1, nLines* nColumns);
				}
			}
			// Создаём массивы для дальнейшей записи форм кораблей и инициализируем нулями. Форма кораблей будет одинаковая у обоих игроков.
			int ***ArrShape = new int**[nShips];
			for (int i = 0; i < nShips; i++) {
				ArrShape[i] = new int*[ArrShipSize[i]];
				for (int j = 0; j < ArrShipSize[i]; j++) {
					ArrShape[i][j] = new int[2];
					ArrShape[i][j][0] = 0;  // Относительная координата x клетки j для корабля i. 
					ArrShape[i][j][1] = 0;  // Относительная координата y клетки j для корабля i. 
				}
			}
			system("cls");
			// Создаём данные игрока User
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
				for (int j = 0; j < nColumns; j++) {  // Инициализируем нулями. 
					User.ShipsField[i][j] = 0;
					User.VisibleShipsField[i][j] = 0;
					User.ShotField[i][j] = 0;
					User.CellsForShot[i][j] = 0;
				}
			}
			// Создаём корабли игрока User и инициируем их поля нулями
			User.Fleet = new TheShip*[nShips];
			int ***UserFleetPosition = new int**[nShips];  // Массив с указателями на массивы **position игрока User (для передачи в функции).
			bool **UserFleetAlive = new bool*[nShips];  // Массив с указателями на переменную Alive каждого корабля игрока User (для передачи в функции).
			for (int i = 0; i < nShips; i++) {
				User.Fleet[i] = new TheShip;  
				User.Fleet[i]->position = new int*[ArrShipSize[i]];  // Создаём массив с координатами [Строка][Столбец] (не индексами)  клеток корабля на поле.
				User.Fleet[i]->hit = new int[ArrShipSize[i]]; // Массив для записи попаданий в клетки корабля. 
				UserFleetPosition[i] = User.Fleet[i]->position;
				UserFleetAlive[i] = &User.Fleet[i]->Alive; 
				*UserFleetAlive[i] = true;   // Инициализируем для всех кораблей User переменные Alive.
				for (int j = 0; j < ArrShipSize[i]; j++) {  
					User.Fleet[i]->position[j] = new int[2]; 
					User.Fleet[i]->position[j][0] = 0;  // Координата 'номер строки' (не индекс массива)  данной ячейки корабля на поле. 
					User.Fleet[i]->position[j][1] = 0;  // Координата 'номер столбца' (не индекс массива) данной ячейки корабля на поле. 
					User.Fleet[i]->hit[j] = 0; // Попаданий в эту ячейку нет.
				}
			}
			// Расставляем корабли игрока User
			if (GameMode == 1 || (GameMode == 2 && SizeAndShapeMode == 0)) {   // Для РЕЖИМА 1 - "Стандартного"
						//  Расставляем корабли игрока User на поле:
				cout << "\n  Расставьте корабли на поле:" << endl;
				// Задание 'стандартной' формы кораблей
				for (int i = 0; i < nShips; i++) {
					for (int j = 0; j < ArrStandartShipSize[i]; j++) {
						ArrShape[i][j][0] = j; // Относительный номер строки  
						ArrShape[i][j][1] = 0; // Относительный номер столбца
					}
				}
				PrintField(nLines, nColumns, User.ShipsField, User.ShotField, User.CellsForShot, UserFleetAlive, ReportKill);
				cout << "\n\n    Расставить корабли автоматически? (0-нет; 1-да): ";
				AutoPlace = static_cast<bool>(getInt_WithValueCheck(0, 1));
				if (!PlaceShips(nLines, nColumns, User.ShipsField, User.ShotField, ArrShipSize, ArrShape, UserFleetPosition, nShips, 1,
					TouchShip, AutoPlace, UserFleetAlive, ReportKill)) {    // Размещаем корабли игрока User на поле.  
					cout << "\nНе удалось разместить на поле корабли игрока User."<<endl;  
					cout << "\nДля выхода введите \'9\', для начала новой игры - любой другой символ: ";
					Exit = getChar();
					if (Exit == '9')  break;
					else continue;
				}
				else cout << "\n              ВСЕ  КОРАБЛИ  УСПЕШНО  РАЗМЕЩЕНЫ" << endl;   
				Sleep(3000);
				PauseWithStars();  
			}
			else if (GameMode == 2) { 
				SetColor(12);
				cout << "Данная функция пока не доступна " << endl;
				SetColor(7);
				cout << "\nДля выхода введите \'9\', для начала новой игры - любой другой символ: ";
				Exit = getChar();
				if (Exit == '9')break;
				else continue;
			}
			//
			// Создаём данные игрока Computer
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
				for (int j = 0; j < nColumns; j++) {  // Инициализируем нулями. 
					Computer.ShipsField[i][j] = 0;
					Computer.VisibleShipsField[i][j] = 0;
					Computer.ShotField[i][j] = 0;
					Computer.CellsForShot[i][j] = 0;
				}
			}
			// Создаём корабли игрока Computer и инициируем их поля нулями
			Computer.Fleet = new TheShip*[nShips];
			int ***ComputerFleetPosition = new int**[nShips];  // Массив с указателями на массивы **position игрока Computer для передачи в функции.
			bool **ComputerFleetAlive = new bool*[nShips];  // Массив с указателями на переменную Alive каждого корабля игрока Computer (для передачи в функции).
			for (int i = 0; i < nShips; i++) {
				Computer.Fleet[i] = new TheShip;
				Computer.Fleet[i]->position = new int*[ArrShipSize[i]];  // Создаём массив с координатами [Строка][Столбец] (не индексами)  клеток корабля на поле.
				Computer.Fleet[i]->hit = new int[ArrShipSize[i]]; // Массив для записи попаданий в клетки корабля. 
				ComputerFleetPosition[i] = Computer.Fleet[i]->position;
				ComputerFleetAlive[i] = &Computer.Fleet[i]->Alive;
				*ComputerFleetAlive[i] = true;   // Инициализируем для всех кораблей Computer переменные Alive.
				for (int j = 0; j < ArrShipSize[i]; j++) {
					Computer.Fleet[i]->position[j] = new int[2];
					Computer.Fleet[i]->position[j][0] = 0;  // Координата 'номер строки' (не индекс массива)  данной ячейки корабля на поле. 
					Computer.Fleet[i]->position[j][1] = 0;  // Координата 'номер столбца' (не индекс массива) данной ячейки корабля на поле. 
					Computer.Fleet[i]->hit[j] = 0; // Попаданий в эту ячейку нет.
				}
			}
			// Расставляем корабли игрока Computer 
			if (!PlaceShips(nLines, nColumns, Computer.ShipsField, Computer.ShotField, ArrShipSize, ArrShape, ComputerFleetPosition, nShips, 1, TouchShip, true, ComputerFleetAlive, ReportKill)) {
				cout << "\nНе удалось разместить на поле корабли игрока Computer." << endl;   // Размещаем корабли на поле.  
				cout << "\nДля выхода введите \'9\', для начала новой игры - любой другой символ: ";
				Exit = getChar();
				if (Exit == '9')  break;
				else continue;
			}
			system("cls");
			cout << "                        Поле с Вашими кораблями " << endl;
			cout << "  ---------------------------------------------------------------------------" << endl;
			PrintField(nLines, nColumns, User.ShipsField, User.ShotField, User.CellsForShot, UserFleetAlive, ReportKill);
			cout << "\n                     Поле компьютера  " << endl;
			cout << "  ---------------------------------------------------------------------------" << endl;
			PrintField(nLines, nColumns, Computer.VisibleShipsField, Computer.ShotField, Computer.CellsForShot, ComputerFleetAlive, ReportKill);
			//  Определим, кто делает первый выстрел
			cout << "\n\nОпеределим, кто делает первый выстрел." << endl;
			cout << "\nКомпьютер загадал число: 0 или 1. Угадайте его." << endl;
			cout << "Ваш вариант: ";
			Queue = getInt_WithValueCheck(0, 1); 
			if (Queue == rand() % 2) {  // Если User угадал число, то
				Queue = 0;  // Очередь игрока User
				cout << "Поздравляем! Вы угадали! Ваш выстрел первый." << endl;
			}
			else {
				Queue = 1;  // Очередь игрока Computer
				cout << "Вы не угадали.  Первым стреляет компьютер." << endl;
			}   
			Sleep(2000);
			PauseWithStars();
			//
			//----------  ПРОЦЕСС ИГРЫ  --------------------------------------------------------------------
			while (true) {
				// Если один из игроков убит, то выход
				User.Alive = false;
				Computer.Alive = false;
				for (int i = 0; i < nShips; i++) {
					if (User.Fleet[i]->Alive == true) User.Alive = true;  
					if (Computer.Fleet[i]->Alive == true) Computer.Alive = true;
				}
				if (User.Alive == false || Computer.Alive == false) break; // Если один из игроков убит, то конец игры.
				User.Alive = true;
				Computer.Alive = true;
				system("cls");
				cout << "                        Поле с Вашими кораблями " << endl;
				cout << "  ---------------------------------------------------------------------------" << endl;
				PrintField(nLines, nColumns, User.ShipsField, User.ShotField, User.CellsForShot, UserFleetAlive, ReportKill);
				cout << "\n                     Поле компьютера  " << endl;
				cout << "  ---------------------------------------------------------------------------" << endl;
				PrintField(nLines, nColumns, Computer.VisibleShipsField, Computer.ShotField, Computer.CellsForShot, ComputerFleetAlive, ReportKill);
				cout << "\n";
				// ---------- Очередь игрока User делать выстрел ----------
				if (Queue == 0)  {
					int i_Short{ 0 }, j_Short{ 0 }; // Индексы строки и столбца ячейки, куда делается выстрел.
					cout << "Ваш выстрел." << endl;
					cout << "Укажите ячейку для выстрела (например \'г8\'): ";
					//  Вводим координаты ячейки, куда происходит выстрел  
					getCellCoordinate(&i_Short, &j_Short, nLines, nColumns); // В i_Short и j_Short ввели номера (не индекс) строки и столбца.
					i_Short--;  
					j_Short--;
					Computer.ShotField[i_Short][j_Short]++; // Выстрел записан в массив ShotField[][] игрока Computer.
					if (Computer.ShipsField[i_Short][j_Short] != 0) {   // Если выстрел попал в ячейку с кораблём
						Computer.VisibleShipsField[i_Short][j_Short] = Computer.ShipsField[i_Short][j_Short];  // то записываем туда номер корабля.
						// Отметим попадание в конкретную- клетку корабля 
						for (int i = 0; i < ArrShipSize[Computer.ShipsField[i_Short][j_Short] - 1]; i++) {
							if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->position[i][0] == (i_Short + 1) &&
								Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->position[i][1] == (j_Short + 1)) {
								Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->hit[i]++; // Записали попадание в конкретную клетку корабля.
								break;
							}
						}
						// Определяем, 'Убит' ли корабль (если есть попадания во все клетки корабля)
						if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->hit[0] > 0) {  // Проверили попадание в первую клетку корабля
							Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->Alive = false;
							for (int i = 1; i < ArrShipSize[Computer.ShipsField[i_Short][j_Short] - 1]; i++) {
								if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->hit[i] == 0) {
									Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->Alive = true;
									break; // Если хоть один квадрат корабля не имеет попаданий, то его Alive = true.
								}
							}
						}
						cout << "    ПОПАЛ !!!   " << endl;
						Sleep(1500);
						if (Computer.Fleet[Computer.ShipsField[i_Short][j_Short] - 1]->Alive == false &&
							ReportKill == true) { // Если корабль 'Убит' и в настройках указано "Сообщать фразу 'Убит'" 
							SetColor(12);
							cout << "           УБИЛ !!!" << endl;
							SetColor(7);
						}
						// Заполняем массив Computer.CellsForShot[][]. 0-свободная ячейка, 1-занята кораблём. Если в настройках 
						// указано "Сообщать фразу 'Убит'", то рядом с 'Убитым' кораблём отмечаем занятые ячейки значением 2,
						// учитывая тип касания кораблей TouchShip.
						if (FindCellsForShot(Computer.CellsForShot, nLines, nColumns, Computer.VisibleShipsField, TouchShip,
							ComputerFleetAlive, Computer.ShotField, ReportKill) == 0) {   // Заполнили Computer.CellsForShot[][].
																	  // Если нет свободных для выстрела ячеек, то выход.
							cout << "Нет свободных ячеек для выстрела" << endl;
							break;
						}
						Sleep(2000);
						PauseWithStars();
						continue;
					}
					else {  // Если выстрел попал в ячейку без корабля
						cout << "  МИМО " << endl;
						Sleep(2000);
						PauseWithStars();
						Queue = 1; //  Очередь следующего выстрела переходит игроку Computer.
						continue;
					}
				}
				// ----------  Очередь игрока Computer делать выстрел  ----------
				if (Queue == 1)  {
					int i_Short{ 0 }, j_Short{ 0 }; // Индексы строки и столбца ячейки, куда делается выстрел.
					cout << "Выстрел делает Компьютер." << endl;
					Sleep(1500);
					// Заполняем массив User.CellsForShot[][] данными о ячейках, свободных для выстрела.
					if(FindCellsForShot(User.CellsForShot, nLines, nColumns, User.VisibleShipsField, TouchShip,
						UserFleetAlive, User.ShotField, ReportKill)==0) {   // Заполнили User.CellsForShot[][]. Если нет свободных для выстрела ячеек, то выход.
						cout << "Нет свободных ячеек для выстрела" << endl;
						break;
					}
					//  Автоматически определяем координаты ячейки, куда происходит выстрел  
					if (!getAutoCellForShot(&i_Short, &j_Short, nLines, nColumns, User.CellsForShot, User.LastShot_Line,
						User.LastShot_Column)) {
						cout << "Нет свободных ячеек для выстрела" << endl;
						break;
					};
					User.LastShot_Line = i_Short;
					User.LastShot_Column = j_Short;
					i_Short--;
					j_Short--;
					cout << "Выстрел в ячейку \'" << RussianSymbols[i_Short] << j_Short + 1 <<"\'"<< endl;
					Sleep(2000);
					User.ShotField[i_Short][j_Short]++; // Выстрел записан в массив ShotField[][] игрока User.
					if (User.ShipsField[i_Short][j_Short] != 0) {   // Если выстрел попал в ячейку с кораблём
						User.VisibleShipsField[i_Short][j_Short] = User.ShipsField[i_Short][j_Short];  // то записываем туда номер корабля.
						// Отметим попадание в конкретную клетку корабля 
						for (int i = 0; i < ArrShipSize[User.ShipsField[i_Short][j_Short] - 1]; i++) {
							if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->position[i][0] == (i_Short + 1) &&
								User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->position[i][1] == (j_Short + 1)) {
								User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->hit[i]++; // Записали попадание в конкретную клетку корабля.
								break;
							}
						}
						// Определяем, 'Убит' ли корабль (если есть попадания во все клетки корабля)
						if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->hit[0] > 0) {  // Проверили попадание в первую клетку корабля
							User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->Alive = false;
							for (int i = 1; i < ArrShipSize[User.ShipsField[i_Short][j_Short] - 1]; i++) {
								if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->hit[i] == 0) {
									User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->Alive = true;
									break; // Если хоть один квадрат корабля не имеет попаданий, то его Alive = true.
								}
							}
						}
						cout << "    ПОПАЛ !!!   " << endl;
						Sleep(1500);
						if (User.Fleet[User.ShipsField[i_Short][j_Short] - 1]->Alive == false &&
							ReportKill == true) { // Если корабль 'Убит' и в настройках указано "Сообщать фразу 'Убит'" 
							SetColor(12);
							cout << "           УБИЛ !!!" << endl;
							SetColor(7);
						}
						// Определяем свободные для выстрела ячейки
						if (FindCellsForShot(User.CellsForShot, nLines, nColumns, User.VisibleShipsField, TouchShip, UserFleetAlive,
							User.ShotField, ReportKill) == 0) {   // Заполнили User.CellsForShot[][].
							                               // Если нет свободных для выстрела ячеек, то выход.
							cout << "Нет свободных для выстрела ячеек" << endl;
							continue;
						}
						Sleep(2000);
						PauseWithStars();
						continue;
					}
					else {  // Если выстрел попал в ячейку без корабля
						cout << "  МИМО " << endl;
						Sleep(2000);
						PauseWithStars();
						Queue = 0; //  Очередь следующего выстрела переходит игроку User.
						continue;
					}
				}
			}
			// Очистка динамической памяти:
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
				cout << "                          РЕЗУЛЬТАТ  ИГРЫ: " << endl;
				cout << endl;
				if (User.Alive == true) {
					cout << "                        ВЫ  ПОБЕДИЛИ !!!!!  " << endl;
					cout << endl;
					cout << "                          ПОЗДРАВЛЯЕМ !!!" << endl;
				}
				if (Computer.Alive == true) cout << "               ПОБЕДИЛ КОМПЬЮТЕР.  Вы проиграли  :( " << endl;
				cout << endl;
				cout << "      ********************************************************" << endl;
			}
			cout << "\n\n\nДля выхода введите \'9\', для начала новой игры - любой другой символ: ";
			Exit = getChar();
		}
	} while (Exit != '9');
	return 0;  // Конец функции main()
}
//
//
void PrintPreamble() // Печать вступления
{
	cout << "\n"<<endl;
	cout << "  ------------------------------------------------------------------" << endl;
	cout << "  -----                                                        ----- " << endl;
	cout << "  -----  ИГРА  \"МОРСКОЙ  БОЙ  ПРОТИВ  КОМПЬЮТЕРА\"  версия 1.0  ----- " << endl;
	cout << "  -----                                                        ----- " << endl;
	cout << "  ------------------------------------------------------------------" << endl;
	cout << endl;
	cout << "Игра имеет два режима:" << endl;
	cout << "  - Режим 1 - \'Стандартный\'" << endl;
	cout << "  - Режим 2 - \'С настройками\'." << endl;
	cout << endl;
	cout << "Режим 1 - \'Стандартный\': " << endl;
	cout << " - игровое поле 10x10, 10 кораблей (один 4-палубный; два 3-палубных;" << endl;
	cout << "     три 2-палубных; четыре 1-палубных); " << endl;
	cout << " - корабли имеют форму прямой линии без изгибов;" << endl;
	cout << " - игрок задаёт, могут ли корабли касаться друг друга;" << endl;
	cout << " - игрок указывает, выдавать или нет сообщение \"Убит\" после попадания " << endl;
	cout << "     во все клетки корабля;" << endl;
	cout << " - возможна автоматическая расстановка кораблей игрока." << endl;
	cout << endl;
	cout << "Режим 2 - \'С настройками\':" << endl;
	cout << " - задаются размеры поля: строк N_Str= 1...28,  столбцов N_Stolb= 1...38;" << endl;
	cout << " - задаётся количество кораблей: от 1 до 99;" ;
	SetColor(12); 
	cout << "(Пока недоступно)" << endl;
	SetColor(7);
	cout << " - задаются размеры каждого корабля: от 1 до N_Str*N_Stolb;";
	SetColor(12);
	cout << "(Пока недоступно)" << endl;
	SetColor(7);
	cout << " - произвольная форма каждого корабля (задаётся игроком). У игрока" << endl;
	cout << "   и у компьютера одинаковое количество и форма кораблей;";
	SetColor(12);
	cout << "(Пока недоступно)" << endl;
	SetColor(7);
	cout << " - есть вариант со \'стандартным\' количеством и формой кораблей;" << endl;
	cout << " - корабли могут касаться друг друга (задаётся игроком);" << endl;
	cout << " - выдавать или нет сообщение \"Убит\" решает игрок;" << endl;
	cout << endl;
	cout << endl;
	cout << "Выберите режим игры:" << endl;
	cout << "  1 - Стандартный режим;" << endl;
	cout << "  2 - Режим с настройками." << endl;
	cout << "Ваш выбор (1 или 2): ";
	return;
}
//
char getChar()  // Ввод символа Char с проверкой
{
	while (true) // цикл продолжается до тех пор, пока пользователь не введёт корректное значение
	{
		char a;
		cin >> a;
		// Проверка на предыдущее извлечение
		if (cin.fail()){ // если предыдущее извлечение оказалось неудачным,
			cin.clear(); // то возвращаем cin в 'обычный' режим работы
			cin.ignore(32767, '\n'); // и удаляем значения предыдущего ввода из входного буфера
			cout << "Ошибка!!! Попробуйте ещё раз\n";
		}
		else
		{
			cin.ignore(32767, '\n'); // удаляем лишние значения
			return a;
		}
	}
}
//
int getInt()  // Ввод числа типа int с проверкой
{
	while (true) // цикл продолжается до тех пор, пока пользователь не введёт корректное значение
	{
		int a;
		cin >> a;
		// Проверка на предыдущее извлечение
		if (cin.fail()) // если предыдущее извлечение оказалось неудачным,
		{
			cin.clear(); // то возвращаем cin в 'обычный' режим работы
			cin.ignore(32767, '\n'); // и удаляем значения предыдущего ввода из входного буфера
			cout << "Ошибка!!! Должно быть целое число. Попробуйте ещё раз.\n";
		}
		else
		{
			cin.ignore(32767, '\n'); // удаляем лишние значения
			return a;
		}
	}
}
//
int getInt_WithValueCheck(int Min, int Max) // Ввод числа типа int с проверкой: возвращаются значения только от Min до Max включительно.
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
		cout << "Ошибка!!! Число должно быть от " << Min << " до " << Max << ". Попробуйте ещё раз" << endl;
	}
	return tmp;
}
//
bool getCellCoordinate(int*Line, int*Column, int MaxLineNumber, int MaxColumnNumber) // Ввод координаты клетки поля в привычном 
					  // для игры "Морской бой" формате. Например 'г4' или 'д9'. Возвращает false если пользователь введёт '0'.
{
	// Используемые символы { а, б, в, г, д, е, ж, з, и, к, л, м, н, о, п, р, с, т, у, ф, х, ц, ч, ш, щ, э, ю, я };  // 28 символов  
	int RusSymbFromCin[] = { -96,-95,-94,-93,-92,-91,-90,-89,-88,-86,-85,-84,-83,-82,-81,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-19,-18,-17 };  // 28 символов
	char LineChar{ 0 }; // Символ обозначения номера строки
	int LineInt{ 0 }, ColumnInt{ 0 };  // Номер строки и номер столбца (не путать с индексом).
	while (true) {
		LineChar = 0;
		LineInt = 0;
		ColumnInt = 0;
		while (true) {
			LineChar = 0; // Символьное обозначение номера строки
			ColumnInt = 0;  // Номер строки и номер столбца (не путать с индексом).
			cin >> LineChar;
			if (LineChar == 48) return false; // Если введён символ '0'.
			cin >> ColumnInt;
			// Проверка на предыдущее извлечение
			if (cin.fail()) // если предыдущее извлечение оказалось неудачным
			{
				cin.clear(); // то возвращаем cin в 'обычный' режим работы
				cin.ignore(32765, '\n'); // и удаляем значения предыдущего ввода из входного буфера
				cout << "Ошибка!!! Попробуйте ещё раз." << endl;
			}
			else {
				cin.ignore(32767, '\n');  // удаляем лишние значения в буфере
				break;
			}
		}
		// Определяем номер строки по введённой букве   
		for (int i = 0; i < 28; i++) {
			if (RusSymbFromCin[i] == LineChar) {
				LineInt = i + 1;  // Определили номер строки (не путать с индексом строки) по введённой букве.
				break;
			}
		}
		if (LineInt<1 || LineInt>MaxLineNumber || ColumnInt<1 || ColumnInt>MaxColumnNumber) {
			cout << "Ошибка!!! Неверное значение!!! Попробуйте ещё раз."<< endl;
		}
		else break;
	}
	*Line = LineInt; // Присваиваем номер (не индекс) строки (на 1 больше индекса массива).
	*Column = ColumnInt;  // Присваиваем номер (не индекс) столбца (на 1 больше индекса массива). 
	return true;
}
//
void PrintField(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCells, bool **FleetAlive, bool ReportKill)  // Печатает игровое поле. 
{
	if (nLines > 28) nLines = 28;
	if (nLines < 1) nLines = 1;
	if (nColumns > 38) nColumns = 38;
	if (nColumns < 1) nColumns = 1;
	int LeftSideSize = (80 - nColumns * 2 - 3) / 2;
	setlocale(LC_ALL, "C");
	const char *RussianSymbols[] = { "а","б","в","г","д","е","ж","з","и","к","л","м","н","о","п","р","с","т","у","ф","х","ц","ч","ш","щ","э","ю","я"};
	// Печатаем верхнюю строку с номерами столбцов
	PrintSpaces(LeftSideSize+2);
	for (int i = 0; i < nColumns; i++) {
		if(i>10 && (i%2!=0)) SetColor(15);
		cout << i+1;
		if (i < 9) cout << " ";
		SetColor(7);
	}
	cout << endl;
	// Печатаем верхнюю линию прямоугольника 
	PrintSpaces(LeftSideSize + 1);
	cout << static_cast<char>(218);
	for (int i = 0; i < nColumns-1; i++)  cout << static_cast<char>(196) << static_cast<char>(194);
	cout << static_cast<char>(196) << static_cast<char>(191) << endl;
	//
	// Печатаем содержимое прямоугольника между верхней и нижней линиями
	for (int i = 0; i < nLines; i++) {
		// Печать первой строки цикла - строки со знаками выстрела Shot
		setlocale(LC_ALL, "Russian");
		PrintSpaces(LeftSideSize);
		cout << RussianSymbols[i];  //  Печать буквы с номером строки (слева от игрового поля)
		setlocale(LC_ALL, "C");
		cout << static_cast<char>(179);
		for (int j = 0; j < nColumns; j++) {
			// Печать содержимого клетки поля (символ выстрела, флажок или пробел)
			if (Shot[i][j] == 0) {
				if (TakenCells[i][j] == 0) cout << " ";  //  Если в этой клетке нет ни выстрела, ни пометки TakenCells, то печатаем пробел.
				if (TakenCells[i][j] != 0) {   // Если нет выстрела, но есть пометка TakenCells, то проверяем:
					if (ShipsField[i][j] != 0) cout << " "; // если корабль, то печатаем пробел.
					else cout << static_cast<char>(251); // если корабля нет, то печатаем "флаг". 
				}
			}
			else if (ShipsField[i][j] == 0) { // Если выстрел мимо корабля - печатаем красную точку. 
				SetColor(12);
				cout << static_cast<char>(249);
				SetColor(7);
			}
			else {
				// Если выстрел в корабль и корабль 'Убит' и надо сообщать об этом, то печатаем красный 'X' (иначе по умолчанию печатаем X белым цветом)
				if (*FleetAlive[ShipsField[i][j] - 1] == false && ReportKill==true) SetColor(12);
				cout << 'X';
				SetColor(7);
			}
			// Печать символа границы клетки (или границы корабля, или границы прямоугольника поля)
			if (j == nColumns - 1) {
				cout << static_cast<char>(179);  // если конец строки - печатаем вертикальную линию
				break;
			}
			else  (ShipsField[i][j] != 0 || ShipsField[i][j + 1] != 0) ? cout << static_cast<char>(179) : cout << " "; // Печать вертикальной границы корабля.
		}
		setlocale(LC_ALL, "Russian");
		cout << RussianSymbols[i] << endl;  // печать буквы (номер строки) справа от правой границы прямоугольника
		setlocale(LC_ALL, "C");
		if (i == nLines - 1) break; // при печати последней строки вторую линию не печатаем
		//
		// Печать второй линии цикла - линия под строкой (т.е. линия разделения между строками)
		PrintSpaces(LeftSideSize+1);
		cout << static_cast<char>(195);  
		for (int j = 0; j < nColumns; j++) {
			// Печать первого символа - горизонтальная граница между клетками (при наличии корабля сверху или снизу)
			(ShipsField[i][j] == 0 && ShipsField[i + 1][j] == 0) ? cout << " ": cout << static_cast<char>(196);
			//
			// Печать второго символа - угол клетки (или правая граница прямоугольника поля)
			if (j == nColumns - 1) {
				cout << static_cast<char>(180);  // если конец строки - печатаем правую границу игрового поля
				break;
			}
				// Условие печати символа 197 ("крест") 
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
		        // Условие печати символа 218 ("верхний левый угол") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] != 0) {
				cout << static_cast<char>(218);
				continue;
			}
			    // Условие печати символа 191 ("верхний правый угол") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(191);
				continue;
			}
			    // Условие печати символа 217 ("нижний правый угол") 
			if (ShipsField[i][j] != 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(217);
				continue;
			}
				// Условие печати символа 192 ("нижний левый угол") 
			if ( ShipsField[i][j] == 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0)	{
				cout << static_cast<char>(192);
				continue;
			}
				// Условие печати символа 193 ("середина нижней стороны корабля") 
			if (ShipsField[i][j] != 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(193);
				continue;
			}
				// Условие печати символа 194 ("середина верхней стороны корабля") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] != 0) {
				cout << static_cast<char>(194);
				continue;
			}
			// Условие печати символа 195 ("середина левой стороны корабля") 
			if (ShipsField[i][j] == 0 && ShipsField[i][j + 1] != 0 && ShipsField[i + 1][j] == 0 && ShipsField[i + 1][j + 1] != 0) {
				cout << static_cast<char>(195);
				continue;
			}
			// Условие печати символа 180 ("середина правой стороны корабля") 
			if (ShipsField[i][j] != 0 && ShipsField[i][j + 1] == 0 && ShipsField[i + 1][j] != 0 && ShipsField[i + 1][j + 1] == 0) {
				cout << static_cast<char>(180);
				continue;
			}
		}
		cout << endl;
	}
	//
	// Печатаем нижнюю линию прямоугольника 
	PrintSpaces(LeftSideSize + 1);
	cout << static_cast<char>(192);
	for (int i = 0; i < nColumns - 1; i++) {
		cout << static_cast<char>(196) << static_cast<char>(193); 
	}
	cout << static_cast<char>(196) << static_cast<char>(217) << endl;
	//
	// Печатаем нижнюю строку с номерами столбцов
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
void SetColor(int text, int bg)    // Изменение цвета печатаемого текста и фона. Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, 
								   // Magenta = 5, Brown = 6, LightGray = 7, DarkGray = 8, LightBlue = 9, LightGreen = 10, LightCyan = 11, 
								   //  LightRed = 12, LightMagenta = 13, Yellow = 14, White = 15. Стандарный цвет текста - SetColor(7).
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((bg << 4) | text));
}
//
int FindTakenCells(int **TakenCellsField, int nLines, int nColumns, int **ShipsField, int TouchShip)  // Находит 'занятые' ячейки - 
               // - ячейки занятые кораблями в ShipsField[][] и, если указано в TouchShip, ячейки рядом с кораблями. 
	           // Информацию о занятых ячейках записывает в TakenCellsField[][]: 0-свободная, 1-занята кораблём, 2-рядом с кораблём. 
	           // Возвращает количество свободных ячеек. Если нет свободных ячеек, то возвращает 0.
{
	int nFree{ 0 }; // Количество свободных ячеек на поле. 
	int CoordNeighborCells[8][2] = { {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0} }; // Относительные координаты
														 // {Строка, Столбец} клеток, соседних с текущей клеткой поля. 
	for (int i = 0; i < nLines; i++) {   // Цикл по строкам.
		for (int k = 0; k < nColumns; k++) TakenCellsField[i][k] = 0; // Инициализируем нулями
		for (int j = 0; j < nColumns; j++) {  // Цикл по столбцам.
			// Условия при которых ячейка считается 'занятой' (т.е. в эту ячейку нельзя ставить корабли):
			if (ShipsField[i][j] != 0) {  // Если в ячейке находится корабль - то ячейка занята.
				TakenCellsField[i][j] = 1;  // Отмечаем цифрой 1 ячейку, занятую кораблём
				continue;  // Переходим к проверке следующей ячейки.
			}
			if (TouchShip == 0) {  // Отметим ячейку, рядом с которой (с любой стороны) есть корабль (если кораблям нельзя касаться друг друга):
				int tmp_i{ 0 }, tmp_j{ 0 };
				for (int NeighborCell = 0; NeighborCell < 8; NeighborCell++) {
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // Индекс строки соседней ячейки.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // Индекс столбца соседней ячейки.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // Ячейки за пределами поля не проверяем.
					if (ShipsField[tmp_i][tmp_j] != 0) {  // Если в соседней ячейке находится корабль
						TakenCellsField[i][j] = 2;  // Отмечаем цифрой 2 ячейку, рядом с которой (с любой стороны от ячейки) находится корабль.
						break;
					}
				}
			}
			else if (TouchShip == 1) { // Отметим ячейки по диагонали рядом с кораблями (когда корабли могут касаться друг друга углами):
				int tmp_i{ 0 }, tmp_j{ 0 };
				// Проверим на наличие кораблей сверху-снизу-справа-слева
				for (int NeighborCell = 1; NeighborCell < 8; NeighborCell += 2) {
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // Номер строки соседней ячейки.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // Номер столбца соседней ячейки.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // Ячейки за пределами поля не проверяем.
					if (ShipsField[tmp_i][tmp_j] != 0) {  // Если в соседней ячейке сверху-снизу-справа-слева находится корабль
						TakenCellsField[i][j] = 2;  // Отмечаем цифрой 2 ячейку, рядом с которой (сверху-снизу-справа-слева) есть корабль.
						break;
					}
				}
			}
		}
	}
	// Определяем количество 'свободных' ячеек nFree
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (TakenCellsField[i][j] == 0) nFree++;
		}
	}
	return nFree;
}
//
int FindCellsForShot(int **CellsForShot, int nLines, int nColumns, int **VisibleShipsField, int TouchShip, bool **FleetAlive, 
	int **Shot, bool ReportKill)  //  Записывает в CellsForShot[][] информацию о ячейках, свободных для выстрела.  
		     // 0-свободная ячейка,  1-занята кораблём, 2-занята из-за "Убитого" корабля по соседству, 3-занята (в неё уже стреляли). 
			  //  Возвращает количество свободных ячеек. Если нет свободных ячеек, то возвращает 0.
{
	int nFree{ 0 }; // Количество свободных ячеек на поле. 
	int CoordNeighborCells[8][2] = { {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0} }; // Относительные координаты
														 // {Строка, Столбец} клеток, соседних с текущей клеткой поля. 
	for (int i = 0; i < nLines; i++) {   // Цикл по строкам.
		for (int k = 0; k < nColumns; k++) CellsForShot[i][k] = 0; // Заполняем нулями.
		for (int j = 0; j < nColumns; j++) {  // Цикл по столбцам.
			// Условия при которых ячейка считается 'занятой' (т.е. в эту ячейку нельзя стрелять):
			if (VisibleShipsField[i][j] != 0) {  // Если в ячейке находится корабль - то ячейка занята.
				CellsForShot[i][j] = 1;  // Отмечаем цифрой 1 ячейку, занятую кораблём.
				continue;  // Переходим к проверке следующей ячейки.
			}
			// Для ячейки не занятой кораблём - проверяем, стреляли ли в ячейку ранее 
			if (Shot[i][j] != 0) {  
				CellsForShot[i][j] = 3;  // Отмечаем цифрой 3 ячейку,  в которую уже стреляли.
				continue;  // Переходим к проверке следующей ячейки.
			};
			// Проверяем, есть ли в соседней ячейке 'Убитый' корабль(с учётом условия TouchShip)
			if (TouchShip == 0) {  // Кораблям нельзя касаться друг друга. Отметим ячейку, с любой стороны которой есть корабль
				int tmp_i{ 0 }, tmp_j{ 0 };
				for (int NeighborCell = 0; NeighborCell < 8; NeighborCell++) {   // Перебор соседних клеток поля (ячеек)
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // Индекс строки соседней клетки.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // Индекс столбца соседней клетка.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // Если клетка за пределами поля, то не проверяем.
					if (VisibleShipsField[tmp_i][tmp_j] != 0 && *FleetAlive[VisibleShipsField[tmp_i][tmp_j] - 1] == false &&
						ReportKill == true) { //  Если в соседней клетке находится 'Убитый' корабль и надо сообщать об этом
						CellsForShot[i][j] = 2;  // Отмечаем цифрой 2 ячейку, рядом с которой (с любой стороны от ячейки) есть 'Убитый' корабль.
						break;
					}
				}
			}
			else if (TouchShip == 1) { // Корабли могут касаться друг друга углами. Отметим ячейки, у которых хотя бы с одной из 
					                       // 4-х сторон (сверху, снизу, справа или слева) есть корабль
				int tmp_i{ 0 }, tmp_j{ 0 };
				// Проверим на наличие кораблей сверху-снизу-справа-слева
				for (int NeighborCell = 1; NeighborCell < 8; NeighborCell += 2) {
					tmp_i = i + CoordNeighborCells[NeighborCell][0]; // Номер строки соседней ячейки.
					tmp_j = j + CoordNeighborCells[NeighborCell][1]; // Номер столбца соседней ячейки.
					if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns - 1) continue; // Ячейки за пределами поля не проверяем.
					if (VisibleShipsField[tmp_i][tmp_j] != 0 && *FleetAlive[VisibleShipsField[tmp_i][tmp_j] - 1] == false &&
						ReportKill == true) {  // Если в соседней ячейке (сверху-снизу-справа-слева) находится 'Убитый' корабль
						                        // и надо сообщать об этом
						CellsForShot[i][j] = 2;  // Отмечаем цифрой 2 ячейку, рядом с которой (сверху-снизу-справа-слева) есть 'Убитый' корабль.
						break;
					}
				}
			}
		}
	}
	// Определяем количество 'свободных' для выстрела ячеек
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (CellsForShot[i][j] == 0) nFree++;
		}
	}
	return nFree;
}
//
bool PlaceShips(int nLines, int nColumns, int **ShipsField, int **Shot, int *ArrShipSize, int ***ArrShape, int ***FleetPosition, 
	int nShips, int ShipNumber, int TouchShip, bool Auto, bool **FleetAlive, bool ReportKill) // Рекурсивная функция. Размещает корабли на поле. Возвращает false если разместить не удалось. 
{
	bool ReturnValue{ false }; // Возвращаемое функцией значение.   
	// Создаём массив TakenCellsField[][] с данными о занятых ячейках (0-свободная ячейка, 1-занята кораблём, 2-занята из-за корабля по соседству):
	int **TakenCellsField = new int*[nLines]; 
	for (int i = 0; i < nLines; i++) {   // Цикл по строкам.
		TakenCellsField[i] = new int[nColumns];
		for (int j = 0; j < nColumns; j++) TakenCellsField[i][j] = 0; // Инициализируем нулями
	}
	if (FindTakenCells(TakenCellsField, nLines, nColumns, ShipsField, TouchShip) == 0) { // Записываем в TakenCellsField[][] данные
		      // о занятых ячейках и проверяем наличие свободных ячеек. Если нет свободных ячеек - возвращаем false.
		cout << "\nНет свободных ячеек." << endl;
		return false;
	}
	// Размещаем новый корабль
	if (Auto) {   // Размещаем корабль автоматически
		if (!PlaceOneShipAutomatically(nLines, nColumns, ShipsField, Shot, TakenCellsField, ArrShipSize, ArrShape, FleetPosition,
			ShipNumber, FleetAlive, ReportKill)) ReturnValue = false;  // Если не удалось автоматически разместить корабль.
		else {
			if (ShipNumber >= nShips) ReturnValue = true; // После успешного размещения последнего корабля (условие выхода из рекурсии).
			else if (PlaceShips(nLines, nColumns, ShipsField, Shot, ArrShipSize, ArrShape, FleetPosition, nShips,
				ShipNumber + 1, TouchShip, Auto, FleetAlive, ReportKill)) ReturnValue = true; // Размещаем следующий корабль - рекурсивный вызов этой-же функции. 
			else ReturnValue = false;    // Если не удалось разместить следующий корабль
		} 
	}
	else {    // Размещаем корабль вручную. 
		if (!PlaceOneShipByHand(nLines, nColumns, ShipsField, Shot, TakenCellsField, ArrShipSize, ArrShape, FleetPosition,
			ShipNumber, FleetAlive, ReportKill)) ReturnValue= false;  // Если не удалось разместить корабль вручную.
		else {
			if (ShipNumber >= nShips) ReturnValue = true; // После успешного размещения последнего корабля (условие выхода из рекурсии).
			else if (PlaceShips(nLines, nColumns, ShipsField, Shot, ArrShipSize, ArrShape, FleetPosition, nShips, ShipNumber + 1,
				 TouchShip, Auto, FleetAlive, ReportKill)) ReturnValue = true; // Размещаем следующий корабль - рекурсивный вызов этой-же функции.. 
			else ReturnValue = false;    // Если не удалось разместить следующий корабль
		}
	}
	// Очищаем динамическую память
	for (int i = 0; i < nLines; i++) delete[]TakenCellsField[i];
	delete[]TakenCellsField;
	//  
	return ReturnValue;  // Выход из функции PlaceShips()
}
//
bool PlaceOneShipByHand(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape, 
	int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill) // Размещение кораблей на поле вручную. Возвращает false если разместить не удалось.
{
	int StartLine{ 0 }, StartColumn{ 0 };  // Номер (не индекс) строки и столбца первой ('стартовой') ячейки корабля.
	int Direction{ 0 }; // Направление корабля от первой ('стартовой') клетки. (1-вверх; 2-вправо; 3-вниз; 4-влево).
	system("cls");
	cout << "\n                     ------------------------------------ " << endl;
	cout << "                         Расстановка кораблей на поле:" << endl;
	cout << "                     ------------------------------------ " << endl;
	cout << "\n\n\n" << endl;
	PrintField(nLines, nColumns, ShipsField, Shot, TakenCellsField, FleetAlive, ReportKill);
	cout << "\n\n";
	cout << "Разместите корабль №" << ShipNumber << ".   Размер: " << ArrShipSize[ShipNumber - 1] << " клетки." << endl;
	while (true) {
		while (true) {
			cout << "Введите положение начала корабля (например: \'д9\') или 0 для выхода: ";
			if (!getCellCoordinate(&StartLine, &StartColumn, nLines, nColumns)) return false; // В StartLine и StartColumn записали место старта - номер (не индекс) строки и столбца. 
			if (TakenCellsField[StartLine - 1][StartColumn - 1] == 0) break;
			else if (TakenCellsField[StartLine - 1][StartColumn - 1] == 1) cout << "Ошибка!!!  Ячейка занята кораблём." << endl;
			else if (TakenCellsField[StartLine - 1][StartColumn - 1] == 2) cout << "Ошибка!!! Ячейка находится рядом с другим кораблём." << endl;
		}
		if (ArrShipSize[ShipNumber - 1] == 1) Direction = 1;  // Для корабля размером 1 
		else {
			cout << "Укажите направление корабля (1-вверх; 2-вправо; 3-вниз; 4-влево): ";
			Direction = getInt_WithValueCheck(1, 4);
		}
		if (!PlaceShipInDirection(nLines, nColumns, ShipsField, TakenCellsField, ArrShipSize, ArrShape, FleetPosition, ShipNumber,
			StartLine - 1, StartColumn - 1, Direction)) cout << "Не удалось разместить корабль. Задайте другие координаты." << endl;
		else break;
	}
	return true;  // Выход из функции PlaceOneShipByHand()
}
//
bool PlaceOneShipAutomatically(int nLines, int nColumns, int **ShipsField, int **Shot, int **TakenCellsField, int *ArrShipSize, int ***ArrShape,
	 int ***FleetPosition, int ShipNumber, bool **FleetAlive, bool ReportKill) // Автоматическое размещение кораблей на поле. Возвращает false если разместить не удалось.
{
	int nFree{ 0 }; // Количество свободных ячеек на поле. 
	int RandomCellNumber{ 0 }; // Номер случайной ячейки (от 1 до nFree включительно).
	int i_start{ 0 }, j_start{ 0 }; // Индексы строки и столбца ячейки старта.
	bool ReturnValue{ false }; // Возвращаемое функцией значение.
	int DirectionArr[] = { 1,2,3,4 }; // Перечень направлений установки корабля. Далее будет перемешан в случайном порядке.
	int DirectionIndex{ 0 }, tmp{ 0 };
	// Создаём массив с информацией о ячейках, подходящих для старта размещения нового корабля ShipNumber
	int **StartCells = new int*[nLines];
	for (int i = 0; i < nLines; i++) {
		StartCells[i] = new int[nColumns];
		for (int j = 0; j < nColumns; j++) {
			StartCells[i][j] = TakenCellsField[i][j]; // Сначала инициализируем занятыми ячейками массив со стартовыми ячейками.
		}
	}
	// Перебираем в случайном порядке свободные ячейки и пытаемся поставить корабль, стартуя с этой ячейки. 
	while (true) {
		ReturnValue = false;
		// Определяем количество nFree свободных для старта ячеек 
		for (int i = 0; i < nLines; i++) {
			for (int j = 0; j < nColumns; j++) {
				if (StartCells[i][j] == 0) nFree++;
			}
		}
		if (nFree <= 0) break; // Если нет свободных ячеек - выход.
		RandomCellNumber = 1 + rand() % nFree;  // Номер случайной ячейки среди свободных ячеек (от 1 до nFree включительно).
		// Определяем индексы строки и столбца для случайной ячейки
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
		// Перемешиваем направления в массиве DirectionArr[] в случайном порядке
		for (int i = 0; i < 3; i++) {
			DirectionIndex = i + rand() % (4 - i);
			tmp = DirectionArr[i];
			DirectionArr[i] = DirectionArr[DirectionIndex];
			DirectionArr[DirectionIndex] = tmp;
		}
		// Устанавливаем корабль, стартуя с ячейки с индексом [i_start][j_start] перебирая по очереди направления из DirectionArr[]
		for (int i = 0; i < 4; i++) {
			if (PlaceShipInDirection(nLines, nColumns, ShipsField, StartCells, ArrShipSize, ArrShape, FleetPosition, ShipNumber,
				i_start, j_start, DirectionArr[i])) {   // Если корабль размещён успешно
				ReturnValue = true;
				break;
			}
		}
		if (ReturnValue == true) break;  // Если корабль размещён успешно - выходим из цикла while()
		else {   // Если корабль не был размещён, то эту стартовую ячейку отмечаем занятой
			StartCells[i_start][j_start] = 1;
		}
	}
	if (!ReturnValue) {   // Если не удалось разместить корабль автоматически, то печатаем последнее состояние игрового поля.
		system("cls");  
		cout << "\n\n\n" << endl;
		PrintField(nLines, nColumns, ShipsField, Shot, TakenCellsField, FleetAlive, ReportKill);
	}
	// Очищаем динамическую память
	for (int i = 0; i < nLines; i++) delete[]StartCells[i];
	delete[]StartCells;
	//
	return ReturnValue; // Выход из функции PlaceOneShipAutomatically()
}
//
bool PlaceShipInDirection(int nLines, int nColumns, int **ShipsField, int **TakenCellsField, int *ArrShipSize, int ***ArrShape, 
	int ***FleetPosition, int ShipNumber, int StartLine_i, int StartColumn_j, int Direction)  // Размещает корабль в направлении Direction
	// начиная с ячейки с индексами [StartLine_i][StartColumn_j]. В массиве ShipsField[][] в клетки, занятые кораблём, записываем номер
	// корабля ShipNumber. В массиве Fleet[ShipNumber-1]->position[][2] записываем индексы клеток корабля [i_Строки][j_Столбца] на игровом поле.
	// Возвращает false если разместить корабль не получилось.
{
	int tmp_i{ 0 }, tmp_j{ 0 };  // Индексы ячейки: tmp_i - индекс строки, tmp_j - индекс столбца.
	// Проверяем, свободны ли ячейки для корабля:
	for (int i = 0; i < ArrShipSize[ShipNumber - 1]; i++) {
		if (Direction == 1) {  // расчёт индекса ячейки для направления 'ВВЕРХ'
			tmp_i = StartLine_i - ArrShape[ShipNumber - 1][i][0]; // Индекс строки для проверяемой ячейки.
			tmp_j = StartColumn_j;  // Индекс столбца для проверяемой ячейки.
		}
		else if (Direction == 2) {  // расчёт индекса ячейки для направления 'ВПРАВО'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j + ArrShape[ShipNumber - 1][i][0];
		}
		else if (Direction == 3) {  // расчёт индекса ячейки для направления 'ВНИЗ'
			tmp_i = StartLine_i + ArrShape[ShipNumber - 1][i][0];
			tmp_j = StartColumn_j;
		}
		else if (Direction == 4) {  // расчёт индекса ячейки для направления 'ВПРАВО'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j - ArrShape[ShipNumber - 1][i][0];
		}
		else {
			cout << "Ошибка!!! Значение переменной Direction вне диапазона." << endl;
			return false;
		}
		// Проверяем ячейку с полученными индексами [tmp_i][tmp_j] на возможность поставить туда корабль
		if (tmp_i<0 || tmp_i>nLines - 1 || tmp_j<0 || tmp_j>nColumns) {  // Если вышли за пределы поля
			return false;
		}
		else if (TakenCellsField[tmp_i][tmp_j] != 0) {  // Если попали на 'занятую' ячейку.
			return false;
		}
	}
	// Если место для корабля свободно - располагаем там корабль
	for (int i = 0; i < ArrShipSize[ShipNumber - 1]; i++) {
		// Определяем индексы строки и столбца для очередной клетки корабля
		if (Direction == 1) {  // расчёт индекса ячейки для направления 'ВВЕРХ'
			tmp_i = StartLine_i - ArrShape[ShipNumber - 1][i][0]; // Индекс строки для проверяемой ячейки.
			tmp_j = StartColumn_j;  // Индекс столбца для проверяемой ячейки.
		}
		else if (Direction == 2) {  // расчёт индекса ячейки для направления 'ВПРАВО'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j + ArrShape[ShipNumber - 1][i][0];
		}
		else if (Direction == 3) {  // расчёт индекса ячейки для направления 'ВНИЗ'
			tmp_i = StartLine_i + ArrShape[ShipNumber - 1][i][0];
			tmp_j = StartColumn_j;
		}
		else if (Direction == 4) {  // расчёт индекса ячейки для направления 'ВПРАВО'
			tmp_i = StartLine_i;
			tmp_j = StartColumn_j - ArrShape[ShipNumber - 1][i][0];
		}
		// Заносим эту клетку корабля в массивы
		ShipsField[tmp_i][tmp_j] = ShipNumber; // В ячейку на поле, занятую кораблём, записываем номер (не индекс) корабля.
		FleetPosition[ShipNumber - 1][i][0] = tmp_i + 1; //  В коодинаты корабля записываем 'номер строки' (не индекс) данной ячейки. 
		FleetPosition[ShipNumber - 1][i][1] = tmp_j+1;  //  В коодинаты корабля записываем 'номер столбца' (не индекс) данной ячейки. 
	}
	return true; // Выход из функции PlaceShipInDirection()
}
//
bool getAutoCellForShot(int*Line, int*Column, int nLines, int nColumns, int **CellsForShot,  int LastShot_Line,
	int LastShot_Column)   // Автоматически определяет координаты ячейки куда будет сделан выстрел. Номер строки и столбца (не индексы) 
	  // записывает в Line и Column. Возвращает true в случае успеха, false - если не удалось найти свободную ячейку для выстрела.
{
	int nFree{ 0 }; // Количество свободных для выстрела ячеек 
	int RandomCellNumber{ 0 }; // Номер случайной ячейки (от 1 до nFree включительно).
	bool ReturnValue{ false }; // Возвращаемое функцией значение.
	// Определяем количество свободных для выстрела ячеек 
	for (int i = 0; i < nLines; i++) {
		for (int j = 0; j < nColumns; j++) {
			if (CellsForShot[i][j] == 0) nFree++;
		}
	}
	if (nFree <= 0) return false; // Если нет свободных ячеек - выход.
	RandomCellNumber = 1 + rand() % nFree;  // Номер случайной ячейки среди свободных ячеек (от 1 до nFree включительно).
	// Определяем номер строки и столбца (не индексы) для выбранной случайной ячейки
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
	return true; // Выход из функции getAutoCellForShot()
}
//
inline void PrintSpaces(int n_spaces)  // печатает строку, состоящую из заданного количества пробелов
{
	for (int i = 0; i < n_spaces; i++) cout << " ";
}
//
void PauseWithStars() // Анимированная пауза - печатает строку из исчезающих звёздочек * 
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