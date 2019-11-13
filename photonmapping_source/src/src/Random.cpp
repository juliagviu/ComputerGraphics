//
// Created by serizba on 28/12/18.
//

#include <iostream>

#include "Random.h"

Random::Random(World* world) {
    random_device rd;
    gen = new mt19937(rd());
    dist_unsigned = new uniform_real_distribution<Real>(0.f, 1.f);
    dist_signed = new uniform_real_distribution<Real>(-1.f, 1.f);

    int l = world->nb_lights();
    // Vector of probabilities of each light
    Real sum = 0.f;
    for (int i=0; i<l; i++) {
        Real p = world->light(i).get_intensities().avg();
        probs.push_back(p);
        sum += p;
    }
    probs[0] = probs[0] / sum;
    for (int i=1; i<l; i++) {probs[i] = probs[i-1] + probs[i]/sum;}
}

Real Random::get_signed() {
    return (*dist_signed)(*gen);
}

Real Random::get_unsigned() {
    return (*dist_unsigned)(*gen);
}

int Random::get_russian_light() {
    Real u = get_unsigned();
    for (int i=0; i<probs.size(); i++) {
        if (u <= probs[i]) { return i; }
    }
    return -1;
}

Vector3 Random::cubo() {
    Vector3 d;
    do {
        d = Vector3(this->get_signed(), this->get_signed(), this->get_signed());
    } while (d.length()>1.0);
    return d;
}

Vector3 Random::solid_angle() {
    Real sinTh = this->get_signed();
    Real phi = this->get_unsigned() * 2 * M_PI;
    return Vector3(sinTh * cos(phi), sinTh * sin(phi), sqrt(1-sinTh*sinTh));
}
