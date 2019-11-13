//
// Created by serizba on 13/11/18.
//

#include "DirPoint.h"


DirPoint operator*(double s, const DirPoint &dp) {
    return dp*s;
}

ostream &operator<<(ostream &os, const DirPoint &dp) {
    os << "(" << dp.coord[0] << "," << dp.coord[1] << "," << dp.coord[2] << "," << dp.coord[3] << ")";
    return os;
}

DirPoint::DirPoint(double x, double y, double z, double h) : coord({x,y,z,h}) {}
DirPoint::DirPoint(double x, double y, double z) : coord({x,y,z,0}) {}
DirPoint::DirPoint() : coord({0,0,0,0}) {}


bool DirPoint::isPoint() const {
    return this->coord[3]==1;
}

void DirPoint::normalize() {
#ifdef MY_DEBUG
    if (this->isPoint()) {
        cerr << "Points can not be normalized" << endl;
    }
#endif
    double modulus = this->modulus();
    this->coord[0] = this->coord[0] / modulus;
    this->coord[1] = this->coord[1] / modulus;
    this->coord[2] = this->coord[2] / modulus;
}

double DirPoint::modulus() const {
#ifdef MY_DEBUG
    if (this->isPoint()) {
        cerr << "Points do not have modulus" << endl;
    }
#endif
    return sqrt(pow(this->coord[0],2)+pow(this->coord[1],2)+pow(this->coord[2],2));
}

DirPoint DirPoint::cross(const DirPoint &dp) const {
#ifdef MY_DEBUG
    if (dp.isPoint() || this->isPoint()) {
        cerr << "Points cannot be multiplied " << endl;
    }
#endif
    double x = this->coord[1]*dp.coord[2]-this->coord[2]*dp.coord[1];
    double y = this->coord[2]*dp.coord[0]-this->coord[0]*dp.coord[2];
    double z = this->coord[0]*dp.coord[1]-this->coord[1]*dp.coord[0];
    return DirPoint(x,y,z,0);
}

double DirPoint::operator*(const DirPoint &dp) const {
#ifdef MY_DEBUG
    if (dp.isPoint() || this->isPoint()) {
        cerr << "Points cannot be multiplied " << endl;
    }
#endif
    return this->coord[0]*dp.coord[0]+this->coord[1]*dp.coord[1]+this->coord[2]*dp.coord[2];
}

double &DirPoint::operator[](const int a) {
    return this->coord[a];
}

DirPoint DirPoint::operator/(const double s) const {
#ifdef MY_DEBUG
    if (this->isPoint()) {
        cerr << "Points cannot be scaled " << endl;
    }
#endif
    return DirPoint(this->coord[0]/s,this->coord[1]/s,this->coord[2]/s,0);
}

DirPoint DirPoint::operator-(const DirPoint &dp) const {
#ifdef MY_DEBUG
    if (dp.isPoint() && !this->isPoint() || !dp.isPoint() && this->isPoint()) {
        cerr << "Points cannot be substracted with directions " << endl;
    }
#endif
    return DirPoint(this->coord[0]-dp.coord[0],this->coord[1]-dp.coord[1],this->coord[2]-dp.coord[2],this->coord[3]-dp.coord[3]);
}

DirPoint DirPoint::operator+(const DirPoint &dp) const {
#ifdef MY_DEBUG
    if (dp.isPoint() && this->isPoint()) {
        cerr << "Points cannot be added " << endl;
    }
#endif
    return DirPoint(this->coord[0]+dp.coord[0],this->coord[1]+dp.coord[1],this->coord[2]+dp.coord[2],this->coord[3]+dp.coord[3]);
}

DirPoint DirPoint::operator*(const double s) const {
    return DirPoint(this->coord[0]*s,this->coord[1]*s,this->coord[2]*s,this->coord[3]);
}

double DirPoint::operator[](const int a) const {
    return this->coord[a];
}


DirPoint DirPoint::normalize(const DirPoint d) {
    DirPoint dd = d;
    dd.normalize();
    return dd;
}

double DirPoint::avg() const {
    return (coord[0]+coord[1]+coord[2])/3.0;
}

DirPoint DirPoint::mult(const DirPoint &dp) const {
    return {this->coord[0]*dp.coord[0],this->coord[1]*dp.coord[1],this->coord[2]*dp.coord[2]};
}

