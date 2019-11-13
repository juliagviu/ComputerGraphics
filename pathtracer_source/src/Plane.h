//
// Created by serizba on 13/11/18.
//

#ifndef GRAFICA_PLANE_H
#define GRAFICA_PLANE_H


#include <iostream>
#include <array>
#include <cmath>
#include <limits>
#include "Figure.h"

using namespace std;

class Plane : public Figure {
private:
    double d;
    DirPoint n;
    DirPoint x,y;
    DirPoint p0;

    //For textured planes:
    // base vectors (their norm is the size of the texture)
    DirPoint tu;
    DirPoint tv;
    // origin of texture
    DirPoint o;


public:
    Plane(const double& d, const DirPoint& n, const BRDF& brdf);
    Plane(const DirPoint& d, const DirPoint& n, const BRDF& brdf);
    Plane(const double& d, const DirPoint& n);
    Plane();
    //constructor for textured planes
    Plane(const double& d, const DirPoint& n, const DirPoint& i, const DirPoint& o, float u, float v, const BRDF& brdf);

    tuple<double,TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const override;
};


#endif //GRAFICA_PLANE_H
