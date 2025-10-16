#pragma once
#include <string>
#include <unordered_map>

#include "Ship.h"

using namespace std;

struct Position
{
	char column;
	int  row;
	bool orientation;
};

struct GridLocation
{
	int row;
	int column;
};

class Battleship
{
public:
	Battleship(void);
	void DrawBoard(void);
	bool GameOver(void);
	void TakeShot(GridLocation);
	bool ValidateUserInput(string);
	void ProcessCommandOption(string);
	void ResetGame(void);
	void PrintCredits(void);

	static GridLocation ParseGridLocation(string);

private:
	char** board_;
	
	int hit_color_     { 0x04 | FOREGROUND_INTENSITY };
	int miss_color_    { 0x09 | FOREGROUND_INTENSITY };
	int ship_color_    { 0x0a | FOREGROUND_INTENSITY };
	int text_color_    { 0x0f };

	const int  ROWS                { 10 };
	const int  COLS                { 10 };
	const int  NUM_SHIPS           { 5 };
	const int  MAX_CONSEC_MISSES   { 15 };
	static const int ASCII_LOWER_A { 97 };
	const string QUIT_SENTINEL     { "q" };

	static const int  DISPLAY_WIDTH      { 80 };
	static const char DEFAULT_GRID_CHAR { '*' };
	static const char HIT_SYM           { 'X' };
	static const char MISS_SYM          { 'O' };

	static const char FRIGATE_SYM    { 'F' };
	static const char BATTLESHIP_SYM { 'B' };
	static const char DESTROYER_SYM  { 'D' };
	static const char SUBMARINE_SYM  { 'S' };
	static const char CARRIER_SYM    { 'C' };

	static const int FRIGATE_LEN    { 2 };
	static const int BATTLESHIP_LEN { 4 };
	static const int DESTROYER_LEN  { 3 };
	static const int SUBMARINE_LEN  { 3 };
	static const int CARRIER_LEN    { 5 };

	const string FRIGATE_STR    { "frigate"};
	const string BATTLESHIP_STR { "battleship" };
	const string DESTROYER_STR  { "destroyer" };
	const string SUBMARINE_STR  { "submarine" };
	const string CARRIER_STR    { "carrier" };

	unordered_map<string, Ship*> fleet_ {
		{ CARRIER_STR,   new Ship(CARRIER_LEN, CARRIER_SYM) },
		{ BATTLESHIP_STR, new Ship(BATTLESHIP_LEN, BATTLESHIP_SYM) },
		{ SUBMARINE_STR, new Ship(SUBMARINE_LEN, SUBMARINE_SYM) },
		{ FRIGATE_STR,   new Ship(FRIGATE_LEN, FRIGATE_SYM) },
		{ DESTROYER_STR, new Ship(DESTROYER_LEN, DESTROYER_SYM) }
	};
	unordered_map<char, string> ship_map_ {
		{ CARRIER_SYM, CARRIER_STR },
		{ BATTLESHIP_SYM, BATTLESHIP_STR},
		{ SUBMARINE_SYM, SUBMARINE_STR },
		{ FRIGATE_SYM, FRIGATE_STR },
		{ DESTROYER_SYM, DESTROYER_STR }
	};

	bool dev_mode_  { false };
	bool music_on_  { true };
	bool sound_fx_  { true };
	bool dark_mode_ { true };

	int  ships_sunk_         { };
	int  consecutive_misses_ { };

	Position* GeneratePosition(int);
	bool      ValidatePosition(Position*, int);
	void      PlaceShip(Ship, Position*);
    void      SetupShip(Ship);
	void      ToggleDevMode(void);
	void      ToggleColorScheme(void);
	void      ToggleSoundFx(void);
	void      PrintCenteredString(string);
};

