//
// Created by serizba on 27/12/18.
//

#include <Film.h>
#include <algorithm>
#include <fstream>
#include <numeric>

#include "Film.h"

Film::Film(int width, int height) : width(width), height(height), img(height*width), distw(0.f, 1.f/width), disth(0.f,1.f/height) {
}

Film::~Film() {

}

int Film::get_width() const {
    return this->width;
}

int Film::get_height() const {
    return this->height;
}

const Vector2 Film::window_coords2image_coords(const Vector2 &p) {
    return Vector2(this->distw(gen) + (p[0]-(this->width/2.0f))/(float) this->width, this->disth(gen) + (p[1]-(this->height/2.0f))/(float) this->height);
    //return Vector2((p[0]-(this->width/2.0f))/(float) this->width, (p[1]-(this->height/2.0f))/(float) this->height);
}

void Film::draw_pixel(int x, int y, const Vector3 &cv) {
    this->img[(this->height-y-1)*this->width+x] += cv;
}

void Film::write(const char *name) {
    // Tonemap
    this->reinhard();
    //this->equacutre();

    // Write
    ofstream f_out;
    f_out.open(name);
    f_out << "P3" << endl;
    f_out << this->width << " " << this->height << "\n255\n";
    for (int i = 0; i < img.size(); i++) {
        f_out << max(min((int)(255 * img[i][0]), 255), 0) << " " << max(min((int)(255 * img[i][1]), 255), 0) << " " << max(min((int)(255 * img[i][2]), 255), 0) << " ";
        if (i + 1 % this->width == 0) {
            f_out << endl;
        }
    }
    f_out.close();
}

// img must be in rgb ranging 0-255
void Film::writeHDR(const string &fileName) {
    int c = 1073741824;
    ofstream f_out;
    f_out.open (fileName);
    f_out << "P3" << endl;
    f_out << "#MAX=" << 1 << endl;
    f_out << "# " << fileName << endl;
    f_out << this->width << " " << this->height << endl;
    f_out << c << endl;
    for (int i = 0; i < img.size(); i++) {
        f_out << img[i][0] << " " << img[i][1] << " " << img[i][2] << " ";
        if (i + 1 % this->width == 0) {
            f_out << endl;
        }
    }
    f_out.close();
}


void Film::equacutre() {
    auto mmax = minmax_element(this->img.begin(), this->img.end(), [](Vector3 a, Vector3 b) { return a[0] < b[0];});
    float divisor = (*mmax.second)[0] - (*mmax.first)[0];
    for_each(this->img.begin(), this->img.end(), [divisor](Vector3& a) { a = Vector3(a[0] / divisor, a[1] / divisor, a[2] / divisor);});
}


static double RGB2XYZ  [3][3] = {{0.5141364, 0.3238786,  0.16036376},
                                 {0.265068,  0.67023428, 0.06409157},
                                 {0.0241188, 0.1228178,  0.84442666}};
static double XYZ2RGB  [3][3] = {{ 2.5651,   -1.1665,   -0.3986},
                                 {-1.0217,    1.9777,    0.0439},
                                 { 0.0753,   -0.2543,    1.1892}};

void rgb2xyY(Vector3& p) {
    float w;
    Vector3 r(0,0,0);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            r[i] += RGB2XYZ[i][j] * p[j];
    //p = mat2xyz*p;
    w = r[0] + r[1] + r[2];
    if (w > 0.) {
        p[0] = r[1];         /* Y */
        p[1] = r[0] / w;     /* x */
        p[2] = r[1] / w;     /* y */
    }
    else {
        p[0] = p[1] = p[2] = 0.f;
    }
}

void xyY2rgb(Vector3& p) {
    float X,Y,Z;
    Vector3 r(0,0,0);
    Y         = p[0];        /* Y */
    r[1] = p[1];        /* x */
    r[2] = p[2];        /* y */
    if ((Y > 0.) && (r[1] > 0.) && (r[2] > 0.)) {
        X = (r[1] * Y) / r[2];
        Z = (X/r[1]) - X - Y;
    }
    else {
        X = Z = 0.f;
    }
    p[0] = X;
    p[1] = Y;
    p[2] = Z;
    r[0] = r[1] = r[2] = 0.f;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            r[i] += XYZ2RGB[i][j] * p[j];
    for (int i = 0; i < 3; i++)
        p[i] = r[i];
}

void Film::reinhard() {
    float log_avg = accumulate(img.begin(), img.end(), 0.0, [] (float a, Vector3& p) {rgb2xyY(p); return a + log(0.00001+p[0]);});
    auto MAX = (*max_element(img.begin(), img.end(), [] (Vector3 p1, Vector3 p2) {return p1[0]<p2[0];}))[0];
    log_avg = exp(log_avg / (float) img.size());
    for_each(img.begin(),img.end(), [MAX,log_avg] (Vector3& p) {
        p[0] *= 0.18/log_avg;
        p[0] = p[0] * (1.f + (p[0] / (MAX*MAX))) / (1.f + p[0]);
        xyY2rgb(p);
    });
}
