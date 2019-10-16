# cmake_minimum_required(VERSION <specify CMake version here>)
project(raytracing_in_a_weekend)

set(CMAKE_CXX_STANDARD 14)

include_directories("raytracing in a weekend")

add_executable(raytracing_in_a_weekend
    "raytracing in a weekend/Camera.h"
    "raytracing in a weekend/HitAble.h"
    "raytracing in a weekend/hitable_list.h"
    "raytracing in a weekend/Main.cpp"
    "raytracing in a weekend/Material.h"
    "raytracing in a weekend/ray.h"
    "raytracing in a weekend/sphere.h"
    "raytracing in a weekend/vec3.h")
