//
// Created by serizba on 10/01/19.
//

#ifndef GRAFICA2_BOUND_H
#define GRAFICA2_BOUND_H

#include "Figure.h"

class Bound : public Figure {

private:
    Figure* bound;
    vector<Figure*> figures;
public:
    Bound(Figure *bound, const vector<Figure *> &figures, const BRDF &brdf);
    tuple<double,TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const;
};


#endif //GRAFICA2_BOUND_H
