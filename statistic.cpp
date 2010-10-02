#include "image.hpp"
#include <math.h>

double square(double x)
{
    return (x*x);
}

double momento_de_inercia(double angle, double mu_20, double mu_11, double mu_02)
{
    return ( square(cos(angle))*mu_02 + square(sin(angle))*mu_20 - sin(2*angle)*mu_11 );
}

void Image::get_statistics()
{
    int x, y;
    double sum = 0, sum_x=0, sum_y =0;
    double sum_xx =0, sum_xy = 0, sum_yy = 0;

    double cx, cy, mu_11, mu_20, mu_02;
    double mean_square_angle, axis_angle;
    double mom_1, mom_2;

    for (y=0; y<height; ++y) {
        for (x=0; x<width; ++x) {
            if (matrix[height-1-y][x] == white) {
                ++sum;
                sum_x += x;
                sum_y += y;
                sum_xx += x*x;
                sum_xy += x*y;
                sum_yy += y*y;
            }
        }
    }

    cx = sum_x / static_cast<double>(sum);
    cy = sum_y / static_cast<double>(sum);
    mu_11 = sum_xy - cx*cy*sum;
    mu_20 = sum_xx - cx*cx*sum;
    mu_02 = sum_yy - cy*cy*sum;

    mean_square_angle = atan(mu_11 / mu_20);
    if (mu_20 != mu_02) {
        axis_angle = 0.5 * atan(2 * mu_11 / (mu_20 - mu_02));
    } else {
        axis_angle = PI / 2;
    }
    mom_1 = momento_de_inercia(axis_angle + (PI/2), mu_20, mu_11, mu_02);
    mom_2 = momento_de_inercia(axis_angle, mu_20, mu_11, mu_02);
    if (mom_2 > mom_1) {
        if (axis_angle >= 0)
            axis_angle += -PI/2;
        else
            axis_angle += PI/2;

        feature_vector[axis_ang] = axis_angle;
        feature_vector[mean_square_ang] = mean_square_angle;
        feature_vector[mom_principal] = mom_1;
        feature_vector[mom_secundario] = mom_2;
        feature_vector[rel_aspecto] = mom_2 / mom_1;
    } else {
        feature_vector[axis_ang] = axis_angle;
        feature_vector[mean_square_ang] = mean_square_angle;
        feature_vector[mom_principal] = mom_2;
        feature_vector[mom_secundario] = mom_1;
        feature_vector[rel_aspecto] = mom_1 / mom_2;
    }
}
