//
// Created by Ujjwal Chadha on 11/2/19.
//

#ifndef UNTITLED_CAMERA_H
#define UNTITLED_CAMERA_H

#include "Mesh.h"
#include "Camera.h"
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include "Utils.h"

using namespace Eigen;
using namespace std;

class Camera {
private:
    Vector3f eye;
    Vector3f lookAt;
//    MatrixXf view;
    MatrixXf projection;

public:
    static const int PROJECTION_ORTHOGRAPHIC = 0, PROJECTION_PRESPECTIVE = 1;

    Camera(int screenWidth, int screenHeight);
    Camera(const Vector3f& eye, const Vector3f& lookAt, int screenWidth, int screenHeight);
    MatrixXf getView();
    Vector3f getEye();
    Vector3f getLookAt();
    MatrixXf getProjection();

    void translateBy(const Eigen::Vector3f& position); //translate
//    void rotateAboutOrigin(int axis, float radians); //rotate
//    void rotateAboutItself(int axis, float radians);
//    void zoom(float factor); //scale
    void setEye(const Vector3f& eye);
    void setLookAt(const Vector3f& lookAt);

};


#endif //UNTITLED_CAMERA_H
