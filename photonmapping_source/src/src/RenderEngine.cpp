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

#include "Timer.h"
#include "RenderEngine.h"
#include "Film.h"
#include <math.h>
#include <iostream>
#include <numeric>

using namespace std;

const Vector3 RenderEngine::trace_ray(const Vector2& pi)
{
	// NOTE: Vector2 'pi' stores the pixel coordinates in the range
	//			pi[0] in [0, film->sizex-1], pi[1] in [0, film->sizey-1]
	Vector2 p = film->window_coords2image_coords(pi);
	Ray r = camera->get_ray(p);

	Intersection it;
	world->first_intersection(r, it);
	
	// Get the ray's first intersection in the scene (if exists)	
	if (it.did_hit())
	{
		return photon_mapping->shade(r, it);
	}
	else
		// ... or return background's color if the ray does not intersect.
		return world->get_background();
}

void RenderEngine::render(const std::string& name)
{
	Timer timer;timer.start();

	cout << "Preparing scene to be rendered...\r";
	world->fix();
	{
		Real secs = timer.get_secs();
		int hours = static_cast<int>(secs)/3600; secs -= hours*3600;
		int minutes = static_cast<int>(secs)/60; secs -= minutes*60;
		std::cout << "Prepared scene to render: \t["<<hours<<":"<<minutes<<":"<<secs<<"]             \n"; 
	}


	// ----------------------------------------------------------------------
	// Precompute...
	cout << "Shooting Photons...\r";
	timer.start();
	photon_mapping->preprocess();
	{
		Real secs = timer.get_secs();
		int hours = static_cast<int>(secs)/3600; secs -= hours*3600;
		int minutes = static_cast<int>(secs)/60; secs -= minutes*60;
		std::cout << "Photons Shot: \t["<<hours<<":"<<minutes<<":"<<secs<<"]             \n"; 
	}
	
	// ----------------------------------------------------------------------
	// Start timer and go ...
	//
	int x=0;
	Real weight = 1./(NB_SAMPLES_ANTIALIASING);
	int sq_samples_aa = sqrtf(static_cast<float>(NB_SAMPLES_ANTIALIASING));

	cout << "Rendering ...\r"; 
	timer.start();

	// ----------------------------------------------------------------------
	//Raytrace all samples in the image

	//for all pixels...
		//trace a ray on each pixel (see function 'RenderEngine::trace_ray()')
/*	for (int y=0;y<film->get_height(); ++y)
	{	
		if(!(y%10))
		{
			Real secs = timer.get_secs();
			int hours = static_cast<int>(secs)/3600; secs -= hours*3600;
			int minutes = static_cast<int>(secs)/60; secs -= minutes*60;
			std::cout << "Rendering ..."
				<<static_cast<Real>(y)/film->get_height()*100<<"%: \t["<<hours<<":"<<minutes<<":"<<secs<<"]             "<<"\r"; 
		}
		for( int x=0; x<film->get_width(); ++x)
		{
			//Get color!
			Vector3 col = trace_ray(Vector2(x,y));

			//Store the RGB data on film
			film->draw_pixel(x,y, col);
		}
	}*/

	vector<unsigned int> positions(this->film->get_width()*this->film->get_height());
	iota(positions.begin(), positions.end(), 0);
	for_each(positions.begin(), positions.end(), [this] (int p) {
	    Vector3 r(0,0,0);
		for (int i=0; i<1; i++) { r += trace_ray(Vector2(p%this->film->get_width(), p/this->film->get_width()));}
		this->film->draw_pixel(p%this->film->get_width(), p/this->film->get_width(), r);
		//cout << p << endl;
	});

	cout << this->photon_mapping->Denergy << endl;
	cout << this->photon_mapping->Ienergy << endl;

/*    vector<unsigned int> positions(32 * this->film->get_width()*this->film->get_height());
    iota(positions.begin(), positions.end(), 0);
    for_each(execution::par, positions.begin(), positions.end(), [this] (int p) {
        p=p/32;
        this->film->draw_pixel(p%this->film->get_width(), p/this->film->get_width(), trace_ray(Vector2(p%this->film->get_width(), p/this->film->get_width())));
    });*/


	// ----------------------------------------------------------------------
	// Rendering finished...
	Real secs = timer.get_secs();
	int hours = static_cast<int>(secs)/3600; secs -= hours*3600;
	int minutes = static_cast<int>(secs)/60; secs -= minutes*60;
	cout << "Rendering ..."
				<<"[DONE]: \t["<<hours<<":"<<minutes<<":"<<secs<<"]                        "<<"\n"; 
	
	
	//..., save the image!
	film->write(name.c_str());
}
