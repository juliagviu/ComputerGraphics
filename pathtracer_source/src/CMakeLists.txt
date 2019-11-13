cmake_minimum_required(VERSION 2.8)
project(pathTracerClion)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_definitions(-DMY_DEBUG)

include_directories(.)

add_executable(pathTracerClion
        #cylinder_stack.cpp
        BRDF.cpp
        BRDF.h
        Cuboid.cpp
        Cuboid.h
        DirPoint.cpp
        DirPoint.h
        Figure.cpp
        Figure.h
        main.cpp
        PathTracer.cpp
        PathTracer.h
        Plane.cpp
        Plane.h
        PuntualLight.cpp
        PuntualLight.h
        Sphere.cpp
        Sphere.h
        TMatrix.cpp
        TMatrix.h
        Image.cpp
        Image.h
        CSG.cpp
        CSG.h
        Cylinder.cpp
        Cylinder.h
        Cone.cpp
        Cone.h)
