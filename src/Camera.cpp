//
// Created by Ujjwal Chadha on 11/2/19.
//

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(int screenWidth, int screenHeight): Camera(Vector3f(0., 0., 1.5),
        Vector3f(0., 0., -5.0), screenWidth, screenHeight) {

}

Camera::Camera(const Vector3f& eye, const Vector3f& lookAt, int screenWidth, int screenHeight) {
    this->eye = eye;
    this->lookAt = lookAt;
//    this->view = MatrixXf(4, 4);
//    this->view <<   1,    0.,   0.,   0.,
//                    0.,   1,    0.,   0.,
//                    0.,   0.,   1,    0.0,
//                    0.,   0.,   0.,   1.;
//    translateBy(position);
        float theta = (3.14/180) * 120;

    // near and far are hardcoded
        float n = -0.1;
        float f = -100.0;
    // right and left
        float r;
        float l;
    // top and bottom
        float t;
        float b;
        float aspect;

//        int width, height;
//        glfwGetWindowSize(window, &width, &height);
        aspect = screenWidth/screenHeight;

        t = tan(theta/2) * abs(n);
        b = -t;

        r = aspect * t;
        l = -r;

        // Apply projection matrix to corner points
        Eigen::Matrix4f orthographic(4,4);
        Eigen::Matrix4f perspective(4,4);
        orthographic <<
                     2/(r - l), 0., 0., -(r+l)/(r-l),
                0., 2/(t - b), 0., -(t+b)/(t-b),
                0., 0., 2/(abs(n)-abs(f)), -(n+f)/(abs(n)-abs(f)),
                0., 0., 0.,   1.;

        // perspective maps a frustrum to a unit cube
        // take  vertex from each end of the frustrum and map them to the unit cube
        perspective <<
                2*abs(n)/(r-l), 0., (r+l)/(r-l), 0.,
                0., (2 * abs(n))/(t-b), (t+b)/(t-b), 0.,
                0., 0.,  (abs(f) + abs(n))/(abs(n) - abs(f)), (2 * abs(f) * abs(n))/(abs(n) - abs(f)),
                0., 0., -1., 0;

//        if(ortho){
//         this->projection = orthographic;
//        }else{
         this->projection = perspective;
//        }
        cout<<"l: "<<l<<", r:"<<r<<", t: "<<t<<", b:"<<b<<", n: "<<n<<", f:"<<f<<endl;
//        glm::mat4 projectionGlm = glm::ortho(l, r, b, t, n, f);
//        glm::mat4 projectionGlm = glm::perspective(glm::radians(45.0f), aspect, n, f);
//        MatrixXf projection(4, 4);
//        projection <<
//                   projectionGlm[0][0], projectionGlm[0][1], projectionGlm[0][2], projectionGlm[0][3],
//            projectionGlm[1][0], projectionGlm[1][1], projectionGlm[1][2], projectionGlm[1][3],
//            projectionGlm[2][0], projectionGlm[2][1], projectionGlm[2][2], projectionGlm[2][3],
//            projectionGlm[3][0], projectionGlm[3][1], projectionGlm[3][2], projectionGlm[3][3];
//        this->projection = projection;

//    cout<<getView()<<endl;
}

void Camera::translateBy(const Eigen::Vector3f &position) {
//    this->view = Utils::generateTranslationMatrix(-1 * position) * this->view;
    this->eye += position;
//    cout<<this->eye<<endl;
//    cout<<this->getView()<<endl;
//    this->lookAt -= position;
}

Vector3f Camera::getEye() {
    return this->eye;
}

Vector3f Camera::getLookAt() {
    return this->lookAt;
}

MatrixXf Camera::getView() {
    Vector3f upVector(0., 1., 0.);
    Vector3f cameraDirection = (eye - lookAt).normalized();
    Vector3f cameraRight = (upVector.cross(cameraDirection).normalized());
    Vector3f cameraUp = cameraDirection.cross(cameraRight);

    Matrix4f rotate;
    rotate << cameraRight[0], cameraRight[1], cameraRight[2], 0.,
            cameraUp[0], cameraUp[1], cameraUp[2], 0.,
            cameraDirection[0], cameraDirection[1], cameraDirection[2], 0.,
            0.,   0.,    0.,  1.0;

    Matrix4f translate;
    translate << 1.0, 0.0, 0.0, -eye[0],
            0.0, 1.0, 0.0, -eye[1],
            0.0, 0.0, 1.0, -eye[2],
            0.0, 0.0, 0.0, 1.0;

    return (rotate * translate);
//    glm::mat4 viewGlm = glm::lookAt(glm::vec3(eye(0), eye(1), eye(2)),
//                        glm::vec3(lookAt(0), lookAt(1), lookAt(2)),
//                        glm::vec3(0.0f, 1.0f, 0.0f));
//    MatrixXf view( 4, 4);
//    view <<
//        viewGlm[0][0], viewGlm[0][1], viewGlm[0][2], viewGlm[0][3],
//        viewGlm[1][0], viewGlm[1][1], viewGlm[1][2], viewGlm[1][3],
//        viewGlm[2][0], viewGlm[2][1], viewGlm[2][2], viewGlm[2][3],
//        viewGlm[3][0], viewGlm[3][1], viewGlm[3][2], viewGlm[3][3];
//
//    return view.transpose();
}

MatrixXf Camera::getProjection() {
    return this->projection;
}