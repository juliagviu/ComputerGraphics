//
// Created by serizba on 30/12/18.
//

#ifndef GRAFICA2_CSG_H
#define GRAFICA2_CSG_H


#include "Figure.h"

class CSGTree {
public:
    CSGTree* left;
    CSGTree* right;
    Figure* f;
    int op;
    // Constructor
    CSGTree(Figure* f) : left(nullptr), right(nullptr), f(f), op(-1) {}
    CSGTree(CSGTree* l, CSGTree* r, int o) : left(l), right(r), f(nullptr), op(o) {}
};

class CSG : public Figure {

private:
    CSGTree* t;
    vector<Figure*> figures;

    // For textured CSGs, cylinder mapping
    DirPoint p1, p2;
    double r;
    DirPoint ref;
    DirPoint v;

public:
    CSG(CSGTree* t, BRDF brdf);

    CSG(CSGTree *t, BRDF brdf, DirPoint p1, DirPoint p2, double r, DirPoint ref);
    tuple<double,TMatrix> intersect(DirPoint o, DirPoint p) override;
    BRDF getBRDF(DirPoint p) const;
};


#endif //GRAFICA2_CSG_H
