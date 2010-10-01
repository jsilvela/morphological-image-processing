#include "image.hpp"
#include <string>
#include <gtk/gtk.h>
#include <iostream>

extern void display (Image & im, const char *title);
extern void eval(string expr, Image & im);
extern void parse_command_line(int argc, char *argv[], Image & im);

int main (int argc, char *argv[])
{
    Image im;
    string buf;

    gtk_set_locale();
    gtk_init(&argc, &argv);

    parse_command_line(argc, argv, im);

    string title("temp");
    while (std::cin >> buf && buf != "exit" && buf != "q" && buf != "quit") {
        eval(buf, im);
        display(im, title.c_str());
    }

    return 0;
}
