//
// Created by serizba on 8/01/19.
//

#ifndef GRAFICA2_CYLINDER_H
#define GRAFICA2_CYLINDER_H


#include "DirPoint.h"
#include "Figure.h"
#include "Plane.h"

class Cylinder : public Figure {
    double r;
    DirPoint p1, p2, v;
    Plane *b1, *b2;

    // for textured cylinders
    DirPoint ref;


public:
    Cylinder(DirPoint p1, DirPoint p2, double r, BRDF brdf);
    Cylinder(DirPoint p1, DirPoint p2, double r, DirPoint ref, BRDF brdf);

    tuple<double,TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const override;

};


#endif //GRAFICA2_CYLINDER_H
