//
// Created by serizba on 18/11/18.
//

#ifndef GRAFICA_BRDF_H
#define GRAFICA_BRDF_H


#include "Image.h"

class BRDF {

    enum Type {NOP=0, EMISSOR, DPHONG, DSPEC, FRES};

public:
    // Diffuse - Phong - Specular - Fresnel - Emission
    Type type;
    DirPoint Kd, Ks, Ksp, e;
    // Diffuse - Emission
    Image *D, *E;

    float alfa, nin, nout;



public:


    // Perform Russian Roulette
    int russianRoulette(double rr) const;
    // Obtain the BRDF of a specific point of image
    BRDF getBRDF(float x, float y) const;
    // Has a texture?
    bool isTextured() const;

    // Constructors
    // Empty for a miss
    BRDF();

    // With coefficients
    static BRDF Fresnel(float nin, float nout);
    static BRDF Phong(DirPoint Kd, DirPoint Ks, float alfa);
    static BRDF Mirror(DirPoint Kd, DirPoint Ksp);
    static BRDF Emisor(DirPoint e);

    // With textures

    // Only Kd is modeled with textures
    static BRDF TextPhong(string Kd, DirPoint Ks, float alfa);
    static BRDF TextEmisor(string e, DirPoint Kd);

private:
    BRDF(Type type, const DirPoint &Kd, const DirPoint &Ks, const DirPoint &Ksp, const DirPoint &e,
         Image *D, Image *E, float alfa, float nin, float nout);

};


#endif //GRAFICA_BRDF_H
