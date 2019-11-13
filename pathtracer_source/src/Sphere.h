//
// Created by serizba on 13/11/18.
//

#ifndef GRAFICA_SPHERE_H
#define GRAFICA_SPHERE_H


#include <iostream>
#include <array>
#include <cmath>
#include <limits>
#include "Figure.h"

using namespace std;

class Sphere : public Figure {
private:
    DirPoint c;
    double r;

    //for textured Spheres
    DirPoint pole; // point in the sphere considered poleNorth

public:
    Sphere(const DirPoint& c, const double r, const BRDF& brdf);
    // constructor for textures
    Sphere(const DirPoint& c, const double r, const float aziPole, const float incPole, const BRDF& brdf);

    tuple<double, TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const override;
};

#endif //GRAFICA_SPHERE_H
