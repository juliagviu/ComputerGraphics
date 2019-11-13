//
// Created by serizba on 13/11/18.
//

#ifndef GRAFICA_FIGURE_H
#define GRAFICA_FIGURE_H

#include <tuple>
#include "BRDF.h"

using namespace std;

class Figure {
protected:
    BRDF brdf;
    const float epsilon = 0.09;
public:
    Figure(const BRDF &brdf);

    Figure();

    virtual tuple<double,TMatrix> intersect(DirPoint o, DirPoint p) = 0;

    // if it has textures calculates u and v from point p (depends on the object) and after gets BRDF from BRDF Class
    virtual BRDF getBRDF(DirPoint p) const = 0;

};


#endif //GRAFICA_FIGURE_H
