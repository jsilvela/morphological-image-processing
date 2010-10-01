/**********************************
region-ops.cpp

Basic operations on regions of pixels
Jaime Silvela
January 2000
**********************************/

#include <stack>
#include <iostream>
#include <assert.h>
#include "image.hpp"

bool VERBOSE = false;
bool BREADTH_FIRST = true;
const int SMALLEST_WHITE_REGION = 10000;
const int SMALLEST_BLACK_REGION = 2000;
const int NUM_FEATURES = 13;
double feature_vector[NUM_FEATURES];
char *_filename;
extern void display (Image im, char *title);

bool Image::in_range(point pt)
{
    return (pt >= start && pt <= end);
}


int Image::fill4_breadth_first(point first, grey g_in, grey g_out)
{
    queue<point> Q;
    int re=0;
    int i;
    point aux;

    *first=g_out;
    ++re;
    Q.push(first);
    while (!Q.empty()) {
        first = Q.front();
        for (i=0; i<4; ++i) {
            aux = first + offset_4neighbor[i];
            if (in_range(aux) && *aux==g_in) {
                *aux=g_out;
                ++re;
                Q.push(aux);
            }
        }
        Q.pop();
    }
    return re;
}

int Image::fill4_depth_first(point first, grey g_in, grey g_out)
{
    stack<point> S;
    int i, re=0;
    bool is_on_top;
    point aux;

    *first=g_out;
    S.push(first);
    ++re;
    while (!S.empty()) {
        first = S.top();
        is_on_top = true;
        for (i=0; i<4; ++i) {
            aux = first + offset_4neighbor[i];
            if (in_range(aux) && *aux==g_in) {
                is_on_top = false;
                *aux=g_out;
                ++re;
                S.push(aux);
                break;
            }
        }
        if (is_on_top) S.pop();
    }
    return re;
}

int Image::fill4_recursive(point first, grey g_in, grey g_out)
{
    point aux;
    int re=1;
    *first=g_out;
    for (int i=0; i<4; ++i) {
        aux = first + offset_4neighbor[i];
        if (in_range(aux) && *aux==g_in)
            re += fill4_recursive(aux, g_in, g_out);
    }
    return re;
}

int Image::fill(point first, grey g_in, grey g_out)
{
    if (BREADTH_FIRST)
        return fill4_breadth_first(first, g_in, g_out);
    else
        return fill4_depth_first(first, g_in, g_out);
}


// get_white_regions: puts hand pixels to white, others to black
void Image::get_white_regions()
{
    point pt, last;
    grey g_out = 1;
    last=start-1;
    int area;
    while ( (pt=get_next_point_with_level(white, last))) {
        assert(g_out < white);
        area = fill(pt, white, g_out);
        if (area > SMALLEST_WHITE_REGION) {
            feature_vector[hand_area]=area;
            filter_by_level(g_out);
            return;
        }
        ++g_out;
        last = pt;
    }
}


void Image::erase_small_whites()
{
    point pt, last, winner=0;
    grey wingrey=0, g_out = 1;
    last=start-1;
    int area;
    while ( (pt=get_next_point_with_level(white, last))) {
        assert(g_out < white);
        area = fill(pt, white, g_out);
        if (area < SMALLEST_WHITE_REGION) {
            fill(pt, g_out, black);
        } else if (area >= SMALLEST_WHITE_REGION) {
            winner = pt;
            wingrey = g_out;
        }
        ++g_out;
        last = pt;
    }
    fill(winner, wingrey, white);
}

// get_black_regions: gives a different greylevel to each region larger than
// a threshold area. Deletes smaller regions
void Image::get_black_regions()
{
    int holes = 0;
    point pt, last;
    grey g_out = 1;
    last=start-1;
    while ((pt=get_next_point_with_level(black, last))) {
        assert(g_out < white);
        if (fill(pt, black, g_out) < SMALLEST_BLACK_REGION)
            fill(pt, g_out, white);
        else
            ++holes;
        ++g_out;
        last=pt;
    }
    feature_vector[num_holes] = holes-1;
}

struct region {
    grey original_color;
    int size;
    point start_point;
};

struct region make_region(grey lvl, int sz, point startpt)
{
    struct region re;
    re.original_color = lvl;
    re.size = sz;
    re.start_point = startpt;
    return re;
}

vector<struct region> region_map;

void Image::get_regions_with_level(grey level)
{
    point pt, last;
    struct region reg;
    static grey g_out = 1;
    last=start-1;
    while ( (pt=get_next_point_with_level(level, last)) ) {
        assert(g_out < white);
        reg = make_region(level, fill(pt, level, g_out), pt);
        region_map.push_back(reg);
        ++g_out;
        last=pt;
    }
}

void Image::get_regions()
{
    get_regions_with_level(white);
    if (VERBOSE) std::cout << "Got white regions" << endl;
    get_regions_with_level(black);
    if (VERBOSE) std::cout << "Got black regions" << endl;
    int size = region_map.size();
    if (VERBOSE) std::cout << "Image has " << size << " regions"  << endl;
    for (int i=0; i<size; ++i) {
        if (region_map[i].size < SMALLEST_BLACK_REGION)
            if (region_map[i].original_color == white) {
                fill( region_map[i].start_point,
                      *(region_map[i].start_point),
                      black);
                region_map[i].start_point = 0;
            } else {
                fill( region_map[i].start_point,
                      *(region_map[i].start_point),
                      white);
                region_map[i].start_point = 0;
            }
        else if (region_map[i].size >= SMALLEST_WHITE_REGION)
            fill( region_map[i].start_point,
                  *(region_map[i].start_point),
                  region_map[i].original_color);
    }
}

// number_of_holes: counts holes by counting different greylevels, one
// for each region
int Image::number_of_holes()
{
    int i, re = 0;
    int *levels = new int[grey_depth];
    point pt;

    for (i=0; i<grey_depth; ++i)
        levels[i]=0;
    for (pt = start; pt <= end; ++pt)
        ++levels[*pt];
    for (i=0; i<grey_depth; ++i)
        if (levels[i])
            ++re;

    delete []levels;
    return re-2;
}
