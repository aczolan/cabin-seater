// util.h

#include <time.h>

int randInt(int lower, int upper)
{
	srand(time(NULL));
	return rand() % upper + lower;
}