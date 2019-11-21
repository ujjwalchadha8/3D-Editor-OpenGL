//
// Created by Ujjwal Chadha on 11/2/19.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include <iostream>
#include <Eigen/Core>

class Utils {
public:
    const static int AXIS_Z = 0, AXIS_X = 1, AXIS_Y = 2;
    static std::vector<std::string> splitString(std::string s, const std::string& delimiter);
    static Eigen::MatrixXf generateScaleMatrix(float factor);
    static Eigen::MatrixXf generateRotationMatrix(int axis, float radians);
    static Eigen::MatrixXf generateTranslationMatrix(const Eigen::Vector3f& translateBy);
    static Eigen::MatrixXf generateScaleAboutPointMatrix(const Eigen::Vector3f& point, float factor);
    static Eigen::MatrixXf generateRotateAboutPointMatrix(int axis, float radians, const Eigen::Vector3f& center);
    static bool rayTriangleIntersect(
            const Eigen::Vector3f &orig, const Eigen::Vector3f &dir,
            const Eigen::Vector3f &v0, const Eigen::Vector3f &v1, const Eigen::Vector3f &v2, float& t);
};

enum RenderType {
    WIREFRAME,
    FLAT_SHADE,
    PHONG_SHADE
};

#endif //UNTITLED_UTILS_H
