#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>

#include <windows.h>

#include "Battleship.h"
#include "Ship.h"

using namespace std;

int main(void)
{
	// set console size
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD newBufferSize = { 100, 100 }; // Example: 120 columns, 40 rows
	SMALL_RECT windowRect = { 0, 0, newBufferSize.X - 1, newBufferSize.Y - 1 };
	SetConsoleScreenBufferSize(hConsole, newBufferSize);
	SetConsoleWindowInfo(hConsole, TRUE, &windowRect);

	//HWND consoleWindow = GetConsoleWindow();
	//if (consoleWindow != NULL) {
	//	// Example: Move to (100, 100) and resize to 800x600 pixels
	//	SetWindowPos(consoleWindow, NULL, 100, 100, 800, 600, SWP_NOZORDER | SWP_SHOWWINDOW);
	//}
	
	Battleship battleship{ };
	string user_input{ };
	GridLocation next_shot{ };

	battleship.DrawBoard();

	auto PrintErrorMessage = []() {
		cout << "Invalid input...\n";
	};
	auto PrintUserPrompt = []() {
		cout << "Enter a grid location or control option: ";
	};
	auto ToLower = [](const string& input_string) {
		string output{ input_string };
		transform(output.begin(), output.end(), output.begin(),
			[](unsigned char c) { return tolower(c); });
		return output;
	};

	PrintUserPrompt();
	while (getline(cin, user_input))
	{
		user_input = ToLower(user_input);
		if (!battleship.ValidateUserInput(user_input)) {
			PrintErrorMessage();
			PrintUserPrompt();
			continue;
		}

		if (user_input.length() == 1) {
			battleship.ProcessCommandOption(user_input);
			battleship.DrawBoard();
			PrintUserPrompt();
			continue;
		}

		next_shot = Battleship::ParseGridLocation(user_input);
		battleship.TakeShot(next_shot);

		if (battleship.GameOver()) {
			cout << "Play again? (y/n): ";
			while (getline(cin, user_input)) {
				user_input = ToLower(user_input);
				if (user_input == "y" || user_input == "yes" ) {
					battleship.ResetGame();
					break;
				}
				else if (user_input == "n" || user_input == "no") {
					return 0;
				}
				cout << "Play again? (y/n): ";
			}
		}

		battleship.DrawBoard();
		PrintUserPrompt();
	}
	
	return 0;
}