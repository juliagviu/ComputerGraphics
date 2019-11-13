//
// Created by serizba on 17/11/18.
//

#ifndef GRAFICA_CUBOID_H
#define GRAFICA_CUBOID_H


#include "Figure.h"
#include "Plane.h"

class Cuboid : public Figure {
private:
    DirPoint vx, vy, vz, vx2, vy2, vz2, p0;
    TMatrix cb, cbinv;
    array<Plane*,6> planes;

public:
    Cuboid(DirPoint vx, DirPoint vy, DirPoint vz, DirPoint p0, BRDF brdf);
    tuple<double, TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const override;
};


#endif //GRAFICA_CUBOID_H
