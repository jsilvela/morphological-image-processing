#include "image.hpp"
#include <string>
#include <iostream>
#include <time.h>
#include <numeric>
#include <stdlib.h>


void eval(string expr, Image & im)
{
    clock_t start_, finish_;
    start_ = clock();
    if (expr == "write") {
        char name[40];
std::cin >> name;
        im.write_TIFF(name);
    } else if (expr == "read") {
        char name[40];
std::cin >> name;
        im.read_TIFF(name);
    } else if (expr == "threshold") {
        int level;
std::cin >>  level;
        im.threshold(level);
    } else if (expr == "regions") {
        im.get_white_regions();
        im.get_black_regions();
    } else if (expr == "regions_bis") {
        im.get_regions ();
    } else if (expr == "white_regions") {
        im.erase_small_whites();
    } else if (expr == "black_regions") {
        im.get_black_regions ();
    } else if (expr == "skeleton") {
        im.get_skeleton();
    } else if (expr == "distance_transform") {
        im.distance_transform();
    } else if (expr == "euclid_transform") {
        im.euclid_transform();
    } else if (expr == "skel") {
        im.skel_transform();
    } else if (expr == "brute") {
        im.brute_distance_transform();
    } else if (expr == "euclid2") {
        im.euclid2_transform();
    } else if (expr == "verbose") {
        VERBOSE = true;
    } else if (expr == "silent") {
        VERBOSE = false;
    } else if (expr == "rectangle") {
        int h, w;
std::cin >> h >> w;
        im.rectangle(h, w);
    } else if (expr == "boundary") {
        im.get_boundary();
    } else if (expr == "contour") {
        im.get__contour(false);
    } else if (expr == "contour_bis") {
        im.get_contour(false);
    } else if (expr == "contour_outer") {
        im.get__contour(true);
    } else if (expr == "contour_bis_outer") {
        im.get_contour(true);
    } else if (expr == "erode") {
        int n;
std::cin >> n;
        im.erode(n);
    } else if (expr == "dilate") {
        int n;
std::cin >> n;
        im.dilate(n);
    } else if (expr == "clean") {
        im.clean_boundary();
    } else if (expr == "display_queue") {
        im.display_points();
    } else if (expr == "verify-queue") {
        if ( im.is_unique() )
std::cout << "Correct boundary queue!!" << endl;
        else
std::cout << "Incorrect boundary queue!!" << endl;
    } else if (expr == "queue-ordered?") {
        if ( im.is_ordered() )
std::cout << "Ordered boundary queue!!" << endl;
        else
std::cout << "Unordered boundary queue!!" << endl;
    }  else if (expr == "curvature") {
        im.get_curvature();
    } else if (expr == "statistic") {
        im.get_statistics();
    } else if (expr == "results") {
std::cout << _filename << " ";
        for (int i=0; i<NUM_FEATURES; ++i)
std::cout << feature_vector[i] << " ";
        //                std::cout << endl;
    } else {
std::cerr << "Invalid input: " << expr << endl;
        return;
    }

    finish_ = clock();
    if (VERBOSE) std::cerr << "done. Evaluation took "
        << (static_cast<double> (finish_-start_)) / CLOCKS_PER_SEC
        << " seconds\n" << endl;
}


void parse_command_line(int argc, char *argv[], Image & im)
{
    char c;
    while (--argc > 0 && (*++argv)[0] == '-')
        while ( (c = *++argv[0]) != '\0')
            switch (c) {
                case 'd':
                    BREADTH_FIRST = false;
                    break;
                case 'v':
                    VERBOSE = true;
                    break;
                default:
                    std::cout << "illegal option" << endl;
                    exit (0);
                    break;
            }
                if (argc == 1)
                    im.read_TIFF(argv[0]);
}

