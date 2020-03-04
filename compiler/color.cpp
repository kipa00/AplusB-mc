#include "color.hpp"

int color_to_int(string s) {
	if (s == "white") return 0;
	else if (s == "orange") return 1;
	else if (s == "magenta") return 2;
	else if (s == "light_blue") return 3;
	else if (s == "yellow") return 4;
	else if (s == "lime") return 5;
	else if (s == "pink") return 6;
	else if (s == "gray") return 7;
	else if (s == "light_gray") return 8;
	else if (s == "cyan") return 9;
	else if (s == "purple") return 10;
	else if (s == "blue") return 11;
	else if (s == "brown") return 12;
	else if (s == "green") return 13;
	else if (s == "red") return 14;
	else if (s == "black") return 15;
	return -1;
}
