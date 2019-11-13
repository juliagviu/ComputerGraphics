//
// Created by serizba on 29/12/18.
//

#ifndef GRAFICA2_IMAGE_H
#define GRAFICA2_IMAGE_H

#include <random>
#include <vector>
#include <fstream>
#include <algorithm>
#include "DirPoint.h"
#include "TMatrix.h"

using namespace std;

class Image {
public:
    vector<DirPoint> img;
    unsigned int w, h;
public:
    Image(string fileName);
    Image(bool kk, string fileName);
    Image(unsigned int w, unsigned int h);
    DirPoint getPixel(float x, float y) const;
    void reinhard(double MAX=0.0);
    void equacutre();
    void gamma(float gamma);
    void equalization();
    void write(const string &fileName);
    void writeHDR(const string &fileName);
    void add(Image& img);


    // Pixels stuff
    void fillPixelPoints(float f, float u, float l, const TMatrix& t);
};



#endif //GRAFICA2_IMAGE_H
