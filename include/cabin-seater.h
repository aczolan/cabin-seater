// cabin-seater.h

#ifndef INCLUDE_CABIN_SEATER_H
#define INCLUDE_CABIN_SEATER_H

#include <math.h>
#include <map>
#include <vector>

struct StructUno
{
	double x;
	double y;

	bool operator=(const StructUno &o) const 
	{
		return (x == o.x) && (y == o.y);
	}

	bool operator<(const StructUno &o) const
	{
		return (x < o.x) || (y < o.y);
	}
};

enum EnumUno
{
	UNO,
	DOS,
	TRES
};

#endif //INCLUDE_CABIN_SEATER_H
