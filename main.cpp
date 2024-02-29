/* CS307 PROJECT: EASYDRAFT */
// insta segfault on running
#ifndef MAIN_CPP
#define MAIN_CPP

#include "TemplateParser.cpp"
int main() {
    TemplateParser tp{};
    tp.parse("stat511.docx");
    return 0;
}
#endif


