//
// Created by Ujjwal Chadha on 11/2/19.
//

#ifndef UNTITLED_WORLD_H
#define UNTITLED_WORLD_H

#include "Mesh.h"
#include "Camera.h"
#include <iostream>
#include <Eigen/Core>
#include <vector>
#include "Utils.h"

class World {
private:
    std::vector<std::reference_wrapper<Mesh>> meshes;
    std::vector<reference_wrapper<Camera>> cameras;
    int viewCamera = 0;
    int selectedMeshIndex = -1;

public:
    void addMesh(Mesh& mesh);

    void addCamera(Camera& camera);

    std::vector<reference_wrapper<Mesh>> getMeshes();

    std::vector<reference_wrapper<Camera>> getCameras();

    void setViewCamera(int cameraNumber);

    Camera& getViewCamera();

    void setSelectedMeshIndex(int meshIndex);

    int getSelectedMeshIndex();
};


#endif //UNTITLED_WORLD_H
