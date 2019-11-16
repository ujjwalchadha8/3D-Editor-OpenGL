//
// Created by Ujjwal Chadha on 11/2/19.
//

#include "World.h"

void World::addMesh(Mesh& mesh) {
    meshes.push_back(mesh);
}

std::vector<reference_wrapper<Mesh>> World::getMeshes() {
    return meshes;
}

void World::addCamera(Camera &camera) {
    cameras.push_back(camera);
}

std::vector<reference_wrapper<Camera>> World::getCameras() {
    return cameras;
}

void World::setViewCamera(int cameraNumber) {
    this->viewCamera = cameraNumber;
}

Camera& World::getViewCamera() {
    return cameras.at(viewCamera);
}