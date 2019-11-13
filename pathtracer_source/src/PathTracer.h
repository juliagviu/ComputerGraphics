//
// Created by serizba on 13/11/18.
//

#ifndef GRAFICA_RAYTRACER_H
#define GRAFICA_RAYTRACER_H


#include <iostream>
#include <array>
#include <cmath>
#include <tuple>
#include <limits>
#include <vector>
#include <queue>
#ifndef MY_DEBUG
#include <pstl/algorithm>
#include <pstl/execution>
#endif

#include <random>
#include <mutex>
#include "DirPoint.h"
#include "Figure.h"
#include "PuntualLight.h"

using namespace std;

// CONSTANTS VALUES

class PathTracer {
private:
    DirPoint o;
    DirPoint f,l,u;
    TMatrix cbmat;              // Calculated change base matrix to avoid wasting time
    vector<Figure*> scene;
    vector<PuntualLight> lights;
    // random stuff
    random_device rd;
    uniform_real_distribution<double> ldist;
    uniform_real_distribution<double> udist;
    uniform_real_distribution<double> gen01;
    mt19937 mt;
    const float epsilon = 0.01; // to avoid numerical errors (intersection to yourself)

public:
    PathTracer(DirPoint o, DirPoint f, DirPoint l, DirPoint u, vector<Figure *>& scene, vector<PuntualLight>& lights);
    Image render(unsigned int w, unsigned int h, unsigned int rpp);

private:
    tuple<double, BRDF, TMatrix> calculateIntersection(DirPoint p1, DirPoint p2) const;
    DirPoint nextEventEstimation(BRDF brdf, TMatrix lb, DirPoint wo) const;
    tuple<DirPoint, DirPoint> phong(BRDF brdf, TMatrix lb, DirPoint wo);
    tuple<DirPoint, DirPoint> perfectSpecular(DirPoint Ksp, TMatrix lb, DirPoint wo) const;
    tuple<DirPoint, DirPoint> fresnel(BRDF brdf, TMatrix lb, DirPoint wo);
    DirPoint calculatePath(DirPoint px);
    DirPoint flipNormal(DirPoint n, DirPoint w) const;

};


#endif //GRAFICA_RAYTRACER_H
