/* CS307 PROJECT: EASYDRAFT */
#ifndef MAIN_CPP
#define MAIN_CPP

#include "config.hpp"
#include "file_operations.hpp"
#include "TemplateParser.hpp"
#include "recycle_bin.hpp"

int main() {
    std::cout << move_template_to_trash("example");
    return 0;
    
}
#endif


