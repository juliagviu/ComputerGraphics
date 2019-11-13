/*********************************************************************************
Copyright (C) 2014 Adrian Jarabo (ajarabo@unizar.es)
Copyright (C) 2014 Diego Gutierrez (diegog@unizar.es)
All rights reserved.

This is an educational Ray Tracer developed for the course 'Informatica Grafica'
(Computer Graphics) tought at Universidad de Zaragoza (Spain). As such, it does not 
intend to be fast or general, but just to provide an educational tool for undergraduate
students. 

This software is provided as is, and any express or implied warranties are disclaimed.
In no event shall copyright holders be liable for any damage.
**********************************************************************************/
//#include <include/Lambertian.h>
//#include <include/Phong.h>
#include <numeric>
#include <random>
#include <PhotonMapping.h>
#include <include/Plane.h>
#include <Film.h>

#include "PhotonMapping.h"
#include "World.h"
#include "Intersection.h"
#include "Ray.h"
#include "BSDF.h"

//*********************************************************************
// Compute the photons by tracing the Ray 'r' from the light source
// through the scene, and by storing the intersections with matter
// in the lists 'xx_photons', storing the diffuse (global) and caustic
// photons respectively. For efficiency, both are computed at the same
// time, since computing them separately would result into a lost of
// several samples marked as caustic or diffuse.
// Same goes with the boolean 'direct', that specifies if direct 
// photons (from light to surface) are being stored or not. 
// The initial traced photon has energy defined by the tristimulus
// 'p', that accounts for the emitted power of the light source.
// The function will return true when there are more photons (caustic
// or diffuse) to be shot, and false otherwise.
//---------------------------------------------------------------------
bool PhotonMapping::trace_ray(const Ray& r, const Vector3 &p,
							  std::list<Photon> &global_photons, std::list<Photon> &caustic_photons, std::list<Photon> &volume_photons, bool direct, int light)
{

	//Check if max number of shots done...
	if( ++m_nb_current_shots > m_max_nb_shots )
	{
		return false;
	}

	// Compute irradiance photon's energy
	Vector3 energy(p);

	Ray photon_ray(r);
	photon_ray.shift();



	bool is_caustic_particle = false;
	bool is_fog_particle = false;
	int im_in = 0;

	int i = 0;
	//Iterate the path
	while(1)
	{

		if (im_in > 1) {
			im_in = 0;
		}
		is_fog_particle = false;
		// Throw ray and update current_it
		Intersection it;
		world->first_intersection(photon_ray, it);


		if( !it.did_hit() ) {
			break;
		}

		if (this->foggy && im_in == 0)
			is_fog_particle = foggy->fog_photonstrace(photon_ray, it, energy, this->random);

		if( i==0 && caustic_photons.size()<m_nb_caustic_photons) {
            photons_shot_caustic[light] = photons_shot_caustic[light] + 1;
		}
		if( i==0 && global_photons.size()<m_nb_global_photons){
			// A photon has been shot from light and will be stored if needed
			photons_shot_global[light] = photons_shot_global[light] + 1;
		}
		if( i==0 && volume_photons.size()<m_nb_volume_photons){
			// A photon has been shot from light and will be stored if needed
			photons_shot_volume[light] = photons_shot_volume[light] + 1;
		}
		i++;

		//Check if has hit a delta material...
		if( it.intersected()->material()->is_delta() )
		{
			// If delta material, then is caustic...
			// Don't store the photon!
			is_caustic_particle = true;
			im_in++;
		}
		else if (photon_ray.get_level() > 0 || direct || is_fog_particle) // fog is stored always, even direct ones
		{
            //If non-delta material, store the photon!
            if( is_fog_particle )
            {
                //If fog particle, store in volume
                if( volume_photons.size() < m_nb_volume_photons )
                    //cout << it.get_position()[0] << " " << it.get_position()[1] << " " << it.get_position()[2] << endl;
                    volume_photons.push_back( Photon(it.get_position(), photon_ray.get_direction(), energy, light ));
            }
			else if( is_caustic_particle )
			{
				//If caustic particle, store in caustics
				if( caustic_photons.size() < m_nb_caustic_photons )
					caustic_photons.push_back( Photon(it.get_position(), photon_ray.get_direction(), energy, light ));
				is_caustic_particle = false;
			}

			else
			{
				//If non-caustic particle, store in global
				if( global_photons.size() < m_nb_global_photons )
					global_photons.push_back( Photon(it.get_position(), photon_ray.get_direction(), energy, light ));
				is_caustic_particle = false;
			}
			//is_caustic_particle = false;

		}

		Real pdf;

		Vector3 surf_albedo = it.intersected()->material()->get_albedo(it);
		Real avg_surf_albedo = surf_albedo.avg();

		Real epsilon2 = static_cast<Real>(rand())/static_cast<Real>(RAND_MAX);
		while (epsilon2 < 0.)
			epsilon2 = static_cast<Real>(rand())/static_cast<Real>(RAND_MAX);

		if (epsilon2 > avg_surf_albedo || photon_ray.get_level() > 20 )
			break;




		// Random walk's next step
		// Get sampled direction plus pdf, and update attenuation
		it.intersected()->material()->get_outgoing_sample_ray(it, photon_ray, pdf );
		photon_ray.shift();

		// Shade...
		energy = energy*surf_albedo;
		if( !it.intersected()->material()->is_delta() ) {
			if( !is_fog_particle ) {
				energy *= dot_abs(it.get_normal(), photon_ray.get_direction()) / 3.14159;
			}
		}

		energy = energy /(pdf*avg_surf_albedo);
	}

	if( caustic_photons.size() == m_nb_caustic_photons &&
		global_photons.size() == m_nb_global_photons && volume_photons.size() == m_nb_volume_photons)
	{
		m_max_nb_shots = m_nb_current_shots-1;
		return false;
	}

	return true;
}


//*********************************************************************
// Preprocess step of photon mapping, where the photons are traced
// through the scene. It follows these steps:
//  1 - Sample a world's light source in the scene to create
//		the initial direct photon from the light source.
//	2 - Trace the photon through the scene storing the inter-
//		sections between the photons and matter (trace_ray function)
//	3 - Finally, once all the photons have been shot, build
//  	the photon maps, that will be used later
//		for rendering.
//---------------------------------------------------------------------
void PhotonMapping::preprocess() {
	cout << this->world->light(0).get_no_samples() << endl;
	list<Photon> cl, gl, vl;
    Vector3 d;
    int l;
	photons_shot_global.resize(this->world->nb_lights(),0);
    photons_shot_caustic.resize(this->world->nb_lights(),0);
	photons_shot_volume.resize(this->world->nb_lights(),0);
	do {
        //d = this->random->solid_angle();
		d = this->random->cubo();
        l = this->random->get_russian_light();
	} while(trace_ray(Ray(this->world->light(l).get_position(), d, 0), this->world->light(l).get_intensities() * 4*(M_PI), gl, cl, vl, false, l));

	plotKD(vl);

	for (Photon p : gl) {this->m_global_map.store({p.position[0], p.position[1], p.position[2]}, p);}
	for (Photon p : cl) {this->m_caustics_map.store({p.position[0], p.position[1], p.position[2]}, p);}
	for (Photon p : vl) {this->m_volume_map.store({p.position[0], p.position[1], p.position[2]}, p);}

	if (!gl.empty()) {
		this->m_global_map.balance();
		cout << "Global: " << this->m_global_map.nb_elements();
	}
	if (!cl.empty()) {
		this->m_caustics_map.balance();
		cout << "Caustics: " << this->m_caustics_map.nb_elements();
	}
	if (!vl.empty()) {
		this->m_volume_map.balance();
		cout << "Volume: " << this->m_volume_map.nb_elements();
	}
}

//*********************************************************************
// Computes the rendering equation
// using radiance estimation with photon mapping, using the photon
// maps computed as a proprocess. Note that you will need to handle
// both direct and global illumination, together with recursive the
// recursive evaluation of delta materials. For an optimal implemen-
// tation you should be able to do it iteratively.
// In principle, the class is prepared to perform radiance estimation
// using k-nearest neighbors ('m_nb_photons') to define the bandwidth
// of the kernel.
//---------------------------------------------------------------------
Vector3 PhotonMapping::shade(Ray& ray, Intersection &it0)
{

	Vector3 L(0);
	Intersection it(it0);

	// Find first intersection that is not delta
	int b = 0; // Number of bounces
	Real distance = ray.get_parameter();
	if (this->foggy) {
		L += this->foggy->fog_raymarching(ray.get_origin(), ray.get_direction(), ray.get_parameter(), m_volume_map,
										  m_nb_photons, photons_shot_volume);
	}

	bool delta_flag = false;

	while (it.did_hit() && it.intersected()->material()->is_delta() && b++ < 20) {
		Real pdf;
		it.intersected()->material()->get_outgoing_sample_ray(it, ray, pdf);
		ray.shift();
		world->first_intersection(ray, it);
		delta_flag = true;
/*		distance += ray.get_parameter();
		if(this->foggy && !this->foggy->isAmbient()) {
			L+=this->foggy->fog_raymarching(ray.get_origin(), ray.get_direction(), ray.get_parameter(), m_volume_map, m_nb_photons, photons_shot_volume);
		}*/
	}

	if(delta_flag && this->foggy && !this->foggy->isAmbient()) {
		distance += ray.get_parameter();
		L+=this->foggy->fog_raymarching(ray.get_origin(), ray.get_direction(), ray.get_parameter(), m_volume_map, m_nb_photons, photons_shot_volume);
	}

/*	if (it.did_hit() && it.intersected()->material()->is_delta()) {
		Real pdf;
		it.intersected()->material()->get_outgoing_sample_ray(it, ray, pdf);
		world->first_intersection(ray, it);
	}*/

/*	while( it.intersected()->material()->is_delta() && ++b < 20)
	{
		Ray r; float pdf;
		it.intersected()->material()->get_outgoing_sample_ray(it, r, pdf );
		//W = W * it.intersected()->material()->get_albedo(it)/pdf;

		r.shift();
		world->first_intersection(r, it);
	}*/

	if (it.did_hit() && !it.intersected()->material()->is_delta()) {
		vector<const KDTree<Photon, 3>::Node *> lc;
		vector<const KDTree<Photon, 3>::Node *> lg;
		Real maxg, maxc;
		Vector3 Lg(0, 0, 0), Lc(0, 0, 0), Ld;
		vector<Real> position = {it.get_position()[0], it.get_position()[1], it.get_position()[2]};


		Lg = estimate(it.get_position(), this->m_global_map, this->m_nb_photons, this->photons_shot_global);
		Lg = Lg * (it.intersected()->material()->get_albedo(it) / M_PI);


		Lc = estimate(it.get_position(), this->m_caustics_map, this->m_nb_photons, this->photons_shot_caustic);
		Lc = Lc * (it.intersected()->material()->get_albedo(it) / M_PI);

		// Add direct light
		int l = this->random->get_russian_light(); //roussian roulette with lights
		if (world->light(l).is_visible(it.get_position())) {
			Ld = world->light(l).get_incoming_light(it.get_position()) * (it.intersected()->material()->get_albedo(it) / M_PI) * dot_abs(it.get_normal(), (world->light(l).get_position()-it.get_position()).normalize());
			if (this->foggy) {
				Vector3 dirn = it.get_position() - world->light(l).get_position();
				Vector3 dir = dirn;
				dirn.normalize();
				Vector3 Ld1 = this->foggy->fog_raymarching(world->light(l).get_position(), dirn, dir.length(), m_volume_map, m_nb_photons, photons_shot_volume);
				Vector3 Ld2 = this->foggy->fog_atenuation(dir.length(), Ld);
				Ld =  Ld1+Ld2;
			}
		}

		//cout << "Indirect:" << (Lc+Lg)[0] << ", " << (Lc+Lg)[1] << ", " << (Lc+Lg)[2] << " --- Direct: " << Ld[0] << ", " << Ld[1] << "," << Ld[2] << endl;
		// Add volume light from participating media
		if (this->foggy) {
			return L + this->foggy->fog_atenuation(distance, Lg + Lc + Ld);
		} else {
			//cout << "Lg: " << Lg[0] << " " << Lg[1] << " " << Lg[2] << endl;
			//cout << "Ld: " << Ld[0] << " " << Ld[1] << " " << Ld[2] << endl;
			//cout << Ld.avg() / (Lc + Lg).avg() << endl;

			Denergy += Ld.avg();
			Ienergy += (Lc + Lg).avg();
			return  Ld + Lc + Lg;
		}
	} else {
		return Vector3(0,0,0);
	}
}




Vector3 PhotonMapping::estimate(Vector3 x, const KDTree<Photon, 3> &tree, int num_photons,
                                const vector<unsigned int> &photons_shot) const {

    Vector3 L(0);
    vector<Real> position = {x[0], x[1], x[2]};
    vector<const KDTree<Photon, 3>::Node *> l;
    Real max;
    if (!tree.is_empty()) {
        tree.find(position, num_photons, l, max);
        switch (this->kernel) {
            case 1: {
				// Normal
				L = accumulate(l.begin(), l.end(), Vector3(0, 0, 0), [photons_shot](Vector3 v, auto p) {
					return v + p->data().flux / photons_shot[p->data().light];
				});
				L = L * (1.0 / (M_PI * max * max));
				break;
			}
            case 2: {
				// Cone
				Real k = 2.0;
				L = accumulate(l.begin(), l.end(), Vector3(0, 0, 0), [photons_shot, x, max, k](Vector3 v, auto p) {
					return v + (1.0 - (p->data().position - x).length() / (max * k)) *
							   (p->data().flux / photons_shot[p->data().light]);
				});
				L = L * (1.0 / ((1 - 2.0 / (3.0 * k)) * M_PI * max * max));
				break;
			}
        	case 3: {
				// Gaussian
				Real alfa = 0.918;
				Real beta = 1.953;
				L = accumulate(l.begin(), l.end(), Vector3(0, 0, 0),
							   [photons_shot, x, max, alfa, beta](Vector3 v, auto p) {
								   return v + alfa * (1.0 - (1 - exp(-beta * ((p->data().position - x).length() *
																			  (p->data().position - x).length()) /
																	 (2 * max * max))) / (1 - exp(-beta))) *
											  (p->data().flux / photons_shot[p->data().light]);
							   });
				L = L * (1.0 / (M_PI * max * max));
				break;
			}
			case 4: {
				//Carapanov
				L = accumulate(l.begin(), l.end(), Vector3(0, 0, 0), [photons_shot, x, max](Vector3 v, auto p) {
					return v + (1 - pow((p->data().position - x).length()/max, 2.0)) * (p->data().flux / photons_shot[p->data().light]);
				});
				L = L * (2.0 / (M_PI * max * max));
				break;
			}
			case 5: {
				//Silverman
				L = accumulate(l.begin(), l.end(), Vector3(0, 0, 0), [photons_shot, x, max](Vector3 v, auto p) {
					return v + pow((1 - pow((p->data().position - x).length()/max, 2.0)), 2.0) * (p->data().flux / photons_shot[p->data().light]);
				});
				L = L * (3.0 / (M_PI * max * max));
				break;
			}
        }

    }

    return L;
}


void PhotonMapping::plotKD(list<Photon> l) {
    Triangle* t1 = new Triangle(Vector3(1.5,2.5,0), Vector3(-1.5,2.5,0),
                                    Vector3(-1.5,-.5,0), nullptr);
    Triangle* t2 = new Triangle(Vector3(1.5,2.5,0), Vector3(-1.5,-.5,0),
                                    Vector3(1.5,-.5,0), nullptr);

    Film* film = new Film(500, 500);
	for (auto p : l) {
		Ray r1(Vector3(0, 1, 3), (p.position-Vector3(0, 1, 3)).normalize(), 0);
		Ray r2(Vector3(0, 1, 3), (p.position-Vector3(0, 1, 3)).normalize(), 0);
		Intersection it1, it2;
		t1->intersect(r1, it1);
		t2->intersect(r2, it2);
		int w,h;
		if (it1.did_hit()) {
			w = static_cast<int>(500*((it1.get_position()[0] - (-1.5)) / 3.0));
			h = static_cast<int>(500*((it1.get_position()[1] - (-0.5)) / 3.0));
			film->draw_pixel(w, h, p.flux);
		} else if (it2.did_hit()) {
			w = static_cast<int>(500*((it2.get_position()[0] - (-1.5)) / 3.0));
			h = static_cast<int>(500*((it2.get_position()[1] - (-0.5)) / 3.0));
			film->draw_pixel(w, h, p.flux);
		}

	}

	film->write("maps-v3.ppm");

}