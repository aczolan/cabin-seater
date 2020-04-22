// util.h

#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include <string>
#include <iostream>
#include <fstream>

bool appendLineToFile(std::string filepath, std::string line)
{
    std::ofstream file(filepath, std::ios::out | std::ios::app);
    file << line;
    return true;
}

bool isEven(int x)
{
	return ( (x % 2 == 0) || (x == 0) );
}

#endif
