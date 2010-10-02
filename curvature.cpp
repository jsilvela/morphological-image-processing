#include "image.hpp"
#include <iostream>
#include <assert.h>

const cmplx minus_one(-1,0);
const double PI = arg(minus_one);

// make_complex: represents point as a complex number
cmplx Image::make_complex(point pt)
{
    int dif, row, col;
    dif = pt-start;
    row = dif / width;
    col = dif % width;
    cmplx re(col, height-1-row);
    return re;
}

// represents boundary queue as complex numbers
vector<cmplx> Image::points_to_complex()
{
    vector<cmplx> V;
    while (!boundary_queue.empty()) {
        V.push_back(make_complex(boundary_queue.front()));
        boundary_queue.pop();
    }
    return V;
}

void decimate(int n, vector<cmplx> & V)
{
    vector<cmplx> re;
    int i, size = V.size();
    for (i=0; i<size; ++i)
        if (i%n == 0)
            re.push_back(V[i]);
    V = re;
}

vector<float> get_arguments(vector<cmplx> vel)
{
    int size=vel.size();
    vector<float> Args(size);
    for (int i=0; i<size; ++i)
        Args[i]=arg(vel[i]);
    return Args;
}

void make_positive_args(vector<float> & V)
{
    int i, size = V.size();
    for (i=0; i<size; ++i)
        if (V[i] < 0)
            V[i] += PI;
}

void normalize_args(vector<float> & V)
{
    int i, size = V.size();
    for (i=0; i<size; ++i) {
        while (V[i] < -PI)
            V[i] += 2*PI;
        while (V[i] > PI)
            V[i] -= 2*PI;
    }
}

void differentiate(vector<cmplx> & V)
{
    int i, size = V.size();
    vector<cmplx> aux(size);

    for (i=1; i<size; ++i)
        aux[i]=V[i]-V[i-1];
    aux[0] = V[0] - V[size-1];
    V = aux;
}

void differentiate(vector<float> & V)
{
    int i, size = V.size();
    vector<float> aux(size);

    for (i=1; i<size; ++i)
        aux[i]=V[i]-V[i-1];
    aux[0] = V[0] - V[size-1];
    V = aux;
}





void smooth(int n, vector<float> & V)
{
    int i, j, size = V.size();
    float add;
    vector<float> re(size);

    for (i = 0; i<size; ++i) {
        add = 0.0;
        for (j=i; j<i+n; ++j)
            add += V[j % size];
        re[i] = add / n;
    }
    V = re;
}

void smooth(int n, vector<cmplx> & V)
{
    int i, j, size = V.size();
    cmplx add;
    cmplx nc = n;
    vector<cmplx> re(size);

    for (i = 0; i<size; ++i) {
        add = 0.0;
        for (j=i; j<i+n; ++j)
            add += V[j % size];
        re[i] = add / nc;
    }
    V = re;
}

void print_vector(vector<float> V)
{
    int i, size = V.size();

    for (i=0; i<size; ++i)
        std::cout << V[i] << endl;
}

void print_vector(vector<cmplx> V)
{
    int i, size = V.size();

    for (i=0; i<size; ++i)
        std::cout << real(V[i]) << " " << imag(V[i]) << endl;
}

void print_letters(vector<char> letters)
{
    int i, size = letters.size();

    for (i=0; i<size; ++i)
        std::cout << letters[i];
    std::cout << endl;
}

void dilate_tips(vector<char> & letters)
{
    int i, size = letters.size();
    vector<char> temp = letters;

    for (i=0; i<size; ++i) {
        if (letters[i] == 'c') {
            temp[(i+1)%size] = 'c';
            temp[(i-1)%size] = 'c';
        }
    }
    letters = temp;
}

// numfingers returns the number of crossings from curved to non-curved pixel ranges
int numfingers(vector<char> letters)
{
    int i, size = letters.size(), crossings =0;
    bool is_curved;
    dilate_tips(letters);
    is_curved = (letters[size-1] == 'c');
    for (i=0; i<size; ++i)
        if (letters[i] != 'c' && is_curved) {
            is_curved = false;
            ++crossings;
        }
        else if (letters[i] == 'c' && !is_curved) {
            is_curved = true;
        }
    return crossings;
}

void update_variables(float elem, float thresh, bool decrement, int & npos, int & nneg, int & nz, float & sum)
{
    if (decrement) {
        sum -= elem;
        if (elem > thresh)
            --npos;
        else if (elem < -thresh)
            --nneg;
        else
            --nz;
    } else {
        sum += elem;
        if (elem > thresh)
            ++npos;
        else if (elem < -thresh)
            ++nneg;
        else
            ++nz;
    }
}

vector<float> get_sign(float threshold, vector<float> V)
{
    int i, size=V.size();
    vector<float> re(size);
    float elem;

    for (i=0; i<size; ++i) {
        elem = V[i];
        if (elem > threshold)
            re[i] = 1;
        else if (elem < -threshold)
            re[i] = -1;
        else
            re[i] = 0;
    }
    return re;
}

void print_fingers(vector<float> V)
{
    int i, size = V.size();
    int ntested = 7, enough = 6;
    float thresh = 0.12;
    int npos, nneg, nz;
    float sum;
    const bool decrement = true, increment = false;
    vector<char> letters;
    char point_type;

    //        print_vector(get_sign(thresh,V));
    assert( ntested < size);
    npos = nneg = nz = 0;
    sum = 0.0;
    for (i=0; i<ntested; ++i) {
        update_variables(V[i], thresh, increment, npos, nneg, nz, sum);
    }
    for (i = 0; i<size; ++i) {
        if (npos >= enough) {
            point_type = 'c';
        } else if (nneg >= enough) {
            point_type = 'C';
        } else if (nz >= enough) {
            point_type = 's';
        } else if (npos >= 4) {
            point_type = 'g';
        } else if (nneg >= 4) {
            point_type = 'G';
        } else if (nz >= 4) {
            point_type = 's';
        } else {
            point_type = 'n';
        }
        if (VERBOSE) {
            std::cout << point_type;
        }
        letters.push_back(point_type);
        update_variables(V[i], thresh, decrement, npos, nneg, nz, sum);
        update_variables(V[(i+ntested)%size], thresh, increment, npos, nneg, nz, sum);
    }
    feature_vector[num_fingers] = numfingers(letters);
}

float get_mean_arg(vector<float> vel_args)
{
    int i,size = vel_args.size();
    float total=0.0;

    normalize_args(vel_args);
    make_positive_args(vel_args); // all args are taken to [0, pi]
    for (i=0; i<size; ++i)
        total += vel_args[i];
    return total / size;
}

// get_perimeter: V is the velocity vector of the contour. We return its absolute sum
double get_abs_sum(vector<cmplx> vel)
{
    int i, size = vel.size();
    double re = 0.0;

    for (i=1; i<size; ++i) {
        re += abs(vel[i]);
    }
    return re;
}

double get_abs_sum(vector<float> vel)
{
    int i, size = vel.size();
    double re = 0.0;

    for (i=1; i<size; ++i) {
        re += abs(vel[i]);
    }
    return re;
}

void Image::get_curvature()
{
    if (boundary_queue.empty()) {
        std::cerr << "curvature: given no curve" << endl;
        return;
    }
    vector<cmplx> c;
    c = points_to_complex();  // position_vector
    decimate(10, c); // decimated position vector
                     //      print_vector(c);   // produces plottable contour file
    differentiate(c);  // velocity vector
    feature_vector[perimeter] = get_abs_sum(c);  // sum of length of velocities
    feature_vector[circularity] =
        feature_vector[perimeter] * feature_vector[perimeter]
        / (4*PI*feature_vector[hand_area]);
    vector<float> Args;
    Args = get_arguments(c); // arguments of velocities
    feature_vector[mean_arg] = get_mean_arg(Args);  // mean argument of contour
    differentiate(Args); // angular velocity of contour velocity vector
    normalize_args(Args);
    //      print_vector(Args);  // produces angular acceleration function
    feature_vector[curvosity] = get_abs_sum(Args) / Args.size();
    print_fingers(Args);
}
