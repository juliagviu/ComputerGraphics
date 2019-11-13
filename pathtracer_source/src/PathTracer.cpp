//
// Created by serizba on 13/11/18.
//

#include <algorithm>
#include "PathTracer.h"

PathTracer::PathTracer(DirPoint o, DirPoint f, DirPoint l, DirPoint u, vector<Figure *>& scene, vector<PuntualLight>& lights) :
        o(o), f(f), l(l), u(u), scene(scene), rd(), mt(rd()) , gen01(0,1), lights(lights) {
/*    if (abs(((float)w)/h - l.modulus()/u.modulus()) > epsilon) {
        cerr << "You are stupid (resolution does not match)" << endl;
        cerr << ((float)w)/h << endl;
        cerr << l.modulus() / u.modulus() << endl;
    }*/
    cbmat = TMatrix::changeBase(o,DirPoint::normalize(f),DirPoint::normalize(l),DirPoint::normalize(u));
}

// w is from the point to outside
DirPoint PathTracer::flipNormal(DirPoint n, DirPoint w) const {
    if (n*w<0) {
        return -1*n;
    }
    return n;
}

// p1 and p2 are points that define a ray p1->p2
tuple<double, BRDF, TMatrix> PathTracer::calculateIntersection(DirPoint p1, DirPoint p2) const {
    double di, dr; TMatrix ti, tr; Figure* fr;
    dr = numeric_limits<double>::max();
    fr = nullptr;

    const double eps = epsilon;
    for (auto f : this->scene) {
        tie(di,ti) = f->intersect(p1, p2);
        if (di < dr && di > eps) {
            dr = di; tr = ti; fr = f;
        }
    }
    if (fr) { return {dr, fr->getBRDF(tr[3]), tr};}
    else { return {dr, {}, tr};}
}

// sin acos
tuple<DirPoint, DirPoint> PathTracer::phong(BRDF brdf, TMatrix lb, DirPoint wo) {
    double costh = sqrt(1-gen01(mt));
    double phi = 2*M_PI*gen01(mt);
    double sinth = sqrt(1-costh*costh);
    // next ray direction
    DirPoint wn(sinth*cos(phi), sinth * sin(phi), costh, 0);
    // change to global base
    DirPoint n = flipNormal(lb[2], wo);
    wn = DirPoint::normalize(TMatrix::changeBase(lb[3], lb[0], lb[1], n)*wn);
    DirPoint wr = DirPoint::normalize(2*n*(wn*n)-wn);
    double cosThr = wo*wr;
    DirPoint res = brdf.Kd+brdf.Ks*((brdf.alfa+2)/2)*pow(abs(cosThr),brdf.alfa);
    return {res,wn};
}

tuple<DirPoint, DirPoint> PathTracer::perfectSpecular(DirPoint Ksp, TMatrix lb, DirPoint wo) const {
    // all calculations are in global base
    // next ray direction
    DirPoint n = flipNormal(lb[2], wo);
    DirPoint wi = DirPoint::normalize(2*n*(wo*n)-wo);
    //DirPoint res = Ksp*(wi.cross(n)).modulus() * (1.0/M_PI);
    DirPoint res = Ksp; // sin seno ni 2
    return {res,wi};
}

tuple<DirPoint, DirPoint> PathTracer::fresnel(BRDF brdf, TMatrix lb, DirPoint wo) {
    wo = -1*wo; // pointing to the intersection point
    DirPoint n = lb[2];
    double n1,n2;
    if (n*wo > 0) {
        // is going out of the volume
        n1 = brdf.nin;
        n2 = brdf.nout;
        n = -1*n;
    } else {
        // entering the volume
        n1 = brdf.nout;
        n2 = brdf.nin;
    }
    double r = n1/n2;
    double cosTh1 = abs(wo*n);
    //double sinTh2 = r * sqrt(max(0.0, 1-cosTh1 * cosTh1));
    double sinTh2 = r * sqrt(1-cosTh1 * cosTh1);
    DirPoint ones(1,1,1);
    if (sinTh2<1) {
        // calculate Fresnel ratio
        //double cosTh2 = sqrt(max(0.0, 1 - sinTh2 * sinTh2));
        double cosTh2 = sqrt(1 - sinTh2 * sinTh2);
        double Fr1 = (n2*cosTh1 - n1*cosTh2)/(n2*cosTh1 + n1*cosTh2);
        double Fr2 = (n1*cosTh2 - n2*cosTh1)/(n1*cosTh2 + n2*cosTh1);
        // ratio of reflection
        double Fr = 0.5*(Fr1*Fr1+Fr2*Fr2);
        // Fresnel's Roussian Roulette
        //cout << Fr1 << " " << Fr2 << endl;
        if (gen01(mt)<=Fr) {
            return perfectSpecular(ones*Fr, lb, -1*wo);
        } else {
            double c2 = 1 - r*r * (1 - cosTh1*cosTh1);
            DirPoint wi = DirPoint::normalize(r * wo + (r * cosTh1 - sqrt(c2)) * n);
            //return make_tuple(ones*(1-Fr)*(wi.cross(n)).modulus()*(1/M_PI), wi);
            return make_tuple(ones*(1-Fr), wi); // quitar seno y 2
            //return make_tuple(ones, wi); // quitar all
        }
    } else {
        // Total reflection
        return perfectSpecular(ones, lb, -1*wo);
    }
}


// DEPENDIENDO DE LA LUZ
DirPoint PathTracer::nextEventEstimation(BRDF brdf, TMatrix lb, DirPoint wo) const {
    DirPoint res;
    for (PuntualLight pl: lights) {
       auto t = calculateIntersection(lb[3], pl.c);
       // if intersection point is further than light then there is no intersection
       if (get<0>(t) == numeric_limits<double>::max() || get<0>(t) >= (pl.c-lb[3]).modulus()) {
           DirPoint wi = pl.c - lb[3];
           DirPoint n = flipNormal(lb[2], wo);
           if (DirPoint::normalize(wi)*n>=0.0) {
               // positive cos
               DirPoint wr = DirPoint::normalize(2*n*(DirPoint::normalize(wi)*n)-DirPoint::normalize(wi));
               double cosThr = wo*wr;
               res = res + (pl.brdf.e*(1/pow(wi.modulus(),2))).mult(brdf.Kd*(1/M_PI) + brdf.Ks*((brdf.alfa+2)/(2*M_PI))*pow(abs(cosThr),brdf.alfa)) * abs(n*DirPoint::normalize(wi));
            }
       }
    }
    // TODO: ruleta rusa con las luces
    return res;
}


DirPoint PathTracer::calculatePath(DirPoint px) {
    DirPoint p1 = this->o;
    DirPoint p2 = px;
    bool endPath = false;
    DirPoint Li(1,1,1);
    vector<DirPoint> throughput;
    vector<DirPoint> nees;

    int phongs = 0;

    while (!endPath) {
        auto t = calculateIntersection(p1, p2);
        tuple<DirPoint,DirPoint> res;

        // russian roulette
        switch(get<1>(t).russianRoulette(gen01(mt))) {
            case 1: { // light
                res = make_tuple(get<1>(t).e,DirPoint(0,0,0,0));
                endPath = true;
                break;
            }
            case 2: { // phong
                res = phong(get<1>(t), get<2>(t), DirPoint::normalize(p1-p2));
                nees.push_back(nextEventEstimation(get<1>(t), get<2>(t), DirPoint::normalize(p1-p2)));
                phongs++;
                break;
            }
            case 3: { // perfect specular
                res = perfectSpecular(get<1>(t).Ksp, get<2>(t), DirPoint::normalize(p1-p2));
                //nees.push_back(nextEventEstimation(get<1>(t), get<2>(t), DirPoint::normalize(p1-p2)));
                break;
            }
            case 4: { // refraction and reflection (Fresnel)
                res = fresnel(get<1>(t), get<2>(t), DirPoint::normalize(p1-p2));
                //nees.push_back(nextEventEstimation(get<1>(t), get<2>(t), DirPoint::normalize(p1-p2)));
                break;
            }
            default: { // absorption
                res = {DirPoint(0,0,0), DirPoint(0,0,0)};
                endPath = true;
                break;
            }
        }
        // p1 is the last intersection point
        p1 = get<2>(t)[3];
        // p2 is a point in the next ray
        p2 = p1 + get<1>(res);
        throughput.push_back(get<0>(res));
    }

    Li = throughput[throughput.size()-1];
    for (int i=nees.size()-1; i>=0; i--) {
        Li = throughput[i].mult(Li) + nees[i];
    }

    return Li;
}

Image PathTracer::render(unsigned int w, unsigned int h, unsigned int rpp) {
    auto begin = std::chrono::steady_clock::now();
    Image img(w, h);
    img.fillPixelPoints(this->f.modulus(), this->u.modulus(), this->l.modulus(), this->cbmat);
    auto calculatePixel = [this, rpp, h, w] (DirPoint p) {
        DirPoint res;
        for(int i=0; i<rpp; i++) {res = res + this->calculatePath(DirPoint(p[0],p[1] - gen01(mt)*2*l.modulus()/h, p[2] - gen01(mt)*2*u.modulus()/w, 1));}
        return res;
    };
    cout << "Start Pathing!" << endl;
#ifdef MY_DEBUG
    cout << "Debug mode!"  << endl;
    transform(img.img.begin(), img.img.end(), img.img.begin(), calculatePixel);
#else
    transform(execution::par, img.img.begin(), img.img.end(), img.img.begin(), calculatePixel);
#endif
    cout << "Paths done!" << endl;
    auto end= std::chrono::steady_clock::now();
    std::cout << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0 <<std::endl;
    return img;

}

