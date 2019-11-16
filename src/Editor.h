//
// Created by Ujjwal Chadha on 10/15/19.
//

#ifndef EDITOR_H
#define EDITOR_H

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <Eigen/Core>
#include "Helpers.h"

#ifdef _WIN32
#  include <windows.h>
#  undef max
#  undef min
#  undef DrawText
#endif

#ifndef __APPLE__
#  define GLEW_STATIC
#  include <GL/glew.h>
#endif

#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#   define __gl_h_ /* Prevent inclusion of the old gl.h */
#else
#   ifdef _WIN32
#       include <windows.h>
#   endif
#   include <GL/gl.h>
#endif

class Editor {
    private:
        GLFWwindow* window;
        Program* currentProgram;
        VertexArrayObject* currentVAO;
        VertexBufferObject* currentPositionsVBO;
        VertexBufferObject* currentColorsVBO;

    public:
        GLFWwindow* initGLFWwindow();
        bool shouldClose();

        void initAndBindVAO(VertexArrayObject& vao);
        void bindVAO(VertexArrayObject& vao);
        VertexArrayObject& getCurrentVAO();

        void initAndBindPositionsVBO(VertexBufferObject& VBO);
        void bindPositionsVBO(VertexBufferObject& VBO);
        void updatePositonsVBO(const Eigen::MatrixXf& M);
        VertexBufferObject& getPositionsVBO();

        void initAndBindColorsVBO(VertexBufferObject& VBO);
        void bindColorsVBO(VertexBufferObject& VBO);
        void updateColorsVBO(const Eigen::MatrixXf& M);
        VertexBufferObject& getColorsVBO();

        void setCurrentProgram(Program& program);
        void loadShadersIntoCurrentProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& fragmentDataName);
        void loadAndBindShadersIntoCurrentProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& fragmentDataName);

        Program& getCurrentProgram();

        void setKeyCallback(GLFWkeyfun callback);
        void setMouseButtonCallback(GLFWmousebuttonfun callback);
        void setFrameSizeCallback(GLFWframebuffersizefun callback);
        void setCursorPositionCallback(GLFWcursorposfun callback);

        void getCursorPositionAsWorldCoordinates(double* xWorld, double* yWorld);
        Eigen::Vector2f getCursorPositionAsWorldCoordinates();
};


#endif
