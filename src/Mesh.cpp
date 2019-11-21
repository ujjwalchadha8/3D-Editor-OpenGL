//
// Created by Ujjwal Chadha on 11/2/19.
//

#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include "Utils.h"
#include <unordered_map>

using namespace std;
using namespace Eigen;

Mesh::Mesh() {

}

Mesh::Mesh(const MatrixXf& vertices, const MatrixXf& faces):
        Mesh(vertices, faces, Eigen::Vector3f(0.0, 1.1, 2.2), WIREFRAME) {
}

Mesh::Mesh(const MatrixXf &vertices, const MatrixXf &faces, const Vector3f& color, const RenderType& renderType) {
    this->vertices = vertices;
    this->faces = faces;
    this->model = MatrixXf(4, 4);
    this->model <<  1,    0.,   0.,   0.,
            0.,   1,    0.,   0.,
            0.,   0.,   1,    0.,
            0.,   0.,   0.,   1.;

    this->color = color;
    this->renderType = renderType;

    this->triangleVertices = calculateTriangleVertices(faces, vertices);
    this->faceNormals = calculateFaceNormals(faces, vertices, this->triangleVertices);
    this->vertexNormals = calculateVertexNormals(faces, vertices, this->triangleVertices, this->faceNormals);
}

Mesh Mesh::fromOffFile(const string &filePath) {
    return fromOffFile(filePath, Eigen::Vector3f(0.0, 1.1, 2.2), WIREFRAME);
}

Mesh Mesh::fromOffFile(const string& filepath, const Vector3f& color, const RenderType& renderType) {
    ifstream inputFile;
    inputFile.open(filepath);

    if (!inputFile) {
        throw std::runtime_error("Error opening data file: " + filepath);
    } else {
        string line;
        getline(inputFile, line);
        getline(inputFile, line);
        vector<string> meta = Utils::splitString(line, " ");
        int numVertices = stoi(meta.at(0));
        int numFaces = stoi(meta.at(1));

        MatrixXf vertices = MatrixXf::Zero(3, numVertices);
        for (int i = 0; i < numVertices; i++) {
            getline(inputFile, line);
            vector<string> vertexString = Utils::splitString(line, " ");

            vertices(0, i) = (stof(vertexString[0]));
            vertices(1, i) = (stof(vertexString[1]));
            vertices(2, i) = stof(vertexString[2]);
        }

        MatrixXf faces = MatrixXf::Zero(3, numFaces);
        for (int i = 0; i < numFaces; i++) {
            getline(inputFile, line);
            vector<string> faceString = Utils::splitString(line, " ");

            faces(0, i) = stoi(faceString[1]);
            faces(1, i) = stoi(faceString[2]);
            faces(2, i) = stoi(faceString[3]);
        }
        inputFile.close();

        Vector3f baryCenter = calculateBarycenter(faces, vertices);
        for (long i = 0; i < vertices.cols(); i++) {
            vertices.col(i) << vertices.col(i) - baryCenter;
        }
        return Mesh(vertices, faces, color, renderType);
    }
}

Vector3f Mesh::calculateBarycenter(const MatrixXf &faces, const MatrixXf &vertices) {
    Vector3f centroid(0., 0., 0.);
    for (long faceNumber = 0; faceNumber < faces.cols(); faceNumber++) {
        const Vector3f& a = vertices.col(faces(0, faceNumber));
        const Vector3f& b = vertices.col(faces(1, faceNumber));
        const Vector3f& c = vertices.col(faces(2, faceNumber));

        const Vector3f& center = (a + b + c) / 3;
        centroid = centroid + center;
    }
    return centroid / faces.cols();
}

MatrixXf Mesh::calculateTriangleVertices(const MatrixXf &faces, const MatrixXf &vertices) {
    MatrixXf triangleVertices = MatrixXf::Zero(3, faces.cols() * faces.rows());
    for (long faceNumber = 0; faceNumber < faces.cols(); faceNumber++) {
        for (long faceVertexNumber = 0; faceVertexNumber < faces.rows(); faceVertexNumber++) {
            int vertexNumber = faces(faceVertexNumber, faceNumber);
            triangleVertices.col((faces.rows()*faceNumber) + faceVertexNumber) << vertices.col(vertexNumber);
        }
    }
    return triangleVertices;
}

MatrixXf Mesh::calculateFaceNormals(const MatrixXf& faces, const MatrixXf& vertices, MatrixXf triangleVertices) {
    MatrixXf normals = MatrixXf::Zero(3, triangleVertices.cols());
    for (long i = 0; i < triangleVertices.cols(); i += 3) {
        Vector3f a = triangleVertices.col(i);
        Vector3f b = triangleVertices.col(i + 1);
        Vector3f c = triangleVertices.col(i + 2);

        Vector3f normal = ((b - a).cross(c - a)).normalized();
        normals.col(i) << normal;
        normals.col(i + 1) << normal;
        normals.col(i + 2) << normal;
    }
    return normals;
}

MatrixXf Mesh::calculateVertexNormals(const MatrixXf& faces, const MatrixXf& vertices, const MatrixXf& triangleVertices,
        const MatrixXf& faceNormals) {

    unordered_map<int, vector<int>> vertexNumberToFaceNumbersMap;
    for (long faceNumber = 0; faceNumber < faces.cols(); faceNumber++) {
        for (long faceVertexNumber = 0; faceVertexNumber < faces.rows(); faceVertexNumber++) {
            int vertexNumber = faces(faceVertexNumber, faceNumber);
            if (vertexNumberToFaceNumbersMap.find(vertexNumber) == vertexNumberToFaceNumbersMap.end()) {
                vertexNumberToFaceNumbersMap[vertexNumber] = std::vector<int>();
            }
            vertexNumberToFaceNumbersMap[vertexNumber].push_back(faceNumber);
        }
    }

    MatrixXf normals(3, triangleVertices.cols());
    for (long faceNumber = 0; faceNumber < faces.cols(); faceNumber++) {
        for (long faceVertexNumber = 0; faceVertexNumber < faces.rows(); faceVertexNumber++) {
            int vertexNumber = faces(faceVertexNumber, faceNumber);
            vector<int> facesAdjascentToVertex =  vertexNumberToFaceNumbersMap[vertexNumber];
            Vector3f vertexNormal(0.0, 0.0, 0.0);
            for (int face: facesAdjascentToVertex) {
                Vector3f faceNormal = faceNormals.col(face*3);
                vertexNormal += faceNormal;
            }
            normals.col((3*faceNumber) + faceVertexNumber) = vertexNormal.normalized();
        }
    }
    return normals;
}

void Mesh::scaleToUnitCube() {
    float xmax = -999999, xmin = 99999;
    float ymax = -999999, ymin = 99999;
    float zmax = -999999, zmin = 99999;
    for (long i = 0; i < getVertices().cols(); i++) {
        Vector3f vertex = getVertices().col(i);
        if (vertex(0) < xmin) {
            xmin = vertex(0);
        }
        if (vertex(0) > xmax) {
            xmax = vertex(0);
        }
        if (vertex(1) < ymin) {
            ymin = vertex(1);
        }
        if (vertex(1) > ymax) {
            ymax = vertex(1);
        }
        if (vertex(2) < zmin) {
            zmin = vertex(2);
        }
        if (vertex(2) > zmax) {
            zmax = vertex(2);
        }
    }
    scale(1 / max(max(xmax-xmin, ymax-ymin), zmax-zmin));
}

RenderType Mesh::getRenderType() {
    return renderType;
}

void Mesh::setRenderType(const RenderType& renderType) {
    this->renderType = renderType;
}

Vector3f Mesh::getColor() {
    return this->color;
}

void Mesh::setColor(const Vector3f &color) {
    this->color = color;
}

void Mesh::translate(const Vector3f& translateBy) {
    this->model << Utils::generateTranslationMatrix(translateBy) * this->model;
}

void Mesh::scale(float factor) {
    this->model << Utils::generateScaleAboutPointMatrix(getTranslation(), factor) * this->model;
}

void Mesh::rotate(int axis, float radians) {
    this->model << Utils::generateRotateAboutPointMatrix(axis, radians, getTranslation()) * this->model;
}

Vector3f Mesh::getTranslation() {
    auto res = Vector3f(this->model.block(0, 3, 3, 1));
    return res;
}

MatrixXf Mesh::getVertices() {
    return this->vertices;
}

MatrixXf Mesh::getFaces() {
    return this->faces;
}

MatrixXf Mesh::getModel() {
    return this->model;
}

MatrixXf Mesh::getTriangleVertices() {
    return this->triangleVertices;
}

MatrixXf Mesh::getFaceNormals() {
    return this->faceNormals;
}

MatrixXf Mesh::getVertexNormals() {
    return this->vertexNormals;
}

Mesh::~Mesh() {

}

