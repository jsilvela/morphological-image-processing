#include "image.hpp"


void Image::threshold(grey level)
{
    point pt;
    for(pt=start; pt<=end; ++pt)
        if (*pt < level)
            *pt=black;
        else
            *pt=white;
}

point Image::get_next_point_with_level(grey level, point last)
{
    point re;
    for (re=last+1; re<=end; ++re)
        if (*re == level)
            return re;
    return 0;
}

//  filter_by_level: puts points with given level to white, others to black
void Image::filter_by_level(grey level)
{
    point pt;
    for (pt=start; pt<=end; ++pt)
        if (*pt == level)
            *pt = white;
        else
            *pt = black;
}

void Image::erase_points_with_level(grey level)
{
    point pt;
    for (pt=start; pt<=end; ++pt)
        if (*pt == level)
            *pt = black;
}
