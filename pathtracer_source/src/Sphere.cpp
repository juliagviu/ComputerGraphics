//
// Created by serizba on 13/11/18.
//

#include "Sphere.h"

Sphere::Sphere(const DirPoint &c, const double r, const BRDF& brdf) :
        c(c), r(r), Figure(brdf) {
}

Sphere::Sphere(const DirPoint &c, const double r, const float aziPole, const float incPole, const BRDF &brdf) :
        c(c), r(r), Figure(brdf), pole(c+r*DirPoint(sin(incPole)*cos(aziPole), sin(incPole)*sin(aziPole), cos(incPole),0)){
}

tuple<double, TMatrix> Sphere::intersect(DirPoint o, DirPoint p) {
    DirPoint v = DirPoint::normalize(p-o);
    double disc = pow(v*(o-c),2)-pow((o-c).modulus(),2)+pow(r,2);
    double dist = numeric_limits<double>::max();
    if(disc < 0) {
        // no solution exists
        return make_tuple(dist, TMatrix());
    } else if(disc == 0) {
        dist = -(v*(o-c));
    } else {
        auto d1 = -(v*(o-c))+sqrt(disc);
        auto d2 = -(v*(o-c))-sqrt(disc);
        if (d1<this->epsilon && d2>=this->epsilon) {
            dist = d2;
        } else if (d2<this->epsilon && d1>=this->epsilon) {
            dist = d1;
        } else {
            dist = min(-(v * (o - c)) + sqrt(disc), -(v * (o - c)) - sqrt(disc));
        }
    }
    DirPoint in = o+dist*DirPoint::normalize(p-o);
    // normal
    DirPoint n = DirPoint::normalize(in-c);
    // if (acos(n*v)<M_PI/2) {
    //     n =  -1*n;
    // }
    // Calculate new local base z = normal
    DirPoint x = DirPoint::normalize(n.cross(DirPoint(1,1,1,0)));
    DirPoint y = DirPoint::normalize(n.cross(x));

    return make_tuple(dist,TMatrix::changeBase(in, x, y, n));
}

BRDF Sphere::getBRDF(DirPoint p) const {
    if (this->brdf.isTextured()){
        /*// vector d in global coordinates
        DirPoint d = DirPoint::normalize(c-p);
        // calculate d in local base (y = pole)
        DirPoint y = DirPoint::normalize(DirPoint(pole[0],pole[1],pol[2],0));
        DirPoint x = DirPoint::normalize(y.cross(DirPoint(1,1,1,0)));
        DirPoint z = DirPoint::normalize(x.cross(y));
        d = TMatrix::changeBase(c,x,y,z).inverse()*d;
        // calculate u,v
        cout << d[2][0] << ","
        double u = 0.5+atan2(d[2],d[0])/(2*M_PI);
        double v = 0.5-asin(d[1])/M_PI;
        cout << u << " " << v << endl;*/
        // calculate p in local base (z=pole)
        DirPoint z = DirPoint::normalize(DirPoint(pole[0],pole[1],pole[2],0));
        DirPoint x = DirPoint::normalize(z.cross(DirPoint(1,1,1,0)));
        DirPoint y = DirPoint::normalize(z.cross(x));
        DirPoint pl = TMatrix::changeBase(c,x,y,z).inverse()*p;
        //calculate theta and phi
        double theta = atan2(sqrt(pl[0]*pl[0]+pl[1]*pl[1]),pl[2]);
        double phi = atan2(pl[1],pl[0]);
        //cout << "theta " << theta << ", phi " << phi << endl;
        double u = theta/M_PI;
        double v = (phi+M_PI)/(2*M_PI);
        //cout << "u " << u << ", v " << v << endl;
        return this->brdf.getBRDF(u,v);
    } else {
        return this->brdf;
    }
}

