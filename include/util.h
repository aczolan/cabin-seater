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

    // file.open(filepath, std::ios::out | std::ios::app);
    // if (file.fail())
    // {
    //     return false;
    // }

    // file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);

    // file << line << std::endl;

    // return true;
}

#endif
