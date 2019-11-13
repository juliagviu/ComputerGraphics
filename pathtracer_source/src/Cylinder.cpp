//
// Created by serizba on 8/01/19.
//

#include "Cylinder.h"


Cylinder::Cylinder(DirPoint p1, DirPoint p2, double r, BRDF brdf) {
    this->brdf = brdf;
    this->r = r;
    this->p1 = p1;
    this->p2 = p2;
    this->v = DirPoint::normalize(p2-p1);
    this->b1 = new Plane(p1, v, {});
    this->b2 = new Plane(p2, v, {});
}

// Constructor for textured cylinders
Cylinder::Cylinder(DirPoint p1, DirPoint p2, double r, DirPoint ref, BRDF brdf) {
    this->brdf = brdf;
    this->r = r;
    this->p1 = p1;
    this->p2 = p2;
    this->v = DirPoint::normalize(p2-p1);
    this->ref = DirPoint::normalize(this->v.cross(ref));
    this->b1 = new Plane(p1, v, {});
    this->b2 = new Plane(p2, v, {});
}

tuple<double, TMatrix> Cylinder::intersect(DirPoint o, DirPoint p) {
    DirPoint v = DirPoint::normalize(p-o);

    int capInter = 0;
    // Intersection with the caps
    auto cap1 = this->b1->intersect(o, p);
    if (get<0>(cap1) < numeric_limits<double>::max() && (get<1>(cap1)[3] - this->p1).modulus() < r) {
        capInter = 1;
    }

    auto cap2 = this->b2->intersect(o, p);
    if (get<0>(cap2) < numeric_limits<double>::max() && (get<1>(cap2)[3] - this->p2).modulus() < r) {
        capInter = 2;
    }


    // Intersection with the cylinder
    DirPoint Ap = o - this->p1;
    double a = (v - (v * this->v) * this->v) * (v - (v * this->v) * this->v);
    double b = 2 * (v - (v * this->v) * this->v) * (Ap - (Ap * this->v) * this->v);
    double c = (Ap - (Ap * this->v) * this->v) * (Ap - (Ap * this->v) * this->v) - this->r * this->r;

    double delta = b*b - 4*(a*c);
    if(delta < epsilon) {
        switch (capInter) {
            case 1:
                return cap1;
            case 2:
                return cap2;
            default:
                return {numeric_limits<double>::max(), {}};
        }
    }

    double t1 = (-b - sqrt(delta))/(2*a);
    double t2 = (-b + sqrt(delta))/(2*a);
    double t;

    if (t1>t2) t = t2;
    else t = t1;

    DirPoint it = o + v*t;
    DirPoint nlong = (this->p1 - it) - ((this->p1 - it) * this->v) * this->v;
    DirPoint n = -1 * DirPoint::normalize(nlong);
    DirPoint x = DirPoint::normalize(n.cross(DirPoint(1,1,1,0)));
    DirPoint y = DirPoint::normalize(n.cross(x));



    if (((it + nlong) - this->p1).modulus() > (this->p1 - this->p2).modulus() + epsilon || ((it + nlong) - this->p2).modulus() > (this->p1 - this->p2).modulus() + epsilon) {
        switch (capInter) {
            case 1:
                return cap1;
            case 2:
                return cap2;
            default:
                return {numeric_limits<double>::max(), {}};
        }
    }

    if (capInter == 1 && get<0>(cap1) < t) {
        return cap1;
    } else if (capInter == 2 && get<0>(cap2) < t) {
        return cap2;
    }

    return {t, TMatrix::changeBase(it, x, y , n)};
}


BRDF Cylinder::getBRDF(DirPoint p) const {
    if (this->brdf.isTextured()){
        DirPoint center = p + ((this->p1 - p) - ((this->p1 - p) * this->v) * this->v);
        if ((center - p).modulus() < (this->r - epsilon)){
            //cout << "cap" << endl;
        }
        // get u,v (theta and height) in range [0,1]
        double v = ((this->p1 - center).modulus())/ ((this->p1-this->p2).modulus());
        if (v > 1) v=1.0; // numeric aproximations

        double cosTh = DirPoint::normalize(p-center)*DirPoint::normalize(this->ref);
        double sinTh = DirPoint::normalize(p-center).cross(DirPoint::normalize(this->ref)).modulus();

        double u = atan2(sinTh, cosTh) / (2*M_PI);

        return this->brdf.getBRDF(static_cast<float>(u), static_cast<float>(v));
    } else {
        return this->brdf;
    }
}
