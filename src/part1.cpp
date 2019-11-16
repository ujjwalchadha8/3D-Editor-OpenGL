//
// Created by Ujjwal Chadha on 10/15/19.
//
#include "Helpers.h"
#include "Editor.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif


// Linear Algebra Library
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <math.h>

#define PI 3.14

bool MOVE_DOWN = true;
bool MOVE_UP = false;
bool MOVE_LEFT = false;
bool MOVE_RIGHT = true;

enum Mode {
    MODE_TRANSLATION,
    MODE_INSERTION,
    MODE_DELETION,
    MODE_COLOR,
    MODE_ANIMATION
};

enum SubMode {
    NONE,
    TRANSLATION_SUB_MODE_MOVE,
    ANIMATION_SUB_MODE_TRANSLATING
};

Mode mode = MODE_INSERTION;
SubMode subMode = NONE;
Editor editor = Editor();
Eigen::MatrixXf positions(0, 0);
Eigen::MatrixXf view(4, 4);
std::vector<Eigen::MatrixXf> models;
Eigen::MatrixXf identity(4, 4);
Eigen::MatrixXf colors(3, 3);
long selectedTriangle = -1;
long selectedVertexPosition = -1;

long animateTriangle;
Eigen::Vector2f animateTriangleFrom;
Eigen::Vector2f animateTriangleTo;
auto animateTriangleStartTime = std::chrono::high_resolution_clock::now();

bool isPointInTriangle(Eigen::Vector2f s, Eigen::Vector4f a, Eigen::Vector4f b, Eigen::Vector4f c) {
    float as_x = s[0]-a[0];
    float as_y = s[1]-a[1];

    bool s_ab = (b[0]-a[0])*as_y-(b[1]-a[1])*as_x > 0;

    if((c[0]-a[0])*as_y-(c[1]-a[1])*as_x > 0 == s_ab) return false;

    if((c[0]-b[0])*(s[1]-b[1])-(c[1]-b[1])*(s[0]-b[0]) > 0 != s_ab) return false;

    return true;
}

void updateSelectedTriangle(const Eigen::Vector2f& newClickPoint) {
    long state = (positions.cols() - 1) % 3;
    for (long i = 0; i < positions.cols() - state - 3; i += 3) {

        Eigen::Vector4f a(positions.col(i)(0), positions.col(i)(1), 0.0, 1.0);
        Eigen::Vector4f b(positions.col(i)(0), positions.col(i+1)(1), 0.0, 1.0);
        Eigen::Vector4f c(positions.col(i)(0), positions.col(i+2)(1), 0.0, 1.0);

        a = view * models.at(i/3) * a;
        b = view * models.at(i/3) * b;
        c = view * models.at(i/3) * c;

        std::cout<<view * models.at(i/3)<<std::endl;

        if(isPointInTriangle(newClickPoint, a, b, c)) {
            selectedTriangle = i;
            return;
        }
    }
    selectedTriangle = -1;
}

void updateSelectedVertexPosition(const Eigen::Vector2f& newClickPoint) {
    long minDistanceVertex = -1;
    double minDistance = INFINITY;
    for (long i = 0; i < positions.cols() - 1; i++) {
        double distanceFromPositionToClickPoint = sqrt(pow(newClickPoint[0] - positions.col(i)[0], 2) + pow(newClickPoint[1] - positions.col(i)[1], 2));
        if (distanceFromPositionToClickPoint < minDistance) {
            minDistance = distanceFromPositionToClickPoint;
            minDistanceVertex = i;
        }
    }
    selectedVertexPosition = minDistanceVertex;
}

Eigen::Vector2f getTriangleCentroid(long triangle) {
    if (triangle < 0 || triangle >= positions.cols()) {
        throw std::runtime_error("Invalid triangle");
    }
    return (positions.col(triangle) + positions.col(triangle + 1) + positions.col(triangle + 2)) / 3;
}

void translateTriangle(long triangle, const Eigen::Vector2f& translateBy) {
    positions.col(triangle) += translateBy;
    positions.col(triangle + 1) += translateBy;
    positions.col(triangle + 2) += translateBy;
}

void translateTriangleTo(long triangle, const Eigen::Vector2f& translateTo) {
    translateTriangle(triangle, translateTo - getTriangleCentroid(triangle));
}

void scaleTriangle(long triangle, double factor) {
    if (selectedTriangle == -1) {
        return;
    }
    Eigen::Vector2f triangleCentroid = getTriangleCentroid(triangle);
    translateTriangle(triangle, -1*triangleCentroid);

    positions.block<2, 3>(0, triangle) *= factor;

    translateTriangle(triangle, triangleCentroid);
}

void rotateTriangle(long triangle, double angleInDegrees) {
    if (selectedTriangle == -1) {
        return;
    }
    double angleInRadians = angleInDegrees * (PI / 180);
    Eigen::Vector2f triangleCentroid = getTriangleCentroid(triangle);
    translateTriangle(triangle, -1*triangleCentroid);

    Eigen::MatrixXf rotationMatrix(2, 2);
    rotationMatrix.col(0) << cos(angleInRadians), sin(angleInRadians);
    rotationMatrix.col(1) << -sin(angleInRadians), cos(angleInRadians);

    positions.block<2, 3>(0, triangle) = rotationMatrix * positions.block<2, 3>(0, triangle) ;
    translateTriangle(triangle, triangleCentroid);
}

void updateVertexColor(long vertex, Eigen::Vector3f& color) {
    colors.col(vertex) << color;
    editor.getColorsVBO().update(colors);
}

Eigen::Vector4f getWorldCoordinatesShift(GLFWwindow* window, bool isVertical) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    Eigen::Vector4f screenCoords(isVertical ? 0.0 : (.20 * width), isVertical ? (.20 * height) : 0.0, 0.0, 0.0);
    Eigen::Vector4f canonicalCoords(((screenCoords[0] / width) * 2 - 1), (screenCoords[1] / height) * 2 - 1, 0.0, 0.0);
    return view.inverse() * canonicalCoords;
}

void moveVertical(bool direction, GLFWwindow* window) {
    Eigen::Vector4f worldCoordsShift = getWorldCoordinatesShift(window, true);
    if (direction == MOVE_DOWN){
        view(1,3) += worldCoordsShift[1];
    } else {
        view(1,3) -= worldCoordsShift[1];
    }
}

void moveHorizontal(bool direction, GLFWwindow* window) {
    Eigen::Vector4f p_world = getWorldCoordinatesShift(window, false);
    if (direction == MOVE_RIGHT) {
        view(0,3) -= p_world[0];
    } else {
        view(0,3) += p_world[0] ;
    }
}

void zoomInBy(float factor) {
    view(0,0) *= factor;
    view(1,1) *= factor;
    view(2,2) *= factor;
}

void mouseButtonListener(GLFWwindow* window, int button, int action, int mods) {

    Eigen::Vector2f clickedPoint = editor.getCursorPositionAsWorldCoordinates();

    switch (mode) {
        case MODE_INSERTION:
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                positions.conservativeResize(2, positions.cols() + 1);
                positions.col(positions.cols()-1) = clickedPoint;
                editor.getPositionsVBO().update(positions);

                colors.conservativeResize(3, positions.cols() + 1);
                colors.col(positions.cols()-1) << 1.0, 0.0, 0.0;
                editor.getColorsVBO().update(colors);

                selectedTriangle = -1;

                long state = (positions.cols() - 1) % 3;
                if (state == 1) {
                    models.push_back(view.inverse());
                }
            }
            break;
        case MODE_TRANSLATION:
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                updateSelectedTriangle(clickedPoint);
                subMode = TRANSLATION_SUB_MODE_MOVE;
            } else if (action == GLFW_RELEASE) {
                subMode = NONE;
            }
            break;
        case MODE_DELETION:
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                updateSelectedTriangle(clickedPoint);
                if (selectedTriangle != -1) {
                    positions.col(selectedTriangle) << 0, 0;
                    positions.col(selectedTriangle + 1) << 0, 0;
                    positions.col(selectedTriangle + 2) << 0, 0;
                }
            }
        case MODE_COLOR:
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                updateSelectedVertexPosition(clickedPoint);
            }
            break;
        case MODE_ANIMATION:
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                if (selectedTriangle == -1) {
                    updateSelectedTriangle(clickedPoint);
                } else {
                    subMode = ANIMATION_SUB_MODE_TRANSLATING;
                    animateTriangle = selectedTriangle;
                    animateTriangleFrom = getTriangleCentroid(selectedTriangle);
                    animateTriangleTo = Eigen::Vector2f(clickedPoint);
                    animateTriangleStartTime = std::chrono::high_resolution_clock::now();
//                    translateTriangleTo(selectedTriangle, clickedPoint);
                }
            }
    }

}

void keyListener(GLFWwindow* window, int key, int scancode, int action, int mods) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    switch (key) {
        case GLFW_KEY_O:
            mode = MODE_TRANSLATION;
            subMode = NONE;
            break;
        case GLFW_KEY_I:
            mode = MODE_INSERTION;
            subMode = NONE;
            break;
        case GLFW_KEY_P:
            mode = MODE_DELETION;
            subMode = NONE;
            break;
        case GLFW_KEY_C:
            mode = MODE_COLOR;
            subMode = NONE;
            break;
        case GLFW_KEY_X:
            mode = MODE_ANIMATION;
            subMode = NONE;
//            translateTriangle(selectedTriangle, Eigen::Vector2f(0, 0) - getTriangleCentroid(selectedTriangle));
            break;
        case GLFW_KEY_K:
            if (mode == MODE_TRANSLATION && selectedTriangle != -1 && action == GLFW_PRESS) {
                scaleTriangle(selectedTriangle, 1.25);
            }
            break;
        case GLFW_KEY_L:
            if (mode == MODE_TRANSLATION && selectedTriangle != -1 && action == GLFW_PRESS) {
                scaleTriangle(selectedTriangle, 0.75);
            }
            break;
        case GLFW_KEY_H:
            if (mode == MODE_TRANSLATION && selectedTriangle != -1 && action == GLFW_PRESS) {
                rotateTriangle(selectedTriangle, -10);
            }
            break;
        case GLFW_KEY_J:
            if (mode == MODE_TRANSLATION && selectedTriangle != -1 && action == GLFW_PRESS) {
                rotateTriangle(selectedTriangle, 10);
            }
        case GLFW_KEY_1:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(1.0, 1.0, 0.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_2:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(1.0, 0.0, 1.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_3:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(0.0, 1.0, 1.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_4:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(0.0, 0.0, 0.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_5:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(1.0, 1.0, 1.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_6:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(0.0, 1.0, 0.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_7:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(0.0, 0.0, 1.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_8:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(1.0, 0.0, 0.0);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_9:
            if (mode == MODE_COLOR && selectedVertexPosition != -1) {
                Eigen::Vector3f selectedColor(0.5, 0.3, 0.2);
                updateVertexColor(selectedVertexPosition, selectedColor);
            }
            break;
        case GLFW_KEY_W:
//            cout << "Shifting DOWN" << endl;
            if (action == GLFW_PRESS)
                moveVertical(MOVE_DOWN, window);
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS)
                moveHorizontal(MOVE_RIGHT, window);
            break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS)
                moveVertical(MOVE_UP, window);
            break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS)
                moveHorizontal(MOVE_LEFT, window);
            break;
        case GLFW_KEY_MINUS:
            if (action == GLFW_PRESS)
                zoomInBy(0.8);
            break;
        case GLFW_KEY_EQUAL:
            if (action == GLFW_PRESS)
                zoomInBy(1.2);
            break;
        default:
            break;
    }
}

int main1() {
    GLFWwindow* window = editor.initGLFWwindow();

    VertexArrayObject vao;
    editor.initAndBindVAO(vao);

    VertexBufferObject vbo;
    editor.initAndBindPositionsVBO(vbo);
    positions.resize(2, 1);
    positions << 0,  0;
    vbo.update(positions);


    VertexBufferObject vboColor;
    editor.initAndBindColorsVBO(vboColor);
    colors.resize(3, 1);
    colors << 1.0, 0.0, 0.0;
    vboColor.update(colors);

    const GLchar* vertex_shader =
            "#version 150 core\n"
            "in vec2 position;"
            "in vec3 color;"
            "uniform mat4 view;"
            "uniform mat4 model;"
            "out vec3 f_color;"
            "void main()"
            "{"
            "    gl_Position = view * model * vec4(position, 0.0, 1.0);"
            "    f_color = color;"
            "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
            "in vec3 f_color;"
            "out vec4 outColor;"
            "uniform int isSelected;"
            "void main()"
            "{"
            "    if (isSelected == 0) outColor = vec4(f_color, 1.0);"
            "    else outColor = vec4(0.0, 0.0, 1.0, 1.0);"
            "}";

    Program program;
    editor.setCurrentProgram(program);
    editor.loadAndBindShadersIntoCurrentProgram(vertex_shader, fragment_shader, "outColor");
    editor.getCurrentProgram().bindVertexAttribArray("position", vbo);
    editor.getCurrentProgram().bindVertexAttribArray("color", vboColor);
    glUniform3f(editor.getCurrentProgram().uniform("triangleColor"), 1.0, 0.0, 0.0);

    editor.setKeyCallback(keyListener);
    editor.setKeyCallback(keyListener);
    editor.setMouseButtonCallback(mouseButtonListener);

    view << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    identity << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    Eigen::Vector2f lastMousePosition;
    while (!editor.shouldClose()) {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Eigen::Vector2f currentMousePosition = editor.getCursorPositionAsWorldCoordinates();
        positions.col(positions.cols() - 1) << currentMousePosition;
        editor.updatePositonsVBO(positions);

        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());

        long state = (positions.cols() - 1) % 3;
        for (long i = 0; i < positions.cols() - state - 3; i += 3) {
            if ((mode == MODE_ANIMATION || mode == MODE_TRANSLATION) and selectedTriangle == i) {
                glUniform1i(editor.getCurrentProgram().uniform("isSelected"), 1);
                if (subMode == TRANSLATION_SUB_MODE_MOVE) {
                    positions.col(i) += currentMousePosition - lastMousePosition;
                    positions.col(i+1) += currentMousePosition - lastMousePosition;
                    positions.col(i+2) += currentMousePosition - lastMousePosition;
                } else if (subMode == ANIMATION_SUB_MODE_TRANSLATING) {
                    auto t_now = std::chrono::high_resolution_clock::now();
                    float delta = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - animateTriangleStartTime).count();
                    float animationDuration = (animateTriangleTo - animateTriangleFrom).norm();
                    if (delta >= animationDuration) {
                        subMode = NONE;
                    }
                    translateTriangleTo(animateTriangle, animateTriangleFrom + (delta / animationDuration) * (animateTriangleTo - animateTriangleFrom));
                }
            } else {
                glUniform1i(editor.getCurrentProgram().uniform("isSelected"), 0);
            }
//            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &model(0, md));
//            std::cout<<"DRAWN at"<<i/3<<std::endl<<models.at(i/3)<<std::endl;

            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, models.at(i/3).data());
            glDrawArrays(GL_TRIANGLES, i, 3);
//            md += 4;
        }
//        std::cout<<std::endl;
        if (state == 1) {
//            std::cout<<"Drawing at"<<models.size()-1<<std::endl<<models.at(models.size()-1);
            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, models.at(models.size()-1).data());
            glDrawArrays(GL_LINE_STRIP, positions.cols() - 2, 2);
        } else if (state == 2) {
            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, models.at(models.size()-1).data());
            glDrawArrays(GL_LINE_LOOP, positions.cols() - 3, 3);
        }

        lastMousePosition = currentMousePosition;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    vbo.free();
    vao.free();
    vboColor.free();
    program.free();
    
    return 0;
}
