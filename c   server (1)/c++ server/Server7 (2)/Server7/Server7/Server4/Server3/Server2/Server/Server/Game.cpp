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

game::game()
{
	type = "";
	map = "";
	
}
void game::setType(string type_)
{
	type = type_;
}
void game::setMap(string map_)
{
	map = map_;
}
void game::setLevel(int level_)
{
	level = level_;
}
void game::setMax(int max_)
{
	max = max_;
}
void game::setMin(int min_)
{
	min = min_;
}
string game::getType()
{
	return type;
}
string game::getMap()
{
	return map;
}
int game::getLevel()
{
	return level;
}
int game::getMax()
{
	return max;
}
int game::getMin()
{
	return min;
}