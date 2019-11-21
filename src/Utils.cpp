//
// Created by Ujjwal Chadha on 11/2/19.
//

#include "Utils.h"

#include <iostream>
#include <vector>

std::vector<std::string> Utils::splitString(std::string s, const std::string &delimiter) {
    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        tokens.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    if (!s.empty()) {
        tokens.push_back(s);
    }
    return tokens;
}

Eigen::MatrixXf Utils::generateScaleMatrix(float factor) {
    Eigen::MatrixXf transform(4, 4);
    transform <<  factor,    0.,   0.,   0.,
                    0., factor,    0.,   0.,
                    0.,   0., factor,    0.,
                    0.,   0.,   0.,      1.;
    return transform;
}

Eigen::MatrixXf Utils::generateRotationMatrix(int axis, float radians) {
    Eigen::MatrixXf transform(4, 4);
    if (axis == Utils::AXIS_Z){
        transform <<
                 cos(radians),      sin(radians),  0.,  0.,
                -sin(radians),     cos(radians),  0.,  0.,
                0.,                     0.,                 1.,  0.,
                0.,                     0.,                 0.,  1.;
    } else if(axis == Utils::AXIS_X){
        transform <<
                 1.,    0.,                  0.,                 0.,
                0.,    cos(radians),   sin(radians),  0.,
                0.,    -sin(radians),  cos(radians),  0.,
                0.,    0.,                  0.,                 1.;

    } else if(axis == Utils::AXIS_Y){
        transform <<
                 cos(radians),  0.,  -sin(radians),   0.,
                0.,                 1.,  0.,                   0.,
                sin(radians),  0.,  cos(radians),    0.,
                0.,                 0.,  0.,                   1.;
    }
    return transform;
}

Eigen::MatrixXf Utils::generateTranslationMatrix(const Eigen::Vector3f &translateBy) {
    Eigen::MatrixXf transform(4, 4);
    transform <<    1,    0.,   0.,   translateBy(0),
                    0.,   1,    0.,   translateBy(1),
                    0.,   0.,   1,    translateBy(2),
                    0.,   0.,   0.,   1.;
    return transform;
}

Eigen::MatrixXf Utils::generateScaleAboutPointMatrix(const Eigen::Vector3f& point, float factor) {
    Eigen::MatrixXf transform(4, 4);
    transform <<    factor,    0.,      0.,   point(0) * (1 - factor),
                    0.,     factor,     0.,   point(1) * (1 - factor),
                    0.,       0.,    factor,  point(2) * (1 - factor),
                    0.,       0.,       0.,       1;
    return transform;
}

bool Utils::rayTriangleIntersect(const Eigen::Vector3f &rayOrigin, const Eigen::Vector3f &rayVector, const Eigen::Vector3f &vertex0,
                                 const Eigen::Vector3f &vertex1, const Eigen::Vector3f &vertex2, float& t) {
    const float EPSILON = 0.00001;

    Eigen::Vector3f edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;

    h = rayVector.cross(edge2);
    a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = rayOrigin - vertex0;
    u = f * s.dot(h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.cross(edge1);
    v = f * rayVector.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    t = f * edge2.dot(q);
    if (t > EPSILON && t < 1/EPSILON) // ray intersection
    {
//        outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

Eigen::MatrixXf Utils::generateRotateAboutPointMatrix(int axis, float radians, const Eigen::Vector3f& center) {
//    Eigen::MatrixXf transform(4, 4);
//    if (axis == Utils::AXIS_Z){
//        float transX = (-cos(radians)*center(0)) - (sin(radians) * center(1)) + (2 * center(0));
//        float transY = (sin(radians)*center(0)) - (cos(radians) * center(1)) + (2 * center(1));
//        transform <<
//                  cos(radians),      sin(radians),          0.,            transX,
//                -sin(radians),       cos(radians),          0.,            transY,
//                0.,                     0.,                 1.,          center(2),
//                0.,                     0.,                 0.,              1.;
//    } else {
//        throw std::runtime_error("Invalid Axis");
//    }
//    return transform;
    return generateTranslationMatrix(center) * generateRotationMatrix(axis, radians) * generateTranslationMatrix(-1*center);
}

