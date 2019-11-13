//
// Created by serizba on 29/12/18.
//


#include <algorithm>
#include "Image.h"

Image::Image(string fileName) {
    ifstream f_in(fileName);
    // Comprobar si fichero es válido
    if(!f_in) {
        cerr << "Cannot open the file : " << fileName << endl;
        exit(1);
    }

    string line;
    unsigned int c,r,g,b;
    getline(f_in,line);
    getline(f_in,line);
    // This attribute should be optional (we consider it mandatory)
    double MAX = stod(line.substr(5));
    getline(f_in,line);
    f_in >> this->w >> this->h;
    f_in >> c;

    this->img = vector<DirPoint>(this->w*this->h);
    // Loop over all values
    for (int i=0; i<w*h; i++) {
        f_in >> r >> g >> b;
        this->img[i] = DirPoint((r*MAX)/(double)c,(g*MAX)/(double)c,(b*MAX)/(double)c,0);
    }

    f_in.close();
}

DirPoint Image::getPixel(float x, float y) const {
    return this->img[std::min(((int)(y * this->h)), (int) this->h - 1) * this->w + (int)(x * this->w)];
}


Image::Image(unsigned int w, unsigned int h) : h(h), w(w), img(w*h) {}



static double RGB2XYZ  [3][3] = {{0.5141364, 0.3238786,  0.16036376},
                                 {0.265068,  0.67023428, 0.06409157},
                                 {0.0241188, 0.1228178,  0.84442666}};
static double XYZ2RGB  [3][3] = {{ 2.5651,   -1.1665,   -0.3986},
                                 {-1.0217,    1.9777,    0.0439},
                                 { 0.0753,   -0.2543,    1.1892}};

void rgb2xyY(DirPoint& p) {
    double w;
    DirPoint r(0,0,0);
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

void xyY2rgb(DirPoint& p) {
    double X,Y,Z;
    DirPoint r(0,0,0);
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

void Image::reinhard(double MAX) {
    double log_avg = accumulate(img.begin(), img.end(), 0.0, [] (double a, DirPoint& p) {rgb2xyY(p); return a + log(0.00001+p[0]);});
    //for (DirPoint p : img) { cout << p[0] << endl;}
    if (MAX==0.0) {
        MAX = (*max_element(img.begin(), img.end(), [](DirPoint p1, DirPoint p2) { return p1[0] < p2[0]; }))[0];
    }
    //cout << MAX << endl;
    log_avg = exp(log_avg / (double) img.size());
    for_each(img.begin(),img.end(), [MAX,log_avg] (DirPoint& p) {
        p[0] *= 0.18/log_avg;
        p[0] = p[0] * (1. + (p[0] / (MAX*MAX))) / (1. + p[0]);
        xyY2rgb(p);
    });
}


// img must be in rgb
void Image::equalization() {
    cout << "aqui" << endl;
    for_each(img.begin(), img.end(),  [] (DirPoint& p) {rgb2xyY(p);});
    auto mmax = minmax_element(img.begin(), img.end(), [](DirPoint a, DirPoint b) { return a[0] < b[0];});
    cout << "here" << endl;
    cout << img.size() << endl;
    cout << (*max_element(img.begin(), img.end(), [](DirPoint p1, DirPoint p2) { return p1[0] < p2[0]; }))[0] << endl;
    double divisor = (*mmax.second)[0] - (*mmax.first)[0];
    cout << divisor << endl;
    for_each(img.begin(), img.end(),  [divisor] (DirPoint& p) {p[0] = p[0]/divisor; xyY2rgb(p); });
}

void Image::gamma(float gamma) {
    cout << "aca" << endl;
    this->equalization();
    for_each(img.begin(), img.end(),  [gamma] (DirPoint& p) {
        rgb2xyY(p);
        p[0] = pow(p[0], 1.0f/gamma);
        xyY2rgb(p);
    });
}

void Image::equacutre() {
    auto mmax = minmax_element(this->img.begin(), this->img.end(), [](DirPoint a, DirPoint b) { return a[0] < b[0];});
    float divisor = (*mmax.second)[0] - (*mmax.first)[0];
    for_each(this->img.begin(), this->img.end(), [divisor](DirPoint& a) { a = DirPoint(a[0] / divisor, a[1] / divisor, a[2] / divisor);});
}

// img must be in rgb ranging 0-255
void Image::write(const string &fileName) {
    ofstream f_out;
    f_out.open (fileName);
    f_out << "P3" << endl;
    f_out << w << " " << h << "\n255\n";
    for (int i=0; i<img.size(); i++) {
        f_out << max(min((int)(255*img[i][0]),255),0) << " " << max(min((int)(255*img[i][1]),255),0) << " " << max(min((int)(255*img[i][2]),255),0) << " ";
        if (i+1%w == 0) {
            f_out << endl;
        }
    }
    f_out.close();
}

// img must be in rgb ranging 0-255
void Image::writeHDR(const string &fileName) {
    int c = 1073741824;
    ofstream f_out;
    f_out.open (fileName);
    f_out << "P3" << endl;
    f_out << "#MAX=" << 1 << endl;
    f_out << "# " << fileName << endl;
    f_out << this->w << " " << this->h << endl;
    f_out << c << endl;
    for (int i=0; i<img.size(); i++) {
        f_out << img[i][0] << " " << img[i][1] << " " << img[i][2] << " ";
        if (i+1%w == 0) {
            f_out << endl;
        }
    }
    f_out.close();
}

Image::Image(bool kk, string fileName) {
    ifstream f_in(fileName);
    // Comprobar si fichero es válido
    if(!f_in) {
        cerr << "Cannot open the file : " << fileName << endl;
        exit(1);
    }

    string line;
    double c,r,g,b;
    getline(f_in,line);
    getline(f_in,line);
    // This attribute should be optional (we consider it mandatory)
    double MAX = stod(line.substr(5));
    getline(f_in,line);
    f_in >> this->w >> this->h;
    f_in >> c;

    this->img = vector<DirPoint>(this->w*this->h);
    // Loop over all values
    for (int i=0; i<w*h; i++) {
        f_in >> r >> g >> b;
        this->img[i] = DirPoint(r, g, b,0);
    }

    f_in.close();
}

void Image::fillPixelPoints(float f, float u, float l, const TMatrix &t)  {
    for (int i=0; i<h; i++) {
        for (int j=0; j<w; j++) {
            img[i * w + j] = t * DirPoint(f, l-j*(2*l)/w, u-i*(2*u)/h, 1);
        }
    }
}

void Image::add(Image& img) {
    std::transform (this->img.begin(), this->img.end(), img.img.begin(), this->img.begin(), std::plus<DirPoint>());
}



