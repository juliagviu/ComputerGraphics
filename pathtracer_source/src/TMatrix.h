//
// Created by serizba on 13/11/18.
//

#ifndef GRAFICA_TMATRIX_H
#define GRAFICA_TMATRIX_H

#include <iostream>
#include <array>
#include <cmath>
#include "DirPoint.h"

using namespace std;

class TMatrix {
private:
    array<array<double,4>,4> m;
public:
    TMatrix();
    TMatrix(const array<array<double,4>,4> m);

    // Calculates inverse of transformation Matrix using [R^t  -R^t*r]
    //													 [ 0  0  0  1]
    TMatrix inverse();

    TMatrix transpose();

    static TMatrix identity();

    static TMatrix translation(const DirPoint& dp);

    static TMatrix scale(double x, double y, double z);

    // axis can be 0 to 2, 0-X 1-Y 2-Z
    // angle is in radians
    static TMatrix rotate(double angle, int axis);

    static TMatrix changeBase(const DirPoint& o, const DirPoint& u, const DirPoint& v, const DirPoint& w);

    TMatrix operator*(const TMatrix& t) const;

    DirPoint operator*(const DirPoint& dp) const;

    DirPoint operator[](const int a) const;

    friend ostream& operator<<(ostream& os, const TMatrix& t);
};


#endif //GRAFICA_TMATRIX_H
