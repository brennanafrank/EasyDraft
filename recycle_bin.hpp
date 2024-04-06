#ifndef RECYCLE_BIN_HPP
#define RECYCLE_BIN_HPP

#include <iostream>
#include <cstdio>
#include "config.hpp"

fs::path TRASH_DIR = (fs::current_path().append("trash")).string();
int move_template_to_trash(std::string filename);
int restore_template(std::string filename);

#endif // RECYCLE_BIN_HPP