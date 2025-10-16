#include <string>
#include <iostream>
#include <iomanip>
#include <random>
#include <cctype>
#include <chrono>
#include <thread>

#include <windows.h>

#include "Battleship.h"

using namespace std;

Battleship::Battleship()
{
	board_ = new char* [ROWS];
	for (int i{ }; i < ROWS; i++) board_[i] = new char[COLS];
	for (int i{ }; i < ROWS; i++) {
		for (int j{ }; j < COLS; j++) {
			board_[i][j] = DEFAULT_GRID_CHAR;
		}
	}
	for (const auto& ship : fleet_) SetupShip(*ship.second);
}

void Battleship::ResetGame(void)
{
	cout << "\nSTARTING NEW GAME!\n\n";
	this_thread::sleep_for(chrono::seconds(2));
	for (int i{ }; i < ROWS; i++) {
		for (int j{ }; j < COLS; j++) {
			board_[i][j] = DEFAULT_GRID_CHAR;
		}
	}
	for (auto& ship : fleet_) {
		(*ship.second).ResetShipState();
		SetupShip(*ship.second);
	}
	consecutive_misses_ = 0;
	ships_sunk_ = 0;
}

void Battleship::TakeShot(GridLocation gl)
{
	if (sound_fx_) {
		PlaySound(TEXT("audio/launch.wav"), NULL, SND_FILENAME | SND_ASYNC);
		this_thread::sleep_for(chrono::seconds(2));
	}

	char grid_occupant{ board_[gl.row][gl.column] };
	if (grid_occupant == DEFAULT_GRID_CHAR) {
		board_[gl.row][gl.column] = MISS_SYM;
		consecutive_misses_ += 1;
		if (sound_fx_) PlaySound(TEXT("audio/miss.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cout << "MISS!\n";
		this_thread::sleep_for(chrono::seconds(2));
	}
	else if (grid_occupant == MISS_SYM) {
		consecutive_misses_ += 1;
		if (sound_fx_) PlaySound(TEXT("audio/miss.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cout << "MISS! Don't waste your missiles!\n";
		this_thread::sleep_for(chrono::seconds(2));
	}
	else if (grid_occupant == HIT_SYM) {
		consecutive_misses_ += 1;
		if (sound_fx_) PlaySound(TEXT("audio/miss.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cout << "You've already hit this coordinate! Don't waste your missiles!\n";
		this_thread::sleep_for(chrono::seconds(2));
	}
	else if (grid_occupant != DEFAULT_GRID_CHAR) {
		cout << "HIT!\n";
		if (sound_fx_) PlaySound(TEXT("audio/hit.wav"), NULL, SND_FILENAME | SND_ASYNC);
		string ship_type = ship_map_[grid_occupant];
		fleet_[ship_type]->Strike();
		if (fleet_[ship_type]->Sunk()) {
			cout << "You sank the enemy " << ship_type << "!\n\n";
			ships_sunk_ += 1;
		}

		board_[gl.row][gl.column] = HIT_SYM;
		consecutive_misses_ = 0;
		
		this_thread::sleep_for(chrono::seconds(2));
	}
}

GridLocation Battleship::ParseGridLocation(string input)
{
	GridLocation target{ };
	target.row = ( (int)input[0] ) - ASCII_LOWER_A;

	string column{ };
	for (int i{ 1 }; i < input.length(); i++) {
		column += input[i];
	}
	target.column = stoi(column) - 1;

	return target;
}

bool Battleship::ValidateUserInput(string input)
{
	int column_parsed{ };
	string column{ };

	switch (input.length()) {
	case 1:
		if (input == "n" ||
			input == "q" ||
			input == "l" ||
			input == "d" ||
			input == "s" ) {
			return true;
		}
	case 2:
	case 3:
		if ((char)input[0] < 'a' || (char)input[0] > 'j') {
			return false;
		}
		for (int i{ 1 }; i < input.length(); i++) {
			column += input[i];
		}
		try {
			column_parsed = stoi(column);
		}
		catch (...) {
			return false;
		}
		if (column_parsed >= 1 && column_parsed <= 10) {
			return true;
		}
	default:
		return false;
	}
}

bool Battleship::GameOver(void)
{
	if (ships_sunk_ == NUM_SHIPS) {
		DrawBoard();
		cout << "GAME OVER! All enemy ships destroyed! YOU WIN!\n";
		return true;
	}
	if (consecutive_misses_ == MAX_CONSEC_MISSES) {
		DrawBoard();
		cout << "GAME OVER! The enemy has discovered your position and targeted your ship! You have lost the battle!\n";
		return true;
	}
	return false;
}

void Battleship::SetupShip(Ship ship)
{
	bool valid_position{ false };
	while (!valid_position)
	{
		Position* ship_position{ GeneratePosition(ship.Length()) };
		valid_position = ValidatePosition(ship_position, ship.Length());
		if (!valid_position) delete ship_position;
		else PlaceShip(ship, ship_position);
	}
}

void Battleship::PlaceShip(Ship ship, Position* pos)
{
	if (!pos->orientation) { // vertical
		for (int i{ pos->row }; i < pos->row + ship.Length(); i++) {
			board_[i][pos->column] = ship.GridSymbol();
		}
	}
	else { // horizontal
		for (int i{ pos->column }; i < pos->column + ship.Length(); i++) {
			board_[pos->row][i] = ship.GridSymbol();
		}
	}
}

bool Battleship::ValidatePosition(Position* pos, int ship_length)
{
	if (!pos->orientation) { // vertical
		if (pos->row + ship_length > ROWS) return false;
		for (int i{ pos->row }; i < pos->row + ship_length; i++) {
			if (board_[i][pos->column] != DEFAULT_GRID_CHAR) return false;
		}
	}
	else { // horizontal
		if (pos->column + ship_length > COLS) return false;
		for (int i{ pos->column }; i < pos->column + ship_length; i++) {
			if (board_[pos->row][i] != DEFAULT_GRID_CHAR) return false;
		}
	}
	return true;
}

Position* Battleship::GeneratePosition(int ship_length)
{
	Position* p = new Position;
	std::random_device rdev;
	std::mt19937 gen(rdev());

	// orientation is 0 or 1
	std::uniform_int_distribution<> orientation_distribution(0, 1);
	p->orientation = orientation_distribution(gen);

	int row_pos{ };
	int col_pos{ };
	if (p->orientation == 0) { // vertical
		std::uniform_int_distribution<> rows_distribution(0, ROWS - ship_length + 1);
		std::uniform_int_distribution<> cols_distribution(0, COLS - 1);
		row_pos = rows_distribution(gen);
		col_pos = cols_distribution(gen);
	}
	else { // horizontal
		std::uniform_int_distribution<> rows_distribution(0, ROWS - 1);
		std::uniform_int_distribution<>
			cols_distribution(0, COLS - ship_length + 1);
		row_pos = rows_distribution(gen);
		col_pos = cols_distribution(gen);
	}
	p->row = row_pos;
	p->column = col_pos;
	return p;
}

void Battleship::DrawBoard()
{
	if (!dev_mode_) system("cls");

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, text_color_);
	
	std::cout << R"(
                                      |__
                                      |\/
                                      ---
                                      / | [
                               !      | |||
                             _/|     _/|-++'
                         +  +--|    |--|--|_ |-
                      { /|__|  |/\__|  |--- |||__/
                     +---------------___[}-_===_.'____                 /\
                 ____`-' ||___-{]_| _[}-  |     |_[___\==--            \/   _
  __..._____--==/___]_|__|_____________________________[___\==--____,------' .7
 |                                                                     BB-61/
  \_________________________________________________________________________|

              ____  ___  ______________    ___________ __  __________ 
             / __ )/   |/_  __/_  __/ /   / ____/ ___// / / /  _/ __ \
            / __  / /| | / /   / / / /   / __/  \__ \/ /_/ // // /_/ /
           / /_/ / ___ |/ /   / / / /___/ /___ ___/ / __  // // ____/ 
          /_____/_/  |_/_/   /_/ /_____/_____//____/_/ /_/___/_/      
    )" << "\n";
	
	int left_padding{ 8 };
	cout << setfill(' ');
	
	std::cout << std::setw(left_padding + 2);
	for (int i{ 1 }; i <= COLS; i++) std::cout << " " << i << " ";
	std::cout << '\n';
	std::cout << std::setw(left_padding + 2);
	for (int i{ 0 }; i < COLS * 3; i++) std::cout << "-";
	std::cout << '\n';
	
	char rowLabel{ 'A' };
	for (int i{ }; i < ROWS; i++)
	{
		std::cout << std::setw(left_padding - 1) << rowLabel << " |";
		rowLabel++;

		for (int j{ }; j < COLS; j++)
		{
			if (board_[i][j] == HIT_SYM) {
				SetConsoleTextAttribute(hConsole, hit_color_);
				std::cout << " " << board_[i][j] << " ";
				SetConsoleTextAttribute(hConsole, text_color_);
			}
			else if (board_[i][j] == MISS_SYM) {
				SetConsoleTextAttribute(hConsole, miss_color_);
				std::cout << " " << board_[i][j] << " ";
				SetConsoleTextAttribute(hConsole, text_color_);
			}
			else if (board_[i][j] != DEFAULT_GRID_CHAR && dev_mode_) {
				SetConsoleTextAttribute(hConsole, ship_color_);
				std::cout << " " << board_[i][j] << " ";
				SetConsoleTextAttribute(hConsole, text_color_);
			}
			else {
				std::cout << " " << DEFAULT_GRID_CHAR << " ";
			}
		}

		if (i == 0) {
			cout << setw(left_padding) << "     Control Options:";
		}
		else if (i == 1) {
			cout << "     ================";
		}
		else if (i == 2) {
			cout << "     N - new game";
		}
		else if (i == 3) {
			cout << "     Q - quit";
		}
		else if (i == 4) {
			cout << "     L - toggle dark/light mode";
		}
		else if (i == 5) {
			cout << "     D - toggle development mode";
		}
		else if (i == 6) {
			cout << "     S - toggle sound effects on/off";
		}
		else if (i == 8) {
			cout << "     Misses: " << consecutive_misses_;
		}

		cout << "\n";
	}

	string settings{ };

	if (dev_mode_) settings += "            Dev Mode: ON  |";
	else settings += "            Dev Mode: OFF  |";

	if (dark_mode_) settings += "  Dark Mode: ON  |";
	else settings += "  Dark Mode: OFF  |";

	if (sound_fx_) settings += "  Sound Effects: ON";
	else settings += "  Sound Effects: OFF";

	cout << "\n";
	cout << setw(DISPLAY_WIDTH) << setfill('=') << "\n";
	cout << settings << "\n";
	cout << setw(DISPLAY_WIDTH) << setfill('=') << "\n";

	string ship_states{ };

	ship_states += " Carrier: ";
	if (fleet_[CARRIER_STR]->Sunk()) ship_states += "X  |";
	else ship_states += "   |";

	ship_states += "  Battleship: ";
	if (fleet_[BATTLESHIP_STR]->Sunk()) ship_states += "X  |";
	else ship_states += "   |";

	ship_states += "  Frigate: ";
	if (fleet_[FRIGATE_STR]->Sunk()) ship_states += "X  |";
	else ship_states += "   |";

	ship_states += "  Submarine: ";
	if (fleet_[SUBMARINE_STR]->Sunk()) ship_states += "X  |";
	else ship_states += "   |";

	ship_states += "  Destroyer: ";
	if (fleet_[DESTROYER_STR]->Sunk()) ship_states += "X";

	cout << ship_states << "\n";
	cout << setw(DISPLAY_WIDTH) << setfill('=') << "\n";

	cout << endl;
}

void Battleship::ProcessCommandOption(string option)
{
	if (option == QUIT_SENTINEL) {
		PrintCredits();
		exit(0);
	}
	else if (option == "d") {
		ToggleDevMode();
	}
	else if (option == "n") {
		ResetGame();
	}
	else if (option == "l") {
		ToggleColorScheme();
	}
	else if (option == "s") {
		ToggleSoundFx();
	}
}

void Battleship::ToggleDevMode(void)
{
	if (dev_mode_) {
		cout << "\nDEVELOPMENT MODE OFF\n\n";
	}
	else {
		cout << "\nDEVELOPMENT MODE ON\n\n";
	}
	dev_mode_ = !dev_mode_;
	this_thread::sleep_for(chrono::seconds(2));
}

void Battleship::ToggleColorScheme(void)
{
	dark_mode_ = !dark_mode_;
	// right most hex digit represents foreground color
	// left most hex digit represents background color
	// 0 - black, 4 - red, 9 - blue, a - green, f - white
	if (dark_mode_) {
		cout << "\nSWITCHING TO DARK MODE!";
		text_color_ = 0x0f;
		hit_color_ = 0x04 | FOREGROUND_INTENSITY;
		miss_color_ = 0x09 | FOREGROUND_INTENSITY;
		ship_color_ = 0x0a | FOREGROUND_INTENSITY;
	}
	else {
		cout << "\nSWITCHING TO LIGHT MODE!";
		text_color_ = 0xf0;
		hit_color_ = 0xf4 | FOREGROUND_INTENSITY;
		miss_color_ = 0xf9 | FOREGROUND_INTENSITY;
		ship_color_ = 0xfa | FOREGROUND_INTENSITY;
	}
	this_thread::sleep_for(chrono::seconds(2));
	DrawBoard(); // redraw required to clear previous background color
}

void Battleship::ToggleSoundFx(void)
{
	if (!sound_fx_) {
		cout << "\nSOUND EFFECTS ON\n";
	}
	else {
		cout << "\nSOUND EFFECTS OFF\n";
	}
	sound_fx_ = !sound_fx_;
	this_thread::sleep_for(chrono::seconds(2));
}

void Battleship::PrintCredits(void)
{
	cout << "\n\n";
	PrintCenteredString("CREDITS");
	PrintCenteredString("=======");
	cout << "\n";
	PrintCenteredString("USS Iowa BB-61 ASCII Art");
	PrintCenteredString("https://asciiart.website/art/4378");
	cout << "\n";
	PrintCenteredString("\"Battleship\" ASCII Art");
	PrintCenteredString("https://patorjk.com/software/taag/");
	cout << "\n\n";
}

void Battleship::PrintCenteredString(string str)
{
	size_t length{ str.length() };
	size_t total_padding{ DISPLAY_WIDTH - length };
	size_t left_padding{ total_padding / 2 };
	cout << setfill(' ') << setw(left_padding + length) << str << "\n";
}