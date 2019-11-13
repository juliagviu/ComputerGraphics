//
// Created by serizba on 18/11/18.
//

#ifndef GRAFICA_PUNTUALLIGHT_H
#define GRAFICA_PUNTUALLIGHT_H


#include "DirPoint.h"
#include "BRDF.h"

class PuntualLight {
public:
    BRDF brdf;
    DirPoint c;
    PuntualLight(const DirPoint &c, const BRDF &brdf);
};


#endif //GRAFICA_PUNTUALLIGHT_H
