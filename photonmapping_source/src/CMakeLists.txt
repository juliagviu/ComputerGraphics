cmake_minimum_required(VERSION 2.8)
project(SmallPM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-no-pie")
set(CMAKE_CXX_FLAGS_RELEASE   "-O3 -no-pie")
set(CMAKE_CXX_FLAGS "-no-pie")

include_directories(include)
include_directories(smallrt)
include_directories(smallrt/include)
include_directories(smallrt/include/image)


add_library(MyLib
        include/KDTree.h
        include/PhotonMapping.h
        include/RenderEngine.h
        include/Film.h
        smallrt/include/image/cimg-additions.h
        smallrt/include/image/cimg-mlaa.h
        smallrt/include/image/cimg-tmo.h
        smallrt/include/image/CImg.h
        smallrt/include/image/image-function.h
        smallrt/include/image/image-manager.h
        smallrt/include/image/image-sampler-iterative.h
        smallrt/include/image/image-sampler-threads.h
        smallrt/include/image/image-sampler.h
        smallrt/include/image/image.h
        smallrt/include/image/mlaa-wrapper.h
        smallrt/include/image/mlaa.hxx
        smallrt/include/image/rgbcolor.h
        smallrt/include/image/spectrum2rgb.h
        smallrt/include/image/spectrum2rgb.old.h
        smallrt/include/image/tone-mapper.h
        smallrt/include/AABB.h
        smallrt/include/BSDF.h
        smallrt/include/BVH.h
        smallrt/include/Camera.h
        smallrt/include/flags.h
        smallrt/include/globals.h
        smallrt/include/Intersection.h
        smallrt/include/Lambertian.h
        smallrt/include/LightSource.h
        smallrt/include/Matrix4x4.h
        smallrt/include/Mesh.h
        smallrt/include/Object3D.h
        smallrt/include/Phong.h
        smallrt/include/Plane.h
        smallrt/include/Ray.h
        smallrt/include/Specular.h
        smallrt/include/Sphere.h
        smallrt/include/Timer.h
        smallrt/include/Transmissive.h
        smallrt/include/Triangle.h
        smallrt/include/Vector2.h
        smallrt/include/Vector3.h
        include/World.h
        src/PhotonMapping.cpp
        src/RenderEngine.cpp
        src/Film.cpp src/Random.cpp include/Random.h
        include/Foggy.h src/Foggy.cpp)

#set_target_properties(${SmallPM} PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(SmallPM src/main.cpp)
#target_link_libraries(SmallPM ${CMAKE_SOURCE_DIR}/../Ux/smallrt.a)
target_link_libraries(SmallPM
        ${CMAKE_SOURCE_DIR}/objects/BVH.o
        ${CMAKE_SOURCE_DIR}/objects/LightSource.o
        ${CMAKE_SOURCE_DIR}/objects/Phong.o
        ${CMAKE_SOURCE_DIR}/objects/Sphere.o
        ${CMAKE_SOURCE_DIR}/objects/Vector3.o
        ${CMAKE_SOURCE_DIR}/objects/Camera.o
        ${CMAKE_SOURCE_DIR}/objects/Matrix4x4.o
        ${CMAKE_SOURCE_DIR}/objects/Plane.o
        ${CMAKE_SOURCE_DIR}/objects/Transmissive.o
        ${CMAKE_SOURCE_DIR}/objects/World.o
        ${CMAKE_SOURCE_DIR}/objects/Intersection.o
        ${CMAKE_SOURCE_DIR}/objects/Mesh.o
        ${CMAKE_SOURCE_DIR}/objects/Ray.o
        ${CMAKE_SOURCE_DIR}/objects/Triangle.o
        ${CMAKE_SOURCE_DIR}/objects/Lambertian.o
        ${CMAKE_SOURCE_DIR}/objects/Object3D.o
        ${CMAKE_SOURCE_DIR}/objects/Specular.o
        ${CMAKE_SOURCE_DIR}/objects/Vector2.o
        )

#target_link_libraries(MyLib -ltbb)
target_link_libraries(SmallPM MyLib)
