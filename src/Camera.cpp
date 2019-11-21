//
// Created by Ujjwal Chadha on 11/2/19.
//

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(const Vector3f &cameraPosition, const Vector3f &cameraTarget, int projectionType, float aspectRatio, float near, float far,
               float fieldOfViewAngle) {
    this->cameraPosition = cameraPosition;
    this->cameraTarget = cameraTarget;
    this->aspectRatio = aspectRatio;
    this->near = near;
    this->far = far;
    this->fieldOfViewAngle = fieldOfViewAngle;
    this->projectionType = projectionType;
}

void Camera::setCameraPosition(const Vector3f &cameraPosition) {
    this->cameraPosition = cameraPosition;
}

void Camera::setCameraTarget(const Vector3f &cameraTarget) {
    this->cameraTarget = cameraTarget;
}

void Camera::setProjectionType(const int projectionType) {
    this->projectionType = projectionType;
}

void Camera::setAspectRatio(float aspectRatio) {
    this->aspectRatio = aspectRatio;
}

void Camera::setNear(float near) {
    this->near = near;
}

void Camera::setFar(float far) {
    this->far = far;
}

void Camera::setFieldOfViewAngle(float fovAngle) {
    this->fieldOfViewAngle = fovAngle;
}

Vector3f Camera::getCameraPosition() {
    return this->cameraPosition;
}

Vector3f Camera::getCameraTarget() {
    return this->cameraTarget;
}

int Camera::getProjectionType() {
    return this->projectionType;
}

float Camera::getAspectRatio() {
    return this->aspectRatio;
}

float Camera::getNear() {
    return this->near;
}

float Camera::getFar() {
    return this->far;
}

float Camera::getFieldOfViewAngle() {
    return this->fieldOfViewAngle;
}

void Camera::translateBy(const Eigen::Vector3f &position) {
    this->cameraPosition += position;
}

void Camera::translateByAngle(float angleForYAxis, float angleForXAxis) {
    Vector3f upVector(0., 1., 0.);
    Vector3f cameraDirection = (cameraPosition - cameraTarget).normalized();
    Vector3f cameraRight = (upVector.cross(cameraDirection).normalized());
    Vector3f cameraUp = cameraDirection.cross(cameraRight);


}

MatrixXf Camera::getView() {
    Vector3f upVector(0., 1., 0.);
    Vector3f cameraDirection = (cameraPosition - cameraTarget).normalized();
    Vector3f cameraRight = (upVector.cross(cameraDirection).normalized());
    Vector3f cameraUp = cameraDirection.cross(cameraRight);

    Matrix4f rotate;
    rotate << cameraRight[0], cameraRight[1], cameraRight[2], 0.,
            cameraUp[0], cameraUp[1], cameraUp[2], 0.,
            cameraDirection[0], cameraDirection[1], cameraDirection[2], 0.,
            0.,   0.,    0.,  1.0;

    Matrix4f translate;
    translate << 1.0, 0.0, 0.0, -cameraPosition[0],
            0.0, 1.0, 0.0, -cameraPosition[1],
            0.0, 0.0, 1.0, -cameraPosition[2],
            0.0, 0.0, 0.0, 1.0;

    return rotate * translate;

}

MatrixXf Camera::getProjection() {
    float theta = this->fieldOfViewAngle;


    float n = this->near;
    float f = this->far;

    float r;
    float l;

    float t;
    float b;
    float aspect = this->aspectRatio;

    t = tan(theta/2) * abs(n);
    b = -t;

    r = aspect * t;
    l = -r;

    if (this->projectionType == PROJECTION_ORTHOGRAPHIC) {
        Eigen::Matrix4f orthographic(4, 4);
        orthographic <<
                     2 / (r - l), 0., 0., -(r + l) / (r - l),
                0., 2 / (t - b), 0., -(t + b) / (t - b),
                0., 0., 2 / (abs(n) - abs(f)), -(n + f) / (abs(n) - abs(f)),
                0., 0., 0., 1.;
        return orthographic;
    } else if (this->projectionType == PROJECTION_PERSPECTIVE) {
        Eigen::Matrix4f perspective(4, 4);
        perspective <<
                    2 * abs(n) / (r - l), 0., (r + l) / (r - l), 0.,
                0., (2 * abs(n)) / (t - b), (t + b) / (t - b), 0.,
                0., 0., (abs(f) + abs(n)) / (abs(n) - abs(f)), (2 * abs(f) * abs(n)) / (abs(n) - abs(f)),
                0., 0., -1., 0;
        return perspective;
    } else {
        throw runtime_error(&"Invalid projection type: " [ this->projectionType]);
    }

}