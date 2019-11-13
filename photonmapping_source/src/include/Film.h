//
// Created by serizba on 27/12/18.
//

#ifndef SMALLPM_FILM_H
#define SMALLPM_FILM_H



#include "globals.h"
#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

using namespace std;

// A rasterized simple Film in HDR
class Film {


private:
    /// Actual Bitmap containing image
    vector<Vector3> img;

    /// Width and height of Film
    int width, height;

    ///Random stuff
    uniform_real_distribution<float> distw;
    uniform_real_distribution<float> disth;
    default_random_engine gen;

/*    /// Half width and height of Film.
    Real xw2,yw2;

    Real _min, _max;*/

    void equacutre();
    void reinhard();

public:
    // Constructs a Film with specified dimensions
    Film(int w, int h);
    ~Film();

    // Returns width of Film
    int get_width() const;

    // Returns height of Film
    int get_height() const;

    // 2D transform from window coordinates to image coordinates.
    // Image coordinates are in the [-1,1]x[-1,1] range
    const Vector2 window_coords2image_coords(const Vector2& p);
    // Draw a pixel to the Film.
    void draw_pixel(int x, int y, const Vector3& cv);

    // Store the image into an HDR
    void write(const char* name);
    void writeHDR(const string &fileName);
};


#endif //SMALLPM_FILM_H
