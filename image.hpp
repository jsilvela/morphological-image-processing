/*********************************
image.hpp

Contains the Image class declaration and some global variables
Jaime Silvela
January 2000
**********************************/

#include <vector>
#include <queue>
#include <complex>

using namespace std;

typedef unsigned char grey;
typedef grey * point;
typedef complex<float> cmplx;

extern const int grey_depth;
extern const grey white;
extern const grey black;
extern bool VERBOSE;
extern bool BREADTH_FIRST;
extern const int NUM_FEATURES;
extern double feature_vector[];
extern char *_filename;
extern const double PI;
enum feature {letternum, hand_area, perimeter, circularity, curvosity, num_holes, num_fingers,
    mean_arg, mean_square_ang, axis_ang, mom_principal, mom_secundario, rel_aspecto};

class Image {
public:
    Image();
    Image(int h, int w);
    Image(const Image &);
    Image& operator=(const Image&);
    ~Image();
    int get_height();
    int get_width();
    int get_size();
    grey** get_matrix();
    point get_start();
    point get_end();
    void read_TIFF_bis(char *title);
    void read_TIFF(char *filename);
    void write_TIFF(char *title);
    void threshold(grey level);
    int fill(point pt, grey g_in, grey g_out);
    int fill4_recursive(point pt, grey g_in, grey g_out);
    int fill4_breadth_first(point pt, grey g_in, grey g_out);
    int fill4_depth_first(point pt, grey g_in, grey g_out);
    void get_regions_with_level(grey level);
    void get_regions();
    point get_next_point_with_level(grey level, point last);
    void erase_small_whites();
    void get_white_regions();
    void get_black_regions();
    void filter_by_level(grey level);
    void erase_points_with_level(grey level);
    int number_of_holes();
    point get_next_boundary_point(point pt);
    bool in_range(point pt);
    bool makes_8connection(point pt, grey level);
    bool makes_4connection(point pt, grey level);
    int get_num_8neighbors(point pt, grey level);
    int get_num_4neighbors(point pt, grey level);
    void get_skeleton();
    void get_curvature();
    double distance_between(point pt1, point pt2);
    double distance_to_boundary(point pt);
    void brute_distance_transform();
    void distance_transform();
    void euclid_transform();
    void skel_transform();
    void euclid2_transform();
    void erode(int n);
    void dilate(int n);
    void print_point(point pt);
    void rectangle(int h, int w);
    void display_points();
    bool is_8boundary_point(point pt);
    bool is_4boundary_point(point pt);
    bool has_white_8neighbor(point pt);
    bool has_white_4neighbor(point pt);
    void get_boundary();
    void get_boundary_depth_first();
    void get_boundary_breadth_first();
    void get_contour(bool once);
    void get_contour_depth_first(bool once);
    void get_contour_breadth_first(bool once);
    void get__contour(bool once);
    void get__contour_depth_first(bool once);
    void get__contour_breadth_first(bool once);
    bool are_4neighbors(point a, point b);
    void clean_boundary();
    bool is_unique();
    bool is_ordered();
    void get_statistics();
    cmplx make_complex(point pt);
    vector<cmplx> points_to_complex();
private:
    int height;
    int width;
    int size;
    grey **matrix;
    grey *start;
    grey *end;
    int offset_8neighbor[8];
    int offset_4neighbor[4];
    queue<point> boundary_queue;
};


