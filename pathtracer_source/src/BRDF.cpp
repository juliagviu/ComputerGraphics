//
// Created by serizba on 18/11/18.
//

#include "BRDF.h"
#include "Image.h"

int BRDF::russianRoulette(double rr) const {
    switch (this->type) {
        case DPHONG:
            if (this->Kd.avg() + this->Ks.avg() >= rr) {return DPHONG;} // Phong
            return NOP;
        case DSPEC:
            if (this->Kd.avg() >= rr)
                return DPHONG; // Phong - diffuse
            else if (this->Kd.avg() + this->Ksp.avg() >= rr)
                return DSPEC; // Perfect specular
            return NOP;
        case FRES:
            if (rr >= 0.9) { return NOP;}
            return FRES;     // Fresnel

        case EMISSOR:
            return EMISSOR;     // Emissor

        default:
            return NOP;     // Miss or absorb
    }
}

BRDF BRDF::Fresnel(float nin, float nout) {return BRDF(FRES, {}, {}, {}, {}, nullptr, nullptr, 0.f, nin, nout);}
BRDF BRDF::Phong(DirPoint Kd, DirPoint Ks, float alfa) {return BRDF(DPHONG, Kd, Ks, {}, {}, nullptr, nullptr, alfa, 0.f, 0.f);}
BRDF BRDF::Mirror(DirPoint Kd, DirPoint Ksp) {return BRDF(DSPEC, Kd, {}, Ksp, {}, nullptr, nullptr, 0.f, 0.f, 0.f);}
BRDF BRDF::Emisor(DirPoint e) {return BRDF(EMISSOR, {}, {}, {}, e, nullptr, nullptr, 0.f, 0.f, 0.f);}
BRDF BRDF::TextPhong(string Kd, DirPoint Ks, float alfa) {return BRDF(DPHONG, {}, Ks, {}, {}, new Image(Kd), nullptr, alfa, 0.f, 0.f);}
BRDF BRDF::TextEmisor(string e, DirPoint Kd) {return BRDF(EMISSOR, {},{}, {}, {}, nullptr, new Image(e), 0.f, 0.f, 0.f);}

BRDF BRDF::getBRDF(float x, float y) const {
    if (this->E) {
        // Emission texture
        DirPoint e = this->E->getPixel(x,y);
        if (e.avg()>0.f) {return Emisor(e);}
        else { return Phong(this->Kd, {}, 0.f);}

    } else if (this->D) {
        // Diffuse texture
        DirPoint d = this->D->getPixel(x,y);
        return Phong(d, this->Ks, this->alfa);
    } else {
        return *this;
    }
}

// Is a texture BRDF?
bool BRDF::isTextured() const {
    return this->E || this->D;
}

// Constructors

BRDF::BRDF() : BRDF(NOP, {}, {}, {}, {}, nullptr, nullptr, 0.f, 0.f, 0.f) {}

BRDF::BRDF(Type type, const DirPoint &Kd, const DirPoint &Ks, const DirPoint &Ksp, const DirPoint &e,
           Image *D, Image *E, float alfa, float nin, float nout) :                                  type(type), Kd(Kd),
                                                                                                     Ksp(Ksp), Ks(Ks),
                                                                                                     e(e), D(D), E(E),
                                                                                                     alfa(alfa),
                                                                                                     nin(nin),
                                                                                                     nout(nout) {}
