//
// Created by serizba on 13/11/18.
//

#include "Plane.h"

Plane::Plane(const double &d, const DirPoint &n, const BRDF& brdf) :
        d(d), n(n), Figure(brdf) {
    x = DirPoint::normalize(n.cross(DirPoint(1,1,1,0)));
    y = DirPoint::normalize(n.cross(x));
    p0 = DirPoint(0,0,0,1)+(d*n);
}


Plane::Plane(const double &d, const DirPoint &n) :
d(d), n(n) {
    x = DirPoint::normalize(n.cross(DirPoint(1,1,1,0)));
    y = DirPoint::normalize(n.cross(x));
    p0 = DirPoint(0,0,0,1)+(d*n);
}

Plane::Plane(const double &d, const DirPoint &n, const DirPoint &i, const DirPoint& o, float u, float v, const BRDF &brdf) :
        d(d), n(n), o(o), Figure(brdf) {
    if(!brdf.isTextured()){
        cerr << "The plane is not textured, you are using the wrong constructor";
    }
    tu = DirPoint::normalize(i);
    tv = DirPoint::normalize(n.cross(tu));
    tu = tu*u;
    tv = tv*v;

    x = DirPoint::normalize(n.cross(DirPoint(1,1,1,0)));
    y = DirPoint::normalize(n.cross(x));
    p0 = DirPoint(0,0,0,1)+(d*n);
}

tuple<double, TMatrix> Plane::intersect(DirPoint o, DirPoint p) {
    DirPoint v = DirPoint::normalize(p-o);
    if (v*n==0) {
        //normal and line are perpendicular
        //line is contained or line is parallel to the plane
        return make_tuple(numeric_limits<double>::max(), TMatrix());
    }
    else {
        DirPoint in = (((p0-o)*n)/(v*n))*v + o;
		
		double dist;
		if(DirPoint::normalize(in-o)*v>0){	// positive cos, intersection (0º)
			dist = (in - o).modulus();
		} else { 
			dist = -((in - o).modulus());  // negative cos, intersection is behind the camera (180º)
		}

        // Calculate new local base z = normal
        DirPoint lbn = n;
        // if (acos(n*v)<M_PI/2) {
        //     lbn =  -1*n;
        // }
        return make_tuple(dist,TMatrix::changeBase(in, x, y, lbn));
    }
}

BRDF Plane::getBRDF(DirPoint p) const {
    if(brdf.isTextured()) {
        // calculate u and v
        DirPoint pl = TMatrix::changeBase(o,DirPoint::normalize(tu),DirPoint::normalize(tv),DirPoint::normalize(n)).inverse()*p;
        double u,v;
        double distu = tu.modulus();
        double distv = tv.modulus();
        u = (pl[0]-floor(pl[0]/distu)*distu)/distu;
        v = (pl[1]-floor(pl[1]/distv)*distv)/distv;
        return this->brdf.getBRDF(u,v);

    } else {
        return this->brdf;
    }
}

Plane::Plane(const DirPoint &p0, const DirPoint &n, const BRDF &brdf) {
    double d = n*DirPoint(p0[0],p0[1],p0[2]);
    if(d<0){
        this->n = -1*n;
        this->d = -d;
    } else{
        this->n = n;
        this->d = d;
    }

    this->x = DirPoint::normalize(this->n.cross(DirPoint(1,1,1,0)));
    this->y = DirPoint::normalize(this->n.cross(x));
    this->p0 = DirPoint(0,0,0,1)+(this->d*this->n);

    this->brdf = brdf;

}


Plane::Plane() = default;


