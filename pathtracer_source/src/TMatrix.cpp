//
// Created by serizba on 13/11/18.
//

#include "TMatrix.h"


ostream &operator<<( ostream &os, const TMatrix& t) {
    for (int i=0; i<4; i++) {
        os << "( ";
        os << t.m[i][0];
        for (int j=1; j<4; j++) {
            os << ", " << t.m[i][j];
        }
        os << " )" << endl;
    }
    return os;
}

TMatrix::TMatrix() {
    m = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
}

TMatrix::TMatrix(const array<array<double, 4>, 4> m) {
    this->m = m;
}

TMatrix TMatrix::inverse() {
    // R^t
    TMatrix t = this->transpose();
    // Adds 0s in last column and last row
    for (int i=0; i<3; i++) {
        t.m[i][3] = 0;
        t.m[3][i] = 0;
    }
    t.m[3][3] = 1;
    // r
    DirPoint d(this->m[0][3],this->m[1][3],this->m[2][3],1);
    // -R^t*r
    d = t*(d*(-1));
    t.m[0][3] = d[0]; t.m[1][3] = d[1]; t.m[2][3] = d[2];
    return t;
}

TMatrix TMatrix::transpose() {
    TMatrix t = TMatrix::identity();
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            t.m[i][j] = this->m[j][i];
        }
    }
    return t;
}

TMatrix TMatrix::identity() {
    TMatrix t;
    t.m[0][0]=1; t.m[1][1]=1; t.m[2][2]=1; t.m[3][3]=1;
    return t;
}

TMatrix TMatrix::translation(const DirPoint &dp) {
    TMatrix t = TMatrix::identity();
    t.m[0][3]=dp[0]; t.m[1][3]=dp[1]; t.m[2][3]=dp[2];
    return t;
}

TMatrix TMatrix::scale(double x, double y, double z) {
    TMatrix t = TMatrix::identity();
    t.m[0][0]=x; t.m[1][1]=y; t.m[2][2]=z;
    return t;
}

TMatrix TMatrix::rotate(double angle, int axis) {
    TMatrix t = TMatrix::identity();
    switch(axis) {
        case 0:
            t.m[1][1]=cos(angle); t.m[1][2]=-sin(angle); t.m[2][1]=sin(angle); t.m[2][2]=cos(angle);
            break;
        case 1:
            t.m[0][0]=cos(angle); t.m[2][0]=-sin(angle); t.m[0][2]=sin(angle); t.m[2][2]=cos(angle);
            break;
        case 2:
            t.m[0][0]=cos(angle); t.m[0][1]=-sin(angle); t.m[1][0]=sin(angle); t.m[1][1]=cos(angle);
            break;
        default:break;
    }
    return t;
}

TMatrix TMatrix::changeBase(const DirPoint &o, const DirPoint &u, const DirPoint &v, const DirPoint &w) {
#ifdef MY_DEBUG
    if (!o.isPoint() || u.isPoint() || v.isPoint() || w.isPoint()) {
        cerr << "Change base: o must be point, u,v and w directions" << endl;
    }
#endif
    array<array<double,4>,4> t = {u[0], v[0], w[0], o[0], u[1], v[1], w[1], o[1], u[2], v[2], w[2], o[2], 0,0,0,1};
    return TMatrix(t);
}

TMatrix TMatrix::operator*(const TMatrix &t) const {
    TMatrix res;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            for (int k=0; k<4; k++) {
                res.m[i][j] = res.m[i][j] + this->m[i][k]*t.m[k][j];
            }
        }
    }
    return res;
}

DirPoint TMatrix::operator*(const DirPoint &dp) const {
    array<double,4> v = {0,0,0,0};
    for (int i=0; i<4; i++) {
        for (int k=0; k<4; k++) {
            v[i] = v[i] + this->m[i][k]*dp[k];
        }
    }
    return DirPoint(v[0],v[1],v[2],v[3]);
}

DirPoint TMatrix::operator[](const int a) const {
    return DirPoint(m[0][a], m[1][a], m[2][a], m[3][a]);
}
