//
// Created by serizba on 9/01/19.
//

#ifndef GRAFICA2_CONE_H
#define GRAFICA2_CONE_H



#include "DirPoint.h"
#include "Figure.h"
#include "Plane.h"

class Cone : public Figure {
    double a ,rbase;
    DirPoint p, pbase, v;
    Plane *b;

    // for textured cones
    DirPoint ref;


public:
    Cone(DirPoint p1, DirPoint p2, double r1, double r2,  BRDF brdf);
    Cone(DirPoint p1, DirPoint p2, double r1, double r2, DirPoint ref, BRDF brdf);
    //Cylinder(DirPoint p1, DirPoint p2, double r, DirPoint ref, BRDF brdf);

    tuple<double,TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const override;

};

#endif //GRAFICA2_CONE_H
