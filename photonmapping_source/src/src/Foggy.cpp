//
// Created by serizba on 18/01/19.
//


#include <Foggy.h>

#include "Foggy.h"

Foggy::Foggy(const Real &sigma_s, const Real &sigma_t, const Vector3 &Lambi) : sigma_s(sigma_s), sigma_t(sigma_t),
                                                                                     Lambi(Lambi), ambient(true) {}

Vector3 Foggy::homoAmbient(Real d, Vector3 L) {
    Real esot = exp(-d * sigma_t);
    return sigma_s * Lambi * ((1. - esot)/sigma_t) + esot * L;
}

/*
bool Foggy::fog_photonstrace(Ray &r, Intersection &it, Vector3 &e, Random *rand) {
    if (this->ambient) {
        e = homoAmbient(r.get_parameter(), e);
        return false;
    }


    Vector3 x =  r.get_origin() + d_rm * r.get_direction();
    Real travelled = d_rm;
    Real fx = 1 - exp(-d_rm * sigma_t);
    while (fx <= rand->get_unsigned() && travelled < r.get_parameter()) {
        x = x + d_rm * r.get_direction();
        travelled += d_rm;
        fx = 1 - exp(-d_rm * sigma_t);
    }
    if (travelled < r.get_parameter()) {
        // Interaction
        //cout << x[0] << " " << x[1] << " " << x[2] << endl;
        Ray rr = Ray(r.get_origin(), r.get_direction(), r.get_level());
        rr.cond_set_parameter(travelled);
        it = Intersection(rr, new Sphere(x, 1, new Fog(rand->cubo(), sigma_s/sigma_t)), Vector3(), Vector2());
        return true;
    }
    return false;
}
*/

bool Foggy::fog_photonstrace(Ray &r, Intersection &it, Vector3 &e, Random *rand) {
    if (this->ambient) {
        e = homoAmbient(r.get_parameter(), e);
        return false;
    }

    Real d = rand->get_unsigned() * r.get_parameter();
    //more fancy d
    //Real d = -1*log(rand->get_unsigned())/sigma_t;
    //if(d > r.get_parameter()){
    //    return false;
    //}
    if (1 - exp(-d * sigma_t) > rand->get_unsigned()) {
        Ray rr = Ray(r.get_origin(), r.get_direction(), r.get_level());
        rr.cond_set_parameter(d);
        //rr.shift();
        it = Intersection(rr, new Sphere({}, 1, new Fog(rand->cubo(), sigma_s/sigma_t)), Vector3(), Vector2());
        return true;
    } else {
        return false;
    }
}



Vector3 Foggy::fog_raymarching(const Vector3& o, const Vector3& d, Real param, const KDTree<Photon, 3>& vm, int num_photons, const vector<unsigned int>& photons_shot) {
    if (this->ambient) {
        return Vector3(0,0,0);
    }

    Vector3 Ld, Li;
    Vector3 x = o + d * this->d_rm;
    Real travelled = this->d_rm;
    while (travelled < param) {
        if (!vm.is_empty()) {
            Real maxv;
            vector<const KDTree<Photon, 3>::Node *> lv;
            vector<Real> position = {x[0], x[1], x[2]};
            vm.find(position, num_photons, lv, maxv);
            Li = accumulate(lv.begin(), lv.end(), Vector3(0, 0, 0), [photons_shot] (Vector3 v, auto p) {return v + p->data().flux / photons_shot[p->data().light];});
            Li = Li * (1 / (4 *M_PI)) * (3 / (4 * M_PI * maxv * maxv * maxv));
            travelled += d_rm;
            x = x + d * d_rm;
            Li = Li * (sigma_s/sigma_t);
        }
    
        Ld = Ld*exp(-d_rm*sigma_t) + sigma_s * d_rm * Li;
    }

    return Ld;
}

bool Foggy::isAmbient() {
    return ambient;
}

Vector3 Foggy::fog_atenuation(Real distance, Vector3 L) {
    if (this->ambient){
        return homoAmbient(distance,L);
    }
    return exp(-distance*sigma_t) * L;
}

Foggy::Foggy(const Real &sigma_s, const Real &sigma_t, const Real &d_rm) : ambient(false), sigma_s(sigma_s), sigma_t(sigma_t), d_rm(d_rm) {}

void Fog::get_outgoing_sample_ray(const Intersection &it, Ray &r, Real &pdf) const {
    r = Ray(it.get_position(), this->next_ray, 0);
    pdf = 1;
}

Vector3 Fog::get_albedo(const Intersection &it) const {
    return Vector3(this->albedo, this->albedo, this->albedo);
}

Real Fog::get_specular(const Intersection &it) const {
    return 0;
}

bool Fog::is_delta() const {
    return false;
}
