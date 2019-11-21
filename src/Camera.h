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
    Vector3f cameraPosition;
    Vector3f cameraTarget;

    int projectionType = PROJECTION_ORTHOGRAPHIC;
    float aspectRatio;
    float near;
    float far;
    float fieldOfViewAngle;

public:
    static const int PROJECTION_ORTHOGRAPHIC = 0, PROJECTION_PERSPECTIVE = 1;

    Camera(const Vector3f& cameraPosition, const Vector3f& cameraTarget, int projectionType,
            float aspectRatio, float near, float far, float fieldOfViewAngle);

    Vector3f getCameraPosition();
    Vector3f getCameraTarget();
    int getProjectionType();
    float getAspectRatio();
    float getNear();
    float getFar();
    float getFieldOfViewAngle();

    void setCameraPosition(const Vector3f& cameraPosition);
    void setCameraTarget(const Vector3f& target);
    void setProjectionType(const int projectionType);
    void setAspectRatio(float aspectRatio);
    void setNear(float near);
    void setFar(float far);
    void setFieldOfViewAngle(float fovAngle);

    MatrixXf getView();
    MatrixXf getProjection();

    void translateBy(const Eigen::Vector3f& position);
    void translateByAngleOnYAxis(float angle);
    void translateByAngleOnXAxis(float angle);
};


#endif //UNTITLED_CAMERA_H
