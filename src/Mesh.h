//
// Created by Ujjwal Chadha on 11/2/19.
//

#ifndef UNTITLED_MESH_H
#define UNTITLED_MESH_H

#include <Eigen/Core>
#include "Utils.h"

using namespace Eigen;
using namespace std;

class Mesh {
private:
    MatrixXf vertices;
    MatrixXf faces;
    MatrixXf triangleVertices;
    MatrixXf faceNormals;
    MatrixXf vertexNormals;

    MatrixXf model;
    Vector3f color;
    RenderType renderType;

    static MatrixXf calculateTriangleVertices(const MatrixXf& faces, const MatrixXf& vertices);
    static MatrixXf calculateFaceNormals(const MatrixXf& faces, const MatrixXf& vertices, MatrixXf triangleVertices);
    static MatrixXf calculateVertexNormals(const MatrixXf& faces, const MatrixXf& vertices,
            const MatrixXf& triangleVertices, const MatrixXf& faceNormals);
    static Vector3f calculateBarycenter(const MatrixXf& faces, const MatrixXf& vertices);

public:
    Mesh(const MatrixXf& vertices, const MatrixXf& faces, const Vector3f& color, const RenderType& renderType);
    Mesh(const MatrixXf& vertices, const MatrixXf& faces);
    ~Mesh();
    static Mesh fromOffFile(const string& filePath);
    static Mesh fromOffFile(const string& filePath, const Vector3f& color, const RenderType& renderType);

    void translate(const Vector3f& translateBy);
    void scale(float factor);
    void rotate(int axis, float radians);

    void scaleToUnitCube();

    Vector3f getTranslation();

    MatrixXf getVertices();
    MatrixXf getFaces();
    MatrixXf getTriangleVertices();
    MatrixXf getFaceNormals();
    MatrixXf getVertexNormals();
    MatrixXf getModel();
    Vector3f getColor();
    RenderType getRenderType();

    void setRenderType(const RenderType& renderType);
    void setColor(const Vector3f& color);
};


#endif //UNTITLED_MESH_H
