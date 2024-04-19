/* CS307 PROJECT: EASYDRAFT */
#ifndef MAIN_CPP
#define MAIN_CPP

#include "config.hpp"
#include "file_operations.hpp"
#include "template_log.hpp"

int main() {
    start_log();
    start_template_log();
    add_template_use("temp_test15.docx");
    return 0;
}
#endif


