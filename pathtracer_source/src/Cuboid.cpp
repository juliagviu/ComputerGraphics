//
// Created by serizba on 17/11/18.
//

#include "Cuboid.h"


Cuboid::Cuboid(DirPoint vx, DirPoint vy, DirPoint vz, DirPoint p0, BRDF brdf) :
        vx(vx), vy(vy), vz(vz), p0(p0), Figure(brdf)  {

    cb = TMatrix::changeBase(p0,DirPoint::normalize(vx),DirPoint::normalize(vy),DirPoint::normalize(vz));
    cbinv = cb.inverse();

    planes[0] = new Plane(p0, DirPoint::normalize(vz), brdf);
    planes[1] = new Plane(p0, DirPoint::normalize(vy), brdf);
    planes[2] = new Plane(p0, DirPoint::normalize(vx), brdf);
    DirPoint p1 = ((p0 + vx) + vy) + vz;
    planes[3] = new Plane(p1, DirPoint::normalize(vz), brdf);
    planes[4] = new Plane(p1, DirPoint::normalize(vy), brdf);
    planes[5] = new Plane(p1, DirPoint::normalize(vx), brdf);

}

tuple<double, TMatrix> Cuboid::intersect(DirPoint o, DirPoint p) {
    DirPoint v = DirPoint::normalize(p-o);

    double mindist = numeric_limits<double>::max();
    TMatrix lb;
    for (int i=0; i<6; i++){
        Plane *pp = planes[i];
    //for (Plane* pp: planes) {
        auto t = pp->intersect(o,p);

        if(get<0>(t)<mindist && get<0>(t)>epsilon) {
            // Point of intersection (global base)
            DirPoint pi = o + (get<0>(t)*v);

            pi = cbinv * pi;
            // Check if pi in the cuboid
            //cout << "chii" << endl;
            if (pi[0]<=vx.modulus()+epsilon && pi[0]>=-epsilon && pi[1]<=vy.modulus()+epsilon && pi[1]>=-epsilon && pi[2]<=vz.modulus()+epsilon && pi[2]>=-epsilon){
                // pi is inside the cuboid

                mindist = get<0>(t);
                lb = get<1>(t);
            }
        }
        // pi is outside (no intersection)
    }
    return {mindist, lb};
}

BRDF Cuboid::getBRDF(DirPoint p) const {
    return this->brdf;
}

