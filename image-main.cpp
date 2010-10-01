#include "image.hpp"
#include <string>
#include <iostream>

extern void eval(string expr, Image & im);
extern void parse_command_line(int argc, char *argv[], Image & im);

int main (int argc, char **argv)
{
    Image im;
    string buf;
    int i;
    for (i=0; i<NUM_FEATURES; ++i) feature_vector[i] = 0.0;
    parse_command_line(argc, argv, im);

    while (std::cin >> buf && buf != "exit" && buf != "q" && buf != "quit") {
        eval(buf, im);
    }
std::cout << endl;
    return 0;
}
