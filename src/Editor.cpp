#include "Editor.h"
#include "Helpers.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

using namespace std;

GLFWwindow* Editor::initGLFWwindow() {

    // Initialize the library
    if (!glfwInit())
        throw runtime_error("Failed to initialize window!");

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw runtime_error("Failed to create window!");
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

#ifndef __APPLE__
    glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    return window;
}

bool Editor::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Editor::setKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
}

void Editor::setMouseButtonCallback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(window, callback);
}

void Editor::setFrameSizeCallback(GLFWframebuffersizefun callback) {
    glfwSetFramebufferSizeCallback(window, callback);
}

void Editor::setCursorPositionCallback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(window, callback);
}

void Editor::initAndBindVAO(VertexArrayObject& vao) {
    vao.init();
    bindVAO(vao);
}

void Editor::bindVAO(VertexArrayObject& vao) {
    vao.bind();
    this->currentVAO = &vao;
}

VertexArrayObject& Editor::getCurrentVAO() {
    return *currentVAO;
}

VertexBufferObject& Editor::getPositionsVBO() {
    return *currentPositionsVBO;
}

void Editor::initAndBindPositionsVBO(VertexBufferObject& VBO) {
    VBO.init();
    bindPositionsVBO(VBO);
}

void Editor::bindPositionsVBO(VertexBufferObject& VBO) {
    VBO.bind();
    this->currentPositionsVBO = &VBO;
}

void Editor::updatePositonsVBO(const Eigen::MatrixXf &M) {
    currentPositionsVBO->update(M);
}

VertexBufferObject& Editor::getColorsVBO() {
    return *currentColorsVBO;
}

void Editor::initAndBindColorsVBO(VertexBufferObject& VBO) {
    VBO.init();
    bindColorsVBO(VBO);
}

void Editor::bindColorsVBO(VertexBufferObject& VBO) {
    VBO.bind();
    this->currentColorsVBO = &VBO;
}

void Editor::updateColorsVBO(const Eigen::MatrixXf &M) {
    currentColorsVBO->update(M);
}

void Editor::setCurrentProgram(Program &program) {
    currentProgram = &program;
}

void Editor::loadShadersIntoCurrentProgram(const std::string &vertexShaderSource,
                                           const std::string &fragmentShaderSource,
                                           const std::string &fragmentDataName) {

    currentProgram->init(vertexShaderSource, fragmentShaderSource, fragmentDataName);
}

void Editor::loadAndBindShadersIntoCurrentProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource,
                                                  const std::string &fragmentDataName) {
    loadShadersIntoCurrentProgram(vertexShaderSource, fragmentShaderSource, fragmentDataName);
    currentProgram->bind();
}

Program& Editor::getCurrentProgram() {
    return *currentProgram;
}

void Editor::getCursorPositionAsWorldCoordinates(double *xWorld, double *yWorld) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    *xWorld = ((xpos/double(width))*2)-1;
    *yWorld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw
}

Eigen::Vector2f Editor::getCursorPositionAsWorldCoordinates() {
    double x, y;
    getCursorPositionAsWorldCoordinates(&x, &y);
    return Eigen::Vector2f(x, y);
}

