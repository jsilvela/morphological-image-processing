#include "image.hpp"

// new experimental approach to skeletonization

int main()
{
    Image im1, im2;
    point start1, start2, end1, end2;
    point pt1, pt2;

    im1.read_TIFF((char *) "s20000.tif");
    im1.threshold(100);
    im1.get_white_regions();
    im1.get_black_regions();
    im1.get_boundary_depth_first();
    im1.erase_small_whites();
    im1.clean_boundary();
    im1.get__contour(false);
    im1.get_skeleton();
    im1.write_TIFF((char *) "skeleton1");

    im2.read_TIFF((char *) "s20000.tif");
    im2.threshold(100);
    im2.get_white_regions();
    im2.get_black_regions();
    im2.get_boundary_breadth_first();
    im2.get_skeleton();
    im1.write_TIFF((char *) "skeleton2");

    start1 = im1.get_start();
    start2 = im2.get_start();
    end1 = im1.get_end();
    end2 = im2.get_end();


    for (pt1 = start1, pt2 = start2; pt1<=end1 && pt2 <=end2; ++pt1, ++pt2)
        if ((*pt1 == 200) && (*pt2 == 200))
            *pt2 = 0;

    im2.write_TIFF((char *) "real_skeleton");

    return 0;
}
