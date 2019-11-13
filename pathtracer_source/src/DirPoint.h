//
// Created by serizba on 13/11/18.
//

#ifndef GRAFICA_DIRPOINT_H
#define GRAFICA_DIRPOINT_H


#include<iostream>
#include<array>
#include<cmath>

using namespace std;

class DirPoint {
private:
    array<double,4> coord;
public:
    DirPoint(double x, double y, double z, double h);
    DirPoint(double x, double y, double z);
    DirPoint();

    bool isPoint() const;

    DirPoint operator+(const DirPoint& dp) const;
    DirPoint operator-(const DirPoint& dp) const;
    DirPoint operator*(const double s) const;
    DirPoint operator/(const double s) const;
    double operator[](const int a) const;
    double& operator[](const int a);
    double operator*(const DirPoint& dp) const;
    DirPoint cross(const DirPoint& dp) const;
    DirPoint mult(const DirPoint& dp) const;
    double modulus() const;
    double avg() const;
    void normalize();
    static DirPoint normalize(const DirPoint d);


    friend ostream& operator<<(ostream& os, const DirPoint& dp);
    friend DirPoint operator*(double s, const DirPoint& dp);
};


#endif //GRAFICA_DIRPOINT_H
