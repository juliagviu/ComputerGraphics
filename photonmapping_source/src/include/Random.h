//
// Created by serizba on 28/12/18.
//

#ifndef SMALLPM_RANDOM_H
#define SMALLPM_RANDOM_H

#include <random>
#include "World.h"

using namespace std;

class Random {
    /// Russian roulette random stuff
    mt19937* gen;
    uniform_real_distribution<Real>* dist_signed;
    uniform_real_distribution<Real>* dist_unsigned;
    vector<Real> probs;

public:
    Random(World* world);

    Real get_unsigned();
    Real get_signed();

    Vector3 cubo();
    Vector3 solid_angle();
    
    // Get a light with russian roulette
    int get_russian_light();


};


#endif //SMALLPM_RANDOM_H
