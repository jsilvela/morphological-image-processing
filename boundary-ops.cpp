#include "image.hpp"
#include <stack>
#include <iostream>
#include <cstring>
#include <assert.h>
#include <math.h>


static const grey bound = 250;
static const grey temp = 251;
static const grey skel = 200;

extern void display (Image & im, char *title);

// color_queue: colors boundary points stored in queue
void color_queue(queue<point> Q, grey level)
{
    while ( !Q.empty() ) {
        *( Q.front()) = level;
        Q.pop();
    }
}

typedef
struct _vect {
    int x, y;
} vect;

double norm(vect v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

double dot_prod(vect a, vect b)
{
    return (a.x * b.x + a.y * b.y);
}

double cos_angle(vect a, vect b)
{
    return (dot_prod(a,b)/(norm(a)*norm(b)));
}

vect sum(vect a, vect b)
{
    vect sum;
    sum.x = a.x + b.x;
    sum.y = a.y + b.y;
    return sum;
}

vect dif(vect a, vect b)
{
    vect dif;
    dif.x = a.x + b.x;
    dif.y = a.y + b.y;
    return dif;
}

double Image::distance_between(point pt1, point pt2)
{
    int d = abs(static_cast<double>(pt1 - pt2));
    int dy = d / width;
    int dx = d % width;
    if (dx > width / 2) {
        dx = width - dx;
        dy = dy + 1;
    }
    return sqrt(dx*dx + dy*dy);
}
    
double Image::distance_to_boundary(point pt)
{
    double d, last=1000.0;
    queue<point> Q;
    point hd;

    Q = boundary_queue;
    while (!Q.empty()) {
        hd = Q.front();
        d = distance_between(pt,hd);
        if (d<last)
            last=d;
        Q.pop();
    }
    return last;
}

// An 8_boundary point is white and has a black 8_neighbor
bool Image::is_8boundary_point(point pt)
{
    point aux;
    if (*pt == white) {
        for (int i=0; i<8; ++i) {
            aux= pt + offset_8neighbor[i];
            if (!in_range(aux))
                return true;
            else if (*aux == black)
                return true;
        }
    }
    return false;
}

// An 4_boundary point is white and has a black 4_neighbor
bool Image::is_4boundary_point(point pt)
{
    point aux;
    if (*pt == white) {
        for (int i=0; i<4; ++i) {
            aux= pt + offset_4neighbor[i];
            if (!in_range(aux))
                return true;
            else if (*aux == black)
                return true;
        }
    }
    return false;
}


point Image::get_next_boundary_point(point last)
{
    point pt;
    for (pt=last+1; pt<=end; ++pt)
        if (is_8boundary_point(pt))
            return pt;
    return 0;
}



// get_skeleton: erodes region repeatedly without removing skeletal points
void Image::get_skeleton()
{
    int i, num_neighbors;
    point pt, aux;
    if (boundary_queue.empty()) {
std::cerr << "get_skeleton: given an empty boundary queue!!"
        << endl;
        return;
    }
    while ( !boundary_queue.empty()) {
        pt=boundary_queue.front();
        for (i=0; i<8; ++i) {
            aux= pt + offset_8neighbor[i];
            if (in_range(aux) && *aux==white) {
                num_neighbors = get_num_8neighbors(aux, white);
                if (!makes_8connection(aux, bound)
                    && num_neighbors >= 2
                    && num_neighbors <= 6
                    ) {
                    *aux=bound;
                    boundary_queue.push(aux);
                } else {
                    *aux=skel;
                }
            }
        }
        boundary_queue.pop();
    }
}

void Image::distance_transform()
{
    point pt, aux;
    grey distance = 1;
    int i, npix;
    if (boundary_queue.empty()) {
std::cerr << "distance_transform: given an empty boundary queue!!"
        << endl;
        return;
    }
    while (!boundary_queue.empty()) {
        ++distance;
        aux = 0;
        boundary_queue.push(aux);  // null pixel separates layers
        npix = 0;
        pt = boundary_queue.front();
        while (pt !=0) {
            for (i=0; i<8; ++i) {
                aux= pt + offset_8neighbor[i];
                if (in_range(aux) && *aux==white) {
                    *aux=distance;
                    boundary_queue.push(aux);
                }
            }
            boundary_queue.pop();
            ++ npix;
            pt = boundary_queue.front();
        }
        if (VERBOSE)
std::cout << "distance_transform: " << npix
    << " pixels removed in iteration "
    << static_cast<int>(distance-bound) << endl;
        boundary_queue.pop();  // remove null pixel
    }
}

void Image::brute_distance_transform()
{
    point pt;
    int i;
    for (i=0,pt=start; pt<=end; ++pt,++i) {
        if (i== width) {
std::cout << "." << flush;
            i = 0;
        }
        if (*pt == white) {
std::cout << endl;
            *pt = distance_to_boundary(pt);
std::cout << (int) *pt << endl;
        }
    }
}	


void Image::euclid_transform()
{
    point pt, aux;
    vect *vectmatrix;
    vect *Vaux, *Vpt;
    vect offsets[8] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    vectmatrix = new vect[size];
    double d;
    int i;
    for (i=0; i<size; ++i) {
        if (start[i] == white) {
            (vectmatrix[i]).x = 1000; // so that distance vector is unrealistically big
            (vectmatrix[i]).y = 0;
        } else {
            (vectmatrix[i]).x = 0;
            (vectmatrix[i]).y = 0;
        }
    }
    while (!boundary_queue.empty()) {
        pt = boundary_queue.front();
        Vpt = vectmatrix + (pt - start);
        for (i=0; i<8; ++i) {
            aux= pt + offset_8neighbor[i];
            Vaux = Vpt + offset_8neighbor[i];
            if (in_range(aux) &&
                (norm(*Vaux) > (d = norm(sum(*Vpt,offsets[i])))))
            {
                *Vaux = sum(*Vpt,offsets[i]);
                boundary_queue.push(aux);
                *aux = floor(d);
            }
        }
        boundary_queue.pop();
    }
}

/*
void Image::skel_transform_bis()
{
    point pt, aux;
    vect *vectmatrix;
    vect *Vaux, *Vpt;
    vect offsets[8] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    vectmatrix = new vect[size];
    double d;
    int i, npix;
    for (i=0; i<size; ++i) {
        if (start[i] == white) {
            (vectmatrix[i]).x = 1000; // so that distance vector is unrealistically big
            (vectmatrix[i]).y = 0;
        } else {
            (vectmatrix[i]).x = 0;
            (vectmatrix[i]).y = 0;
        }
    }
    while (!boundary_queue.empty()) {
        pt = boundary_queue.front();
        Vpt = vectmatrix + (pt - start);
        for (i=0; i<8; ++i) {
            aux= pt + offset_8neighbor[i];
            Vaux = Vpt + offset_8neighbor[i];
            if (in_range(aux))
                if (norm(*Vaux) > (d = norm(sum(*Vpt,offsets[i])))) {
                    *Vaux = sum(*Vpt,offsets[i]);
                    boundary_queue.push(aux);
                    *aux = floor(d);
                } else if (cos_angle(*Vaux, *Vpt) < 0.8 &&
                           norm(dif(*Vaux,*Vpt)) > 20.0) {
                    *aux = 111;
                }
        }
        boundary_queue.pop();
    }
}
*/

void Image::skel_transform()
{
    point pt, aux;
    vect *vectmatrix;
    vect *Vaux, *Vpt;
    vect offsets[8] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    vectmatrix = new vect[size];
    double d;
    int i;
    for (i=0; i<size; ++i) {
        if (start[i] == white) {
            (vectmatrix[i]).x = 1000; // so that distance vector is unrealistically big
            (vectmatrix[i]).y = 0;
        } else {
            (vectmatrix[i]).x = 0;
            (vectmatrix[i]).y = 0;
        }
    }
    while (!boundary_queue.empty()) {
        pt = boundary_queue.front();
        Vpt = vectmatrix + (pt - start);
        for (i=0; i<8; ++i) {
            aux= pt + offset_8neighbor[i];
            Vaux = Vpt + offset_8neighbor[i];
            if (in_range(aux))
                if (norm(*Vaux) > (d = norm(sum(*Vpt,offsets[i])))) {
                    *Vaux = sum(*Vpt,offsets[i]);
                    boundary_queue.push(aux);
    //                *aux = floor(d);
                }
        }
        boundary_queue.pop();
    }
        for (pt = start; pt < end; ++pt)
            if (*pt == white) {
                Vpt = vectmatrix + (pt - start);
                for (i = 0; i<8; ++i) {
                    Vaux = Vpt + offset_8neighbor[i];
                    if (in_range(aux = pt + offset_8neighbor[i]) &&
                        cos_angle(*Vpt,*Vaux) < 0.8 &&
                        norm(dif(*Vpt,*Vaux)) > 20.0)
                        *aux = 111;
                }
            }
}


void Image::euclid2_transform()
{
    point pt, aux;
    vect *vectmatrix;
    vect *Vaux, *Vpt;
    vect offsets[8] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    vectmatrix = new vect[size];
    int i, npix;
std::memset(vectmatrix, 0, 2*size);
    /*    for (i=0; i<size; ++i) {
        (vectmatrix[i]).x = 0;
    (vectmatrix[i]).y = 0;
    }
*/
    if (boundary_queue.empty()) {
std::cerr << "distance_transform: given an empty boundary queue!!"
        << endl;
        return;
    }
    while (!boundary_queue.empty()) {
        aux = 0;
        boundary_queue.push(aux);  // null pixel separates layers
        npix = 0;
        pt = boundary_queue.front();
        while (pt !=0) {
            Vpt = vectmatrix + (pt - start);
            for (i=0; i<8; ++i) {
                aux= pt + offset_8neighbor[i];
                Vaux = Vpt + offset_8neighbor[i];
                if (in_range(aux)) {
                    if (*aux==white) {
                        *Vaux = sum(*Vpt,offsets[i]);
                        *aux = 	temp;  // temp to distinguish pixels that aren't ready
                        boundary_queue.push(aux);
                    } else if (*aux == temp) {
                        int d = (int) norm(sum(*Vpt, offsets[i]));
                        if (d<norm(*Vaux)) {
                            *Vaux = sum(*Vpt, offsets[i]);
                        }
                    }
                }
            }
            *pt = floor(norm(*Vpt));
            boundary_queue.pop();
            ++ npix;
            pt = boundary_queue.front();
        }	
            boundary_queue.pop();  // remove null pixel
            color_queue(boundary_queue, bound);
    }
}

void Image::erode(int n)
{
    point pt, aux;
    int i;
    if (boundary_queue.empty()) {
std::cerr << "erode: given an empty boundary queue!!"
        << endl;
        return;
    }
    for ( ; n>0; --n) {
        aux = 0;
        boundary_queue.push(aux);
        pt = boundary_queue.front();
        while (pt !=0) {
            for (i=0; i<8; ++i) {
                aux= pt + offset_8neighbor[i];
                if (in_range(aux) && *aux==white) {
                    *aux=bound;
                    boundary_queue.push(aux);
                }
            }
            *pt=black;
            boundary_queue.pop();
            pt = boundary_queue.front();
        }
        boundary_queue.pop();
    }
}


void Image::dilate(int n)
{
    int i;
    point pt, aux;
    if (boundary_queue.empty()) {
std::cerr << "dilate: given an empty boundary queue!!"
        << endl;
        return;
    }
    for ( ; n>0; --n) {
        aux = 0;
        boundary_queue.push(aux);
        pt = boundary_queue.front();
        while (pt !=0) {
            for (i=0; i<8; ++i) {
                aux= pt + offset_8neighbor[i];
                if (in_range(aux) && *aux==black) {
                    *aux=bound;
                    boundary_queue.push(aux);
                }
            }
            *pt=white;
            boundary_queue.pop();
            pt = boundary_queue.front();
        }
        boundary_queue.pop();
    }
}


// makes_8connection: true if coloring a point modifies connectivity
bool Image::makes_8connection(point pt, grey level)
{
    int crossing_index = 0;
    bool in_level;
    point aux;

    aux = pt + offset_8neighbor[7];
    in_level = (in_range(aux) && (*aux == level));
    for (int i=0; i<8; ++i) {
        aux = pt + offset_8neighbor[i];
        if (in_level && (!in_range(aux) || (*aux!=level))) {
            in_level = false;
            ++ crossing_index;
        } else if (!in_level && in_range(aux) && (*aux == level)) {
            in_level = true;
        }
    }
    return (crossing_index > 1);
}


// makes_8connection: true if coloring a point modifies connectivity
bool Image::makes_4connection(point pt, grey level)
{
    int crossing_index = 0;
    bool in_level;
    point aux;

    aux = pt + offset_4neighbor[3];
    in_level = (in_range(aux) && (*aux == level));
    for (int i=0; i<4; ++i) {
        aux = pt + offset_4neighbor[i];
        if (in_level && (!in_range(aux) || (*aux!=level))) {
            in_level = false;
            ++ crossing_index;
        } else if (!in_level && in_range(aux) && (*aux == level)) {
            in_level = true;
        }
    }
    return (crossing_index != 1);
}


//computes number of white 8-neighbors
int Image::get_num_8neighbors(point pt, grey level)
{
    point aux;
    int i, re=0;
    for (i=0; i<8; ++i) {
        aux = pt + offset_8neighbor[i];
        if (in_range(aux) && *aux==level)
            ++re;
    }
    return re;
}

int Image::get_num_4neighbors(point pt, grey level)
{
    point aux;
    int i, re=0;
    for (i=0; i<4; ++i) {
        aux = pt + offset_4neighbor[i];
        if (in_range(aux) && *aux==level)
            ++re;
    }
    return re;
}




// displays boundary points in queue (deletes other points)
void Image::display_points()
{
std::memset(start, black, size*sizeof(grey));
    color_queue(boundary_queue, white);
}


// print point's row and column numbers
void Image::print_point(point pt)
{
    int dif, row, col;
    dif = pt-start;
    row = dif / width;
    col = dif % width;
std::cout << "row = " << row << "; col = " << col << " ";
}


// get_boundary_breadth_first: gets unordered boundary using BFS
void Image::get_boundary_breadth_first()
{
    int i;
    queue<point> Q;

    point pt, aux, last = start-1;
    filter_by_level(white);
    while ( (pt=get_next_boundary_point(last)) ) {
        last = pt;
        *pt = bound;
        Q.push(pt);
        while ( !Q.empty()) {
            pt=Q.front();
            for (i=0; i<4; ++i) {
                aux= pt + offset_4neighbor[i];
                if (in_range(aux) &&
                    is_8boundary_point(aux)) {
                    *aux=bound;
                    Q.push(aux);
                }
            }
            boundary_queue.push(pt);
            Q.pop();
        }
    }
}

// get_boundary_depth_first: gets unordered boundary using DFS
void Image::get_boundary_depth_first()
{
    int i;
    bool is_on_top;
    stack<point> S;

    point pt, aux, last = start-1;
    filter_by_level(white);
    while ( (pt=get_next_boundary_point(last)) ) {
        last = pt;
        *pt = bound;
        S.push(pt);
        while ( !S.empty()) {
            pt=S.top();
            is_on_top = true;
            for (i=0; i<4; ++i) {
                aux= pt + offset_4neighbor[i];
                if (in_range(aux) &&
                    is_8boundary_point(aux)) {
                    *aux=bound;
                    S.push(aux);
                    is_on_top = false;
                    break;
                }
            }
            if (is_on_top) {
                S.pop();
                boundary_queue.push(pt);
            }
        }
    }
}

void Image::get_boundary()
{
    if (BREADTH_FIRST)
        get_boundary_breadth_first();
    else
        get_boundary_depth_first();
}


// data structures for breadth first graph traversal. As in Cormen's "Introd. to Algorithms"
struct node {
    point pt;
    int parent;
};

struct node make_node(point p, int par)
{
    struct node re;
    re.pt = p;
    re.parent = par;
    return re;
}

// oldest ancestor of node in breadth-first tree
int min_reachable(vector<struct node> & V, int i)
{
    while (i > 0) {
        if ( V[i].pt != 0) {
            i = V[i].parent;
        } else
            return i;
    }
    return 0;
}


// make_ordered: takes a vector containing a breadth-first graph of the image
// boundary and extracts from it the contour, which it places on a queue
void make_ordered (vector<struct node> & V, queue<point> & Q, int last)
{
    int i;
    stack<point> S;

    i=last;
    while (i != -1) {
        Q.push(V[i].pt);
        V[i].pt = 0;       // to avoid loops
        i = V[i].parent;
    }

    i=last-1;
    while ( min_reachable(V, i) > 10 ) --i;

    while (V[i].pt != 0) {
        S.push(V[i].pt);
        i = V[i].parent;
    }

    while ( !S.empty() ) {
        Q.push( S.top() );
        S.pop();
    }
}


// get_contour_breadth_first: gets ordered contour using BFS
void Image::get_contour_breadth_first(bool once)
{
    int i;
    int front, back;
    point pt, aux, last = start-1;

    filter_by_level(white);
    while ( (pt = get_next_boundary_point(last)) ) {
        last = pt;
        vector<struct node> V;
        *pt = bound;
        front = back = 0;
        V.push_back( make_node(pt, -1) );
        ++back;
        while ( front != back ) {
            pt = V[front].pt;
            for (i=0; i<4; ++i) {
                aux= pt + offset_4neighbor[i];
                if (in_range(aux) &&
                    is_8boundary_point(aux)) {
                    *aux=bound;
                    V.push_back( make_node(aux, front) );
                    ++back;
                }
            }
            ++front;
        }
        make_ordered(V, boundary_queue, back-1);
        if (once) break;
    }
    erase_points_with_level(bound);
    erase_small_whites();
    color_queue(boundary_queue, bound);
}

void stack_to_queue(stack<point>  & S, queue<point> & Q)
{
    while ( !S.empty()) {
        Q.push( S.top());
        S.pop();
    }
}


// get_contour_depth_first: gets ordered boundary using DFS
void Image::get_contour_depth_first(bool once)
{
    int i;
    point pt, aux, last = start-1;
    bool is_on_top;

    filter_by_level(white);
    while ( (pt=get_next_boundary_point(last)) ) {
        last = pt;
        stack<point> S, Max;
        *pt = bound;
        S.push(pt);
        while ( !S.empty()) {
            pt=S.top();
            is_on_top = true;
            for (i=0; i<4; ++i) {
                aux= pt + offset_4neighbor[i];
                if (in_range(aux) &&
                    is_8boundary_point(aux)) {
                    is_on_top = false;
                    *aux=bound;
                    S.push(aux);
                    break;
                }
            }
            if (is_on_top) {
                if (S.size() > Max.size()) {
                    Max = S;
                }
                S.pop();
            }
        }
        stack_to_queue(Max, boundary_queue);
        if (once) break;
    }
    erase_small_whites();
    erase_points_with_level(bound);
    color_queue(boundary_queue, bound);
}

void Image::get_contour(bool once)
{
    if (BREADTH_FIRST)
        return get_contour_breadth_first(once);
    else
        return get_contour_depth_first(once);
}


// clean_boundary: deletes little "hairs" and islets from contour
// so that it can be traversed in order
void Image::clean_boundary()
{
    vector<point> Keep;
    point aux;
    int i, size;

    while (!boundary_queue.empty()) {
        aux = boundary_queue.front();
        if (has_white_8neighbor(aux))
            Keep.push_back(aux);
        else
            *aux = black;
        boundary_queue.pop();
    }

    size = Keep.size();
    for (i=0; i<size; ++i)
        *( Keep[i] ) = white;
}


bool Image::has_white_8neighbor(point pt)
{
    point aux;
    for (int i=0; i<8; ++i) {
        aux= pt + offset_8neighbor[i];
        if (in_range(aux) && *aux == white)
            return true;
    }
    return false;
}

bool Image::has_white_4neighbor(point pt)
{
    point aux;
    for (int i=0; i<4; ++i) {
        aux= pt + offset_4neighbor[i];
        if (in_range(aux) && *aux == white)
            return true;
    }
    return false;
}

bool Image::are_4neighbors(point a, point b)
{
    int i;
    point aux;
    for (i=0; i<4; ++i) {
        aux = a + offset_4neighbor[i];
        if (in_range(aux) && aux == b)
            return true;
    }
    return false;
}


// get__contour_depth_first: apply after boundary is clean to get the ordered boundary
// the parameter "once" specifies whether to look for only outer contour or for all the
// connected boundary segments (in the case of a multiply connected region
void Image::get__contour_depth_first(bool once)
{
    int i;
    point pt, aux, first, last = start-1;
    bool is_on_top;

    pt = first = get_next_boundary_point(last);
    while ( pt != 0) {
        last = first = pt;
        stack<point> S;
        *pt = bound;
        S.push(pt);
        while ( !S.empty()) {
            pt=S.top();
            is_on_top = true;
            for (i=0; i<4; ++i) {
                aux= pt + offset_4neighbor[i];
                if (in_range(aux) &&
                    is_8boundary_point(aux)) {
                    is_on_top = false;
                    *aux=bound;
                    S.push(aux);
                    break;
                }
            }
            if (is_on_top) {
                assert( are_4neighbors(pt, first) );
                stack_to_queue(S, boundary_queue);
            }
        }
        if (once) break;
        pt = get_next_boundary_point(last);

    }
}

// sorts even and odd terms of a list. Auxiliary function for contour scanning
void unscramble (vector<point> & V, queue<point> & Q)
{
    int i, l_even, l_odd;

    i = V.size()-1;
    if (i % 2 == 0) {
        l_even = i;
        l_odd = i-1;
    } else {
        l_even = i-1;
        l_odd = i;
    }

    for (i=l_even; i >= 0; i -= 2)
        Q.push(V[i]);
    for (i=1; i<=l_odd; i+=2)
        Q.push(V[i]);
}

// get__contour_breadth_first: apply after boundary is clean to get the ordered boundary
// the parameter "once" specifies whether to look for only outer contour or for all the
// connected boundary segments (in the case of a multiply connected region
void Image::get__contour_breadth_first(bool once)
{
    int i;
    int front, back;
    point pt, aux, last = start-1;

    while ( (pt = get_next_boundary_point(last)) ) {
        vector<point> V;
        last = pt;
        *pt = bound;
        front = back = 0;
        V.push_back(pt);
        ++back;
        while ( front != back ) {
            pt = V[front];
            for (i=0; i<4; ++i) {
                aux= pt + offset_4neighbor[i];
                if (in_range(aux) &&
                    is_8boundary_point(aux)) {
                    *aux=bound;
                    V.push_back(aux);
                    ++back;
                }
            }
            ++front;
        }
        unscramble(V, boundary_queue);
        if (once) break;
    }
}


void Image::get__contour(bool once)
{
    if (BREADTH_FIRST)
        return get__contour_depth_first(once);
    else
        return get__contour_breadth_first(once);
}



// true if every boundary point has 2 boundary neighbors
bool Image::is_unique()
{
    point pt;

    while (!boundary_queue.empty()) {
        pt = boundary_queue.front();
        if ( get_num_4neighbors(pt, bound) != 2)
            return false;
        boundary_queue.pop();
    }
    return true;
}


// true if boundary queue starts and finishes in the same point
bool Image::is_ordered()
{
    point last, pt;

    last = boundary_queue.back();
    while (!boundary_queue.empty()) {
        pt = boundary_queue.front();
        if (are_4neighbors(last, pt)) {
            last = pt;
            boundary_queue.pop();
        } else {
std::cout << "is_ordered: Offending points colored";
            print_point(last);
            print_point(pt);
            *last = *pt = skel;
            return false;
        }
    }
    return true;
}
