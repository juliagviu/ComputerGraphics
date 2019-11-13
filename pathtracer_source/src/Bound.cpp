//
// Created by serizba on 10/01/19.
//

#include "Bound.h"

Bound::Bound(Figure *bound, const vector<Figure *> &figures, const BRDF &brdf) : Figure(brdf), bound(bound),
                                                                                 figures(figures) {}

tuple<double, TMatrix> Bound::intersect(DirPoint o, DirPoint p) {
    auto t = this->bound->intersect(o, p);
    if (get<0>(t) < numeric_limits<double>::max() && get<0>(t)>epsilon) {
        double di, dr; TMatrix ti, tr;
        dr = numeric_limits<double>::max();

        const double eps = epsilon;
        for (auto f : this->figures) {
            tie(di,ti) = f->intersect(o, p);
            if (di < dr && di > eps) {
                dr = di; tr = ti;
            }
        }
        return {dr, tr};
    }
    return {numeric_limits<double>::max(), {}};
}

BRDF Bound::getBRDF(DirPoint p) const {
    return this->brdf;
}
