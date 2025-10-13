#pragma once

class Ship
{
public:
	Ship(int len, char symbol) {
		length_ = len;
		grid_symbol_ = symbol;
	}
	void Strike(void) {
		hits_ += 1;
		if (hits_ == length_) sunk_ = true;
	}
	int Length(void) { return length_; }
	bool Sunk(void) { return sunk_; }
	char GridSymbol(void) { return grid_symbol_; }
	void ResetShipState(void) {
		hits_ = 0;
		sunk_ = false;
	}
	
private:
	char grid_symbol_;
	int  length_;
	int  hits_{ };
	bool sunk_{ false };
};
