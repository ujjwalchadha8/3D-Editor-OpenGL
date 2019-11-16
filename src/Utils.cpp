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

