#pragma once
#include <string>
using namespace std;
class game {
private:
	string type;
	string map;
	int level;
	int max;
	int min;
public:
	game();
	void setType(string type_);
	void setMap(string map_);
	void setLevel(int level_);
	void setMax(int max_);
	void setMin(int min_);
	string getType();
	string getMap();
	int getLevel();
	int getMax();
	int getMin();
};