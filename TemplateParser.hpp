#ifndef EASYDRAFT_TEMPLATEPARSER_HPP
#define EASYDRAFT_TEMPLATEPARSER_HPP

//#include "extlibs/11Zip/include/elzip/elzip.hpp"
//#include "extlibs/11Zip/src/elzip.cpp"
//#include "zip_file.hpp"
#include <assert.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;
using recursive_directory_iterator = filesystem::recursive_directory_iterator;


void parse(const std::string);



#endif //EASYDRAFT_TEMPLATEPARSER_HPP
