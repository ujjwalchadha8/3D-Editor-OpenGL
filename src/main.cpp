// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include "Mesh.h"
#include "Utils.h"
#include "World.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif


// Linear Algebra Library
#include <Eigen/Core>

// Timer
#include <chrono>
#include <iostream>


World world;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Vector3f rayOrigin(0.0, 0.0, 0.0);
Vector3f rayDirection(0.0, 0.0, 0.0);

//vector<Mesh> meshes;
Mesh meshes[100] = {Mesh()};
int meshArrayTop = 0;

Vector3f screenCoordsToWorldCoords(GLFWwindow* window, Vector3d screenCoords) {
    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    double xpos = screenCoords(0);
    double ypos = screenCoords(1);
    double zpos = screenCoords(2);

    // Convert mouse position to world coordinates
    Vector4f p_screen(xpos,height-1-ypos,zpos,1);
    Vector4f p_canonical((p_screen[0]/width)*2-1,((p_screen[1]/height)*2)-1, zpos, 1);
    Vector4f p_camera = world.getViewCamera().getProjection().inverse() * p_canonical;
    Vector4f p_world = world.getViewCamera().getView().inverse() * p_camera;

    return Vector3f(p_world(0), p_world(1), p_world(2));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        Vector3f worldPoint = screenCoordsToWorldCoords(window, Vector3d(xpos, ypos, 0.0));
        Vector3f worldPoint2;
        if (world.getViewCamera().getProjectionType() == Camera::PROJECTION_PERSPECTIVE) {
            worldPoint2 = world.getViewCamera().getCameraPosition();
        } else {
            worldPoint2 = screenCoordsToWorldCoords(window, Vector3d(xpos, ypos, 1.0));
        }

        rayOrigin = worldPoint;
        rayDirection = (worldPoint - worldPoint2).normalized();

        int closestMeshIntersectedIndex = -1;
        float closestMeshDistance = 999999.0;
        for (int meshNo = 0; meshNo < world.getMeshes().size(); meshNo++) {
            Mesh mesh = world.getMeshes().at(meshNo).get();
            MatrixXf model = mesh.getModel();
            MatrixXf triangles = mesh.getTriangleVertices();
            for (long i = 0; i < triangles.cols(); i += 3) {
                Vector4f a4 = model * Vector4f(triangles.col(i)(0), triangles.col(i)(1), triangles.col(i)(2), 1.0);
                Vector4f b4 = model * Vector4f(triangles.col(i+1)(0), triangles.col(i+1)(1), triangles.col(i+1)(2), 1.0);
                Vector4f c4 = model * Vector4f(triangles.col(i+2)(0), triangles.col(i+2)(1), triangles.col(i+2)(2), 1.0);

                Vector3f a = Vector3f(a4(0), a4(1), a4(2));
                Vector3f b = Vector3f(b4(0), b4(1), b4(2));
                Vector3f c = Vector3f(c4(0), c4(1), c4(2));
                float t = -1;
                if (Utils::rayTriangleIntersect(rayOrigin, rayDirection, a, b, c, t)) {
                    if (t < closestMeshDistance) {
                        closestMeshDistance = t;
                        closestMeshIntersectedIndex = meshNo;
                    }
                }
            }
        }
        world.setSelectedMeshIndex(closestMeshIntersectedIndex);
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Camera& camera = world.getViewCamera();
    switch (key) {
        case GLFW_KEY_1:
            if (action == GLFW_PRESS) {
                meshes[meshArrayTop] = Mesh::fromOffFile("../data/unit_cube.off", Vector3f(1.0, 1.0, 0.0), FLAT_SHADE);
                meshes[meshArrayTop].scaleToUnitCube();
                meshes[meshArrayTop].translate(Vector3f(0.0, 0.0, 0.0));
                world.addMesh(meshes[meshArrayTop]);
                meshArrayTop++;
            }
            break;
        case GLFW_KEY_2:
            if (action == GLFW_PRESS) {
                meshes[meshArrayTop] = Mesh::fromOffFile("../data/bunny.off", Vector3f(0.0, 1.0, 0.0), FLAT_SHADE);
                meshes[meshArrayTop].scaleToUnitCube();
                meshes[meshArrayTop].translate(Vector3f(-0.072, -0.1, 0.0));
                world.addMesh(meshes[meshArrayTop]);
                meshArrayTop++;
            }
            break;
        case GLFW_KEY_3:
            if (action == GLFW_PRESS) {
                meshes[meshArrayTop] = Mesh::fromOffFile("../data/bumpy_cube.off", Vector3f(1.0, 0.0, 0.0), FLAT_SHADE);
                meshes[meshArrayTop].scaleToUnitCube();
                meshes[meshArrayTop].translate(Vector3f(0.0, 0.0, 0.0));
                world.addMesh(meshes[meshArrayTop]);
                meshArrayTop++;
            }
            break;

        case GLFW_KEY_UP:
            camera.translateBy(Vector3f(0.0, 0.5, 0.));
            break;
        case GLFW_KEY_DOWN:
            camera.translateBy(Vector3f(0.0, -0.5, 0.));
            break;
        case GLFW_KEY_LEFT:
            camera.translateBy(Vector3f(-0.5, 0.0, 0.));
            break;
        case GLFW_KEY_RIGHT:
            camera.translateBy(Vector3f(0.5, 0.0, 0.0));
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            camera.translateBy(Vector3f(0.0, 0.0, -0.1));
            break;
        case GLFW_KEY_SLASH:
            camera.translateBy(Vector3f(0.0, 0.0, 0.1));
            break;

        case GLFW_KEY_TAB:
            if (action == GLFW_PRESS) {
                if (world.getViewCamera().getProjectionType() == Camera::PROJECTION_ORTHOGRAPHIC) {
                    world.getViewCamera().setProjectionType(Camera::PROJECTION_PERSPECTIVE);
                    world.getViewCamera().setFieldOfViewAngle((3.14/180) * 90);
                } else {
                    world.getViewCamera().setProjectionType(Camera::PROJECTION_ORTHOGRAPHIC);
                    world.getViewCamera().setFieldOfViewAngle((3.14/180) * 140);
                }
            }
            break;
        case GLFW_KEY_LEFT_SHIFT:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                Mesh& mesh = world.getMeshes().at(world.getSelectedMeshIndex()).get();
                if (mesh.getRenderType() == PHONG_SHADE) mesh.setRenderType(WIREFRAME);
                else if (mesh.getRenderType() == WIREFRAME) mesh.setRenderType(FLAT_SHADE);
                else if (mesh.getRenderType() == FLAT_SHADE) mesh.setRenderType(PHONG_SHADE);
            }
            break;
        case  GLFW_KEY_A:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().translate(Vector3f(-0.1, 0, 0.0));
            }
            break;
        case  GLFW_KEY_D:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().translate(Vector3f(0.1, 0, 0.0));
            }
            break;
        case  GLFW_KEY_W:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().translate(Vector3f(0.0, 0.0, -0.1));
            }
            break;
        case  GLFW_KEY_S:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().translate(Vector3f(0.0, 0, 0.1));
            }
            break;
        case  GLFW_KEY_Q:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().translate(Vector3f(0.0, 0.1, 0.0));
            }
            break;
        case  GLFW_KEY_Z:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().translate(Vector3f(0.0, -0.1, 0.0));
            }
            break;
        case  GLFW_KEY_E:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().rotate(Utils::AXIS_Z, -0.1);
            }
            break;
        case  GLFW_KEY_R:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().rotate(Utils::AXIS_Z, 0.1);
            }
            break;
        case  GLFW_KEY_F:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().rotate(Utils::AXIS_X, -0.1);
            }
            break;
        case  GLFW_KEY_G:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().rotate(Utils::AXIS_X, 0.1);
            }
            break;
        case  GLFW_KEY_C:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().rotate(Utils::AXIS_Y, -0.1);
            }
            break;
        case  GLFW_KEY_V:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().rotate(Utils::AXIS_Y, 0.1);
            }
            break;
        case  GLFW_KEY_P:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().scale(1.1);
            }
            break;
        case  GLFW_KEY_L:
            if (action == GLFW_PRESS) {
                if (world.getSelectedMeshIndex() == -1) return;
                world.getMeshes().at(world.getSelectedMeshIndex()).get().scale(1/1.1);
            }
            break;
    }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    world.getViewCamera().setAspectRatio((float)width / (float)height);
}

GLenum getPolygonDrawType(RenderType renderType) {
    switch (renderType) {
        case WIREFRAME:
            return GL_LINE;
        case FLAT_SHADE:
            return GL_FILL;
        case PHONG_SHADE:
            return GL_FILL;
        default:
            throw runtime_error("Unsupported type");
    }
}

int main()
{
    GLFWwindow* window = HelperGL::initAndCreateGLFWWindow();

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    VertexBufferObject VBO_Positions;
    VBO_Positions.init();
    VBO_Positions.update(Eigen::MatrixXf(3, 0));

    VertexBufferObject VBO_VertexNormals;
    VBO_VertexNormals.init();
    VBO_VertexNormals.update(Eigen::MatrixXf(3, 0));

    VertexBufferObject VBO_FaceNormals;
    VBO_FaceNormals.init();
    VBO_FaceNormals.update(Eigen::MatrixXf(3, 0));

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    const GLchar* vertex_shader =
            "#version 150 core\n"
            "in vec3 position;\n"
            "in vec3 vertex_normal;\n"
            "in vec3 face_normal;\n"

            "uniform vec3 color;\n"
            "uniform bool flat_normal;\n"

            "uniform mat4 projection;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"

            "out vec3 Normal;\n"
            "out vec3 FragPos;\n"
            "out vec3 objectColor;\n"

            "void main()"
            "{"
            "    gl_Position = projection * view * model * vec4(position, 1.0);"
            "    FragPos = vec3(model * vec4(position, 1.0f));"
            "    if(flat_normal){"
            "       Normal = mat3(transpose(inverse(model))) * face_normal;"
            "    }else{"
            "       Normal = mat3(transpose(inverse(model))) * vertex_normal;"
            "    }"
            "    objectColor = color;"
            "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"

            "in vec3 Normal;"
            "in vec3 FragPos;"
            "in vec3 objectColor;"

            "out vec4 outColor;"

            "uniform vec3 lightPos;"
            "uniform vec3 viewPos;"
            "uniform bool flat_normal;"

            "void main()"
            "{"
            "    vec3 lightColor = vec3(1.0, 1.0, 1.0);"
            "      float ambientStrength = 0.01f;"
            "      vec3 ambient = ambientStrength * lightColor;"
            "      vec3 norm = normalize(Normal);"
            "      vec3 lightDir = normalize(lightPos - FragPos);"
            "      float diff = max(dot(norm, lightDir), 0.0);"
            "      vec3 diffuse = diff * lightColor;"
            "      if(flat_normal){"
            "         vec3 result = (ambient + diffuse) * objectColor;"
            "         outColor = vec4(result, 1.0);"
            "       }else{"
            "         float specularStrength = 0.5f;"
            "         vec3 viewDir = normalize(viewPos - FragPos);"
            "         vec3 reflectDir = reflect(-lightDir, norm);  "
            "         float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
            "         vec3 specular = specularStrength * spec * lightColor;  "
            "         vec3 result = (ambient + diffuse + specular) * objectColor;"
            "         outColor = vec4(result, 1.0);"
            "       }"
            "}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position", VBO_Positions);
    program.bindVertexAttribArray("vertex_normal", VBO_VertexNormals);
    program.bindVertexAttribArray("face_normal", VBO_FaceNormals);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetWindowSizeCallback(window, window_size_callback);

    int screenWidth, screenHeight;
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    Camera camera(Vector3f(0., 0., 3.), Vector3f(0., 0., 0.),
            Camera::PROJECTION_PERSPECTIVE, (float)screenWidth / (float)screenHeight, -0.5, -100.0, (3.14/180) * 90);
    world.addCamera(camera);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        // Set the uniform value depending on the time difference
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        glUniform3f(program.uniform("triangleColor"), (float)(sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

        // Clear the framebuffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //Set the camera view
        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, world.getViewCamera().getView().data());
        glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, world.getViewCamera().getProjection().data());
        // Draw each mesh

        for (int meshIndex = 0; meshIndex < world.getMeshes().size(); meshIndex++) {
            Mesh mesh = world.getMeshes().at(meshIndex).get();
            VBO_Positions.update(mesh.getTriangleVertices());
            VBO_VertexNormals.update(mesh.getVertexNormals());
            VBO_FaceNormals.update(mesh.getFaceNormals());

            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, mesh.getModel().data());
            glPolygonMode(GL_FRONT_AND_BACK, getPolygonDrawType(mesh.getRenderType()));
            if (world.getSelectedMeshIndex() == meshIndex) {
                glUniform3f(program.uniform("color"), 0.0, 0.0, 1.0);
            } else {
                glUniform3f(program.uniform("color"), mesh.getColor()(0), mesh.getColor()(1), mesh.getColor()(2));
            }
            glUniform1i(program.uniform("flat_normal"), mesh.getRenderType() != PHONG_SHADE);
            glUniform3f(program.uniform("lightPos"), -5.0, 0.0, 10.0);
            glUniform3f(program.uniform("viewPos"), camera.getCameraPosition()(0), camera.getCameraPosition()(1),
                        camera.getCameraPosition()(2));

            glDrawArrays(GL_TRIANGLES, 0, mesh.getFaces().cols() * mesh.getFaces().rows());

            if (mesh.getRenderType() == FLAT_SHADE) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glUniform3f(program.uniform("color"), 0.0, 0.0, 0.0);
                glDrawArrays(GL_TRIANGLES, 0, mesh.getFaces().cols() * mesh.getFaces().rows());
            }
        }

//        MatrixXf identityModel = MatrixXf::Identity(4, 4);
//        glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, identityModel.data());
//        MatrixXf line(3, 2);
//        line << rayOrigin(0), (rayOrigin + (100 * rayDirection))(0),
//                rayOrigin(1), (rayOrigin + (100 * rayDirection))(1),
//                rayOrigin(2), (rayOrigin + (100 * rayDirection))(2);
//        VBO_Positions.update(line);
//        glDrawArrays(GL_LINE_STRIP, 0, 2);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO_Positions.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
