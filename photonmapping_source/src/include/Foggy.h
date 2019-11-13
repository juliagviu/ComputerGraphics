//
// Created by serizba on 18/01/19.
//

#ifndef SMALLPM_FOGGY_H
#define SMALLPM_FOGGY_H


#include "Vector3.h"
#include <cmath>
#include "Ray.h"
#include "Intersection.h"
#include "Sphere.h"
#include "Random.h"
#include "BSDF.h"
#include "PhotonMapping.h"
#include <iostream>

using namespace std;


struct Photon;

class Foggy {
    // coefficients, distance ray marching
    Real sigma_s, sigma_t, d_rm;
    Vector3 Lambi;
    bool ambient;



public:
    Foggy(const Real &sigma_s, const Real &sigma_t, const Vector3 &Lambi);
    Foggy(const Real &sigma_s, const Real &sigma_t, const Real &d_rm);

    Vector3 homoAmbient(Real d, Vector3 L);
    bool fog_photonstrace(Ray &r, Intersection &it, Vector3 &e, Random *rand);
    Vector3 fog_atenuation(Real distance, Vector3 L);
    Vector3 fog_raymarching(const Vector3& o, const Vector3& d, Real param, const KDTree<Photon, 3>& vm, int num_photons, const vector<unsigned int>& photons_shot);
    bool isAmbient();


};


class Fog : public BSDF {
public:
    //Diffuse coefficient
    Vector3 next_ray;
    Real albedo;

    Fog(const Vector3 &next_ray, float albedo) :  BSDF(nullptr), next_ray(next_ray), albedo(albedo) {}

    void get_outgoing_sample_ray(const Intersection &it, Ray &r, Real &pdf)const;
    Vector3 get_albedo(const Intersection &it) const;
    Real get_specular(const Intersection &it) const;
    bool is_delta()const;
};

#endif //SMALLPM_FOGGY_H
