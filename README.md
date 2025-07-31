# CTIS164 – Technical Mathematics with Programming

This repository contains homework assignments for CTIS164, which combines mathematics with C++ programming and basic graphics using OpenGL/FreeGLUT.

## 📘 Homework Overview

### HW1 – Concentric Circles  
Draws three concentric circles using parametric equations and renders them with OpenGL.

### HW2 – Triangle Angle Calculator  
Calculates angles and side lengths of a triangle given three 2D points, then visualizes the triangle.

### HW3 – Vector Operations  
Implements common 2D vector functions (e.g., magnitude, dot product, polar/rectangular conversion) and visualizes them.

## 🛠️ Build Example (MSYS2)
```bash
g++ hw3.cpp vec.cpp -o hw3.exe -lfreeglut -lopengl32 -lglu32
