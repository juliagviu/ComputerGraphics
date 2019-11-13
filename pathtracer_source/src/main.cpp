//
// Created by serizba on 13/11/18.
//

#include "Sphere.h"
#include "PathTracer.h"
#include "Plane.h"
#include "Cuboid.h"
#include "Cylinder.h"
#include "Cone.h"
#include "CSG.h"

int main() {
    vector<Figure*> scene;
    vector<PuntualLight> lights;

    // Mandatory scenes are 0 and 1, nice textured is 6, nice CSG 7
    int image = 0;

    // Materials
    BRDF red_diffuse = BRDF::Phong({0.9,0.1,0.1}, {0,0,0}, 0);
    BRDF green_diffuse = BRDF::Phong({0.1,0.8,0.1},{0,0,0}, 0);
    BRDF grey_diffuse = BRDF::Phong({0.5,0.5,0.5},{0,0,0}, 0);
    BRDF black_diffuse = BRDF::Phong({0.1,0.1,0.1},{0,0,0}, 0);
    BRDF yellow_diffuse = BRDF::Phong({0.8, 0.8, 0.085}, {0.0, 0.0, 0.0}, 0);
    BRDF yellow_phong = BRDF::Phong({0.8, 0.8, 0.085}, {0.1, 0.1, 0.1}, 10);
    BRDF sky = BRDF::Phong({0.4588,0.85,0.95}, {0,0,0}, 0);
    BRDF mirror = BRDF::Mirror({0,0,0},{0.9,0.9,0.9});
    // TODO: revisar BRDF fresnel que no funciona (todo negro)
    BRDF fresnel = BRDF::Fresnel(1.51,1.0);

    BRDF emisor = BRDF::Emisor({255,255,255});

    switch(image) {
        case 1: {
            // Cornell box with area light plane two spheres
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), emisor));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), BRDF()));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo

            scene.push_back(new Sphere(DirPoint(900, -80, -180, 1), 75, 0, 0, yellow_phong));
            //scene.push_back(new Sphere(DirPoint(1000, 80, -180, 1), 75, 0, 0, mirror));
            break;
        }
        case 2: {
            // Cornell box with puntual lights, one Sphere
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));
            // two puntual lights on the corners
            //lights.push_back(PuntualLight(DirPoint(900, -50,200,1), emisor));
            //lights.push_back(PuntualLight(DirPoint(900,50,200,1), emisor));
            scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, fresnel));
            break;
        }
        case 3: {
            // Cornell box with area light plane, one Sphere
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(DirPoint(-1, 0, 0, 1), DirPoint(1, 0, 0, 0), grey_diffuse));
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(
                    new Plane(300, DirPoint(0, 0, 1, 0), DirPoint(1, 0, 0, 0), DirPoint(500, 200, 300, 1), 800, 400,
                              BRDF::TextEmisor("texturas/emisor.ppm", {0.9, 0.1, 0.1})));
            //scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), emisor)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, yellow_phong));
            break;
        }
        case 4: {
            // Box with textures, puntual lights, one Sphere
            scene.push_back(
                    new Plane(200, DirPoint(0, 1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(1200, DirPoint(1, 0, 0, 0), DirPoint(0, 1, 0, 0), DirPoint(1200, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(200, DirPoint(0, -1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, -200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(300, DirPoint(0, 0, 1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 300, 1), 800, 400,
                              BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(250, DirPoint(0, 0, -1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, -250, 1), 800, 400,
                              BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));
            scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, fresnel));
            break;
        }
        case 5: {
            // Box with textures, area light plane, one Sphere
            scene.push_back(
                    new Plane(200, DirPoint(0, 1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(1200, DirPoint(1, 0, 0, 0), DirPoint(0, 1, 0, 0), DirPoint(1200, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(200, DirPoint(0, -1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, -200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(300, DirPoint(0, 0, 1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 300, 1), 800, 400,
                              emisor));
            scene.push_back(
                    new Plane(250, DirPoint(0, 0, -1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, -250, 1), 800, 400,
                              BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, mirror));
            break;
        }
        case 6: {
            // Box with textures, area light textured, one Sphere
            scene.push_back(
                    new Plane(200, DirPoint(0, 1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(1200, DirPoint(1, 0, 0, 0), DirPoint(0, 1, 0, 0), DirPoint(1200, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(200, DirPoint(0, -1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, -200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(300, DirPoint(0, 0, 1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 300, 1), 800, 400,
                              BRDF::TextEmisor("texturas/emisor.ppm", {0.9, 0.1, 0.1})));
            scene.push_back(
                    new Plane(250, DirPoint(0, 0, -1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, -250, 1), 800, 400,
                              BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, yellow_phong));
            break;
        }
        case 7: {
            // Cornell box with puntual lights, CSG of cuboid and sphere
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));
            Sphere *s = new Sphere(DirPoint(925, 25, -25, 1), 100, 0, 0, yellow_diffuse);
            TMatrix rot = TMatrix::rotate(0.6, 0) * TMatrix::rotate(0.6, 2);
            Cuboid *c = new Cuboid(rot * (160 * DirPoint::normalize(DirPoint(1, 0, 0, 0))),
                                   rot * (160 * DirPoint::normalize(DirPoint(0, -1, 0, 0))),
                                   rot * (160 * DirPoint::normalize(DirPoint(0, 0, 1, 0))),
                                   DirPoint(820, 80, -80, 1), mirror);
            //scene.push_back(s);
            //scene.push_back(c);
            scene.push_back(new CSG(new CSGTree(new CSGTree(c), new CSGTree(s), 1), yellow_diffuse));
            //BRDF::TextPhong("texturas/ladrillos.ppm", {0,0,0}, 0),DirPoint(820,80,-200,1),DirPoint(820,80,100,1), 140, DirPoint(1,1,1,0)));
            break;
        }
        case 8: {
            // Cornell box with area light plane, CSG of cuboid and sphere
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), emisor)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            Sphere *s = new Sphere(DirPoint(925, 25, -25, 1), 100, 0, 0, yellow_diffuse);
            TMatrix rot = TMatrix::rotate(0.6, 0) * TMatrix::rotate(0.6, 2);
            Cuboid *c = new Cuboid(rot * (160 * DirPoint::normalize(DirPoint(1, 0, 0, 0))),
                                   rot * (160 * DirPoint::normalize(DirPoint(0, -1, 0, 0))),
                                   rot * (160 * DirPoint::normalize(DirPoint(0, 0, 1, 0))),
                                   DirPoint(820, 80, -80, 1), yellow_diffuse);
            //scene.push_back(s);
            //scene.push_back(c);
            scene.push_back(new CSG(new CSGTree(new CSGTree(c), new CSGTree(s), 2), yellow_diffuse));
            break;
        }
        case 9: {
            // Cornell box with area light, one Sphere, black plane behind
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), emisor)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            scene.push_back(new Plane(0, DirPoint(-1, 0, 0, 0), black_diffuse)); // Plano extra de detras

            scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, mirror));
            break;
        }
        case 10: {
            // Box textured, snail with CSG
            scene.push_back(
                    new Plane(200, DirPoint(0, 1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(1200, DirPoint(1, 0, 0, 0), DirPoint(0, 1, 0, 0), DirPoint(1200, 200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(200, DirPoint(0, -1, 0, 0), DirPoint(1, 0, 0, 0), DirPoint(800, -200, 0, 1), 800, 400,
                              BRDF::TextPhong("texturas/ladrillos.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(300, DirPoint(0, 0, 1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, 300, 1), 800, 400,
                              BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            scene.push_back(
                    new Plane(200, DirPoint(0, 0, -1, 0), DirPoint(1, 0, 0, 0), DirPoint(800, 200, -250, 1), 800, 400,
                              BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));
            Sphere *s0 = new Sphere(DirPoint(900, -50, -290, 1), 140, 0, 0, yellow_diffuse);
            Sphere *s1 = new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, yellow_diffuse);
            Sphere *s2 = new Sphere(DirPoint(900, -50, -80, 1), 70, 0, 0, yellow_diffuse);
            Sphere *s3 = new Sphere(DirPoint(900, -50, -40, 1), 50, 0, 0, yellow_diffuse);
            Sphere *s4 = new Sphere(DirPoint(900, -50, 0, 1), 30, 0, 0, yellow_diffuse);
            scene.push_back(new CSG(new CSGTree(
                    new CSGTree(
                            new CSGTree(
                                    new CSGTree(
                                            new CSGTree(new CSGTree(s1), new CSGTree(s0), 2),
                                            new CSGTree(s2),
                                            2),
                                    new CSGTree(new CSGTree(s2), new CSGTree(s3), 2),
                                    3),
                            new CSGTree(new CSGTree(s3), new CSGTree(s4), 2),
                            3),
                    new CSGTree(s4),
                    3), BRDF::TextPhong("texturas/snail2.ppm", {0, 0, 0}, 0), DirPoint(900, -50, -430, 1),
                                    DirPoint(900, -50, 30, 1), 140, DirPoint(1, 1, 1, 0)));
            break;
        }
        case 11: {
            // Cornell box with one puntual light cylinder
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));

            scene.push_back(new Cylinder(DirPoint(900, -80, -180, 1), DirPoint(900, -80, 180, 1), 50, yellow_phong));
            scene.push_back(
                    new Cylinder(DirPoint(900, -80, -180, 1), DirPoint(1000, 80, 50, 1), 50, DirPoint(1, 1, 1, 0),
                                 BRDF::TextPhong("texturas/madera.ppm", {0, 0, 0}, 0)));
            break;
        }
        case 12: {
            int num = 32;
            int vueltas = 1;


            // Something doing whatever
            scene.push_back(new Plane(400, DirPoint(0, 1, 0, 0), grey_diffuse));
            scene.push_back(new Plane(1600, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(400, DirPoint(0, -1, 0, 0), grey_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));
            DirPoint p(1200, 0, -220, 1);
            DirPoint v1(300, 0, 0, 0);
            DirPoint v2(0, 300, 0, 0);
            DirPoint v3(-300, 0, 0, 0);
            DirPoint v4(0, -300, 0, 0);
            auto rot = TMatrix::rotate((vueltas * 2 * M_PI) / num, 2) * TMatrix::scale(0.95, 0.95, 0.95);
            double r = 15.0;
            for (int i = 0; i < num; i++) {
                DirPoint p1 = p + v1;
                DirPoint p2 = p + v2;
                DirPoint p3 = p + v3;
                DirPoint p4 = p + v4;

                scene.push_back(new Cylinder(p1, p2, r, yellow_diffuse));
                scene.push_back(new Sphere(p1, r, yellow_diffuse));
                scene.push_back(new Cylinder(p2, p3, r, yellow_diffuse));
                scene.push_back(new Sphere(p2, r, yellow_diffuse));
                scene.push_back(new Cylinder(p3, p4, r, yellow_diffuse));
                scene.push_back(new Sphere(p3, r, yellow_diffuse));
                scene.push_back(new Cylinder(p1, p4, r, yellow_diffuse));
                scene.push_back(new Sphere(p4, r, yellow_diffuse));

                p = p + DirPoint(0, 0, r + r * 0.95);
                r = 0.95 * r;
                v1 = rot * v1;
                v2 = rot * v2;
                v3 = rot * v3;
                v4 = rot * v4;
            }

            break;


        }
        case 13: {
            // Cornell box with one cone
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));

            scene.push_back(new Cone({900, -80, -180, 1}, {1000, 80, -80, 1}, 30.0, 5.0, yellow_phong));
            break;
        }
        case 14: {
            // Cornell box with puntual lights, one Sphere
            scene.push_back(new Plane(DirPoint(0, -150, 0, 1), DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(DirPoint(0, 150, 0, 1), DirPoint(0, 1, 0, 0), green_diffuse));
            scene.push_back(new Plane(DirPoint(150, 0, 0, 1), DirPoint(1, 0, 0, 0), grey_diffuse));
            scene.push_back(new Plane(DirPoint(0, 0, 200, 1), DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(DirPoint(0, 0, 0, 1), DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de abajo
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(0, 0, 190), emisor));
            // two puntual lights on the corners
            //lights.push_back(PuntualLight(DirPoint(900, -50,200,1), emisor));
            //lights.push_back(PuntualLight(DirPoint(900,50,200,1), emisor));
            //scene.push_back(new Sphere(DirPoint(900, -50, -150, 1), 100, 0, 0, fresnel));
            break;
        }
        case 15: {
            // Cornell box with two puntual light and many spheres (with high 1920x1440 resolution)
            scene.push_back(new Plane(350, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1400, DirPoint(1, 0, 0, 0), BRDF::Phong({0.9,0.9,0.9}, {0,0,0},0))); // Plano del fondo
            scene.push_back(new Plane(350, DirPoint(0, -1, 0, 0),green_diffuse));
            scene.push_back(new Plane({-1, 0, 0, 1}, DirPoint(1, 0, 0, 0), BRDF::Phong({0.9,0.9,0.9}, {0,0,0},0)));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), BRDF::Phong({0.9,0.9,0.9}, {0,0,0},0))); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), BRDF::Phong({0.9,0.9,0.9}, {0,0,0},0))); // Plano de abajo
            //lights.push_back(PuntualLight(DirPoint(900, 0, 170, 1), emisor));
            lights.push_back(PuntualLight(DirPoint(1125, 0, 230, 1), emisor));
            /*lights.push_back(PuntualLight(DirPoint(1000, 50, 170, 1), emisor));*/

            //scene.push_back(new Sphere({1125, 0, 325}, 80, emisor));



            Sphere *s = new Sphere(DirPoint(1125, 25, -25, 1), 100, 0, 0, sky);
            /*TMatrix rot = TMatrix::rotate(0.6, 0) * TMatrix::rotate(0.6, 2);*/
            /*Cuboid *c = new Cuboid(rot * (160 * DirPoint::normalize(DirPoint(1, 0, 0, 0))),
                                   rot * (160 * DirPoint::normalize(DirPoint(0, -1, 0, 0))),
                                   rot * (160 * DirPoint::normalize(DirPoint(0, 0, 1, 0))),
                                   DirPoint(1020, 80, -80, 1), fresnel);*/

/*            Cuboid *c = new Cuboid(rot * (240 * DirPoint::normalize(DirPoint(1, 0, 0, 0))),
                                   rot * (240 * DirPoint::normalize(DirPoint(0, -1, 0, 0))),
                                   rot * (240 * DirPoint::normalize(DirPoint(0, 0, 1, 0))),
                                   DirPoint(1020, 80, -80, 1), fresnel);*/

            TMatrix rot1 = TMatrix::rotate(0.7853, 1); // * TMatrix::rotate(0.7853, 2) * TMatrix::rotate(-0.7853, 0);
            TMatrix rot2 = TMatrix::rotate(0.7853, 1) * TMatrix::rotate(-0.7853, 2) * TMatrix::rotate(-0.7853, 0);
            Cuboid *c1 = new Cuboid(rot1 * (140 * DirPoint::normalize(DirPoint(-1, 0, 0, 0))),
                                   rot1 * (140 * DirPoint::normalize(DirPoint(0, -1, 1, 0))),
                                   rot1 * (140 * DirPoint::normalize(DirPoint(0, 1, 1, 0))),
                                   DirPoint(1125, 0, -250, 1), sky);

            scene.push_back(new Sphere({1125, 0, -150, 1}, 100, sky));



           /* Cuboid *c2 = new Cuboid(rot2 * (140 * DirPoint::normalize(DirPoint(-1, 0, 0, 0))),
                                   rot2 * (140 * DirPoint::normalize(DirPoint(0, -1, 1, 0))),
                                   rot2 * (140 * DirPoint::normalize(DirPoint(0, 1, 1, 0))),
                                   DirPoint(1125, 160, 30, 1), fresnel);*/

/*            Cuboid *c3 = new Cuboid((240 * DirPoint::normalize(DirPoint(1, 0, 0, 0))),
                                    (240 * DirPoint::normalize(DirPoint(0, 1, 0, 0))),
                                    (240 * DirPoint::normalize(DirPoint(0, 0, 1, 0))),
                                    DirPoint(1125, -120, -250, 1), fresnel);

            scene.push_back(c3);*/


            //scene.push_back(new Bounding(new Sphere({925, 25, new CSG(new CSGTree(new CSGTree(c), new CSGTree(s), 2), yellow_diffuse));
            //scene.push_back(s);
            //scene.push_back(c);

            scene.push_back(c1);
            //scene.push_back(c2);

/*            scene.push_back(new Sphere({1125, 0, -100, 1}, 100, fresnel));
            scene.push_back(new Sphere({1125, 0, -100, 1}, 40, BRDF::Fresnel(1.0, 1.5)));*/

            break;
        }
    default: {// case 0
            // Cornell box with one puntual light two spheres
            scene.push_back(new Plane(200, DirPoint(0, 1, 0, 0), red_diffuse));
            scene.push_back(new Plane(1200, DirPoint(1, 0, 0, 0), grey_diffuse)); // Plano del fondo
            scene.push_back(new Plane(200, DirPoint(0, -1, 0, 0), green_diffuse));
            scene.push_back(new Plane(300, DirPoint(0, 0, 1, 0), grey_diffuse)); // Plano de arriba
            scene.push_back(new Plane(250, DirPoint(0, 0, -1, 0), grey_diffuse)); // Plano de abajo
            // one puntual light in the middle
            lights.push_back(PuntualLight(DirPoint(900, 0, 200, 1), emisor));

            scene.push_back(new Sphere(DirPoint(900, -80, -180, 1), 75, 0, 0, yellow_phong));
            scene.push_back(new Sphere(DirPoint(1000, 80, -180, 1), 75, 0, 0, mirror));
            break;
        }
    }


    PathTracer r(DirPoint(0, 0, 0, 1), DirPoint(400, 0, 0, 0), DirPoint(0, 1.33*120, 0, 0), DirPoint(0, 0, 120, 0), scene, lights);
    auto img = r.render(1920, 1440, 128);
    img.reinhard(1.5);
    //img.equacutre();
    img.write("imagen.ppm");


    // TO GENERATE SEVERAL IMAGES
/*    int i = 0;
    while(1) {
        auto img = r.render(1000, 1000, 16);
        img.writeHDR("ultimate/csg-i/"+to_string(i)+".ppm");
        cout << i << endl;
        i++;
    }*/

}