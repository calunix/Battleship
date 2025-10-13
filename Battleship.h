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

	static GridLocation ParseGridLocation(string);

	friend class Fleet;

private:
	char** board_;
	
	const int  ROWS                { 10 };
	const int  COLS                { 10 };
	const int  NUM_SHIPS           { 5 };
	const int  MAX_CONSEC_MISSES   { 15 };
	static const int ASCII_LOWER_A { 97 };
	const string QUIT_SENTINEL     { "q" };

	static const char DEFAULT_GRID_CHAR { '*' };
	static const char HIT_SYM           { 'X' };
	static const char MISS_SYM          { 'O' };

	static const char FRIGATE_SYM       { 'F' };
	static const char BATTLESHIP_SYM    { 'B' };
	static const char DESTROYER_SYM     { 'D' };
	static const char SUBMARINE_SYM     { 'S' };
	static const char CARRIER_SYM       { 'C' };

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
	bool music_on_  { false };
	bool sound_fx_  { false };
	bool dark_mode_ { false };

	int  ships_sunk_         { };
	int  consecutive_misses_ { };

	Position* generatePosition(int);
	bool      validatePosition(Position*, int);
	void      PlaceShip(Ship, Position*);
    void      SetupShip(Ship);
	void      ToggleDevMode(void);
};

