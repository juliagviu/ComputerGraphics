//
// Created by serizba on 9/01/19.
//

#include "Cone.h"

Cone::Cone(DirPoint p1, DirPoint p2, double r1, double r2, BRDF brdf) {
    this->brdf = brdf;
    this->a = atan((r1 - r2) / (p2 - p1).modulus());
    this->p = p1 + r1*((p2- p1) * (1.0/(r1 - r2)));
    this->v = DirPoint::normalize(p2-p1);
    this->pbase = p1;
    this->rbase = r1;
    this->b = new Plane(this->pbase, v, {});
}

Cone::Cone(DirPoint p1, DirPoint p2, double r1, double r2, DirPoint ref, BRDF brdf) {
    this->brdf = brdf;
    this->a = atan((r1 - r2) / (p2 - p1).modulus());
    this->p = p1 + r1*((p2- p1) * (1.0/(r1 - r2)));
    this->v = DirPoint::normalize(p2-p1);
    this->pbase = p1;
    this->rbase = r1;
    this->ref = DirPoint::normalize(this->v.cross(ref));
    this->b = new Plane(this->pbase, v, {});
}

tuple<double, TMatrix> Cone::intersect(DirPoint o, DirPoint p) {
    DirPoint v = DirPoint::normalize(p-o);
    DirPoint Ap = o - this->p;

    bool capInter = false;
    auto cap = this->b->intersect(o, p);
    if (get<0>(cap) < numeric_limits<double>::max() && (get<1>(cap)[3] - this->pbase).modulus() < this->rbase) {
        capInter = true;
    }

    double a = (cos(this->a) * cos(this->a)) * ((v - (v * this->v) * this->v) * (v - (v * this->v) * this->v)) -  (sin(this->a) * sin(this->a)) * ((v * this->v) * (v * this->v));
    double b = 2.0 * (cos(this->a) * cos(this->a)) * ((v - (v * this->v) * this->v) * (Ap - (Ap * this->v) * this->v)) - 2.0 * (sin(this->a) * sin(this->a)) * ((v * this->v) * (Ap * this->v));
    double c = (cos(this->a) * cos(this->a)) * ((Ap - (Ap * this->v) * this->v) * (Ap - (Ap * this->v) * this->v)) - (sin(this->a) * sin(this->a)) * ((Ap * this->v) * (Ap * this->v));

    double delta = b*b - 4*(a*c);
    if(delta < epsilon) {
        if (capInter) {
            return cap;
        }
        return {numeric_limits<double>::max(), {}};
    }

    double t1 = (-b - sqrt(delta))/(2*a);
    double t2 = (-b + sqrt(delta))/(2*a);
    double t;

    if (t1>t2) t = t2;
    else t = t1;

    DirPoint it = o + v*t;
    DirPoint nlong = (this->pbase - it) - ((this->pbase - it) * this->v) * this->v;
    DirPoint n = -1 * DirPoint::normalize(nlong);
    DirPoint x = DirPoint::normalize(n.cross(DirPoint(1,1,1,0)));
    DirPoint y = DirPoint::normalize(n.cross(x));

    if (((it + nlong) - this->pbase).modulus() > (this->p - this->pbase).modulus() + epsilon || ((it + nlong) - this->p).modulus() > (this->p - this->pbase).modulus() + epsilon) {
        if (capInter) {
            return cap;
        }
        return {numeric_limits<double>::max(), {}};
    }
    return {t, TMatrix::changeBase(it, x, y, n)};
}

BRDF Cone::getBRDF(DirPoint p) const {
    if (this->brdf.isTextured()){
        DirPoint center = p + ((this->pbase - p) - ((this->pbase - p) * this->v) * this->v);
        // get u,v (theta and height) in range [0,1]
        double v = ((this->pbase - center).modulus())/ ((this->pbase-this->p).modulus());
        if (v > 1) v=1.0; // numeric aproximations

        double cosTh = DirPoint::normalize(p-center)*DirPoint::normalize(this->ref);
        double sinTh = DirPoint::normalize(p-center).cross(DirPoint::normalize(this->ref)).modulus();
        double u;
        if (cosTh >= 0 && sinTh >= 0){
            u = sinTh / 4;
        } else if(cosTh <= 0 && sinTh >=0){
            u = 0.25 - cosTh/4;
        } else if(cosTh <=0 && sinTh <=0){
            u = 0.5 - sinTh/4;
        } else {
            u = 0.75 + cosTh/4;
        }

        u = atan2(sinTh, cosTh) / (2*M_PI);

        //cout << u << " " << v << endl;
        return this->brdf.getBRDF(static_cast<float>(u), static_cast<float>(v));
    } else {
        return this->brdf;
    }
}
