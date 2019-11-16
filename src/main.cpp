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

// VertexBufferObject wrapper

World world;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Mesh& mesh = world.getMeshes().at(0);
    Camera& camera = world.getViewCamera();
    switch (key)
    {
        case  GLFW_KEY_1:
            if (action == GLFW_PRESS) mesh.scale(2);
            break;
        case GLFW_KEY_2:
            mesh.rotate(Utils::AXIS_Z, 0.1);
            break;
        case  GLFW_KEY_3:
            mesh.rotate(Utils::AXIS_X, 0.1);
            break;
        case  GLFW_KEY_Q:
            if (action == GLFW_PRESS) mesh.translate(Vector3f(0.2, 0, 0.0));
            break;
        case  GLFW_KEY_W:
            if (action == GLFW_PRESS) mesh.scale(0.8);
            break;

        case  GLFW_KEY_P:
            camera.translateBy(Vector3f(0.0, 0.0, -0.5));
            break;
        case  GLFW_KEY_L:
            camera.translateBy(Vector3f(0.0, 0.0, 0.5));
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
        default:
            break;
    }
//    std::cout<<world.getViewCamera().getView()<<std::endl;
}

GLenum getPolygonDrawType(RenderType renderType) {
    switch (renderType) {
        case WIREFRAME:
            return GL_LINE;
        case FLAT_SHADE:
            return GL_FILL;
        case PHONG_SHADE:
            throw runtime_error("Unsupported type");
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
            // Ambient
            "      float ambientStrength = 0.01f;"
            "      vec3 ambient = ambientStrength * lightColor;"

            // Diffuse
            "      vec3 norm = normalize(Normal);"
            "      vec3 lightDir = normalize(lightPos - FragPos);"
            "      float diff = max(dot(norm, lightDir), 0.0);"
            "      vec3 diffuse = diff * lightColor;"
            "      if(flat_normal){"
            "         vec3 result = (ambient + diffuse) * objectColor;"
            "         outColor = vec4(result, 1.0);"
            "       }else{"
            // Specular
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


    Mesh bumpyMesh = Mesh::fromOffFile("../data/bumpy_cube.off", Vector3f(1.0, 0.0, 0.0), WIREFRAME);
    world.addMesh(bumpyMesh);
    bumpyMesh.scale(0.05);
    bumpyMesh.rotate(Utils::AXIS_Z, 0.4);
    bumpyMesh.translate(Eigen::Vector3f(0.0, 0.0, -2));

//    Mesh bunnyMesh = Mesh::fromOffFile("../data/bunny.off", Vector3f(0.0, 1.0, 0.0), FLAT_SHADE);
//    bunnyMesh.translate(Eigen::Vector3f(-0.3, -0.1, -5));
//    bunnyMesh.scale(4);
//    world.addMesh(bunnyMesh);

//    Mesh unitCube = Mesh::fromOffFile("../data/unit_cube.off", Vector3f(1., 1., 0.), FLAT_SHADE);
//    world.addMesh(unitCube);
//    unitCube.scale(0.1);

    int screenWidth, screenHeight;
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    Camera camera(screenWidth, screenHeight);
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

//        glClear(GL_COLOR_BUFFER_BIT);

        //Set the camera view
        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, world.getViewCamera().getView().data());
        glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, world.getViewCamera().getProjection().data());
        // Draw each mesh
        for (Mesh mesh: world.getMeshes()) {
            VBO_Positions.update(mesh.getTriangleVertices());
            VBO_VertexNormals.update(mesh.getVertexNormals());
            VBO_FaceNormals.update(mesh.getFaceNormals());

            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, mesh.getModel().data());
            glPolygonMode(GL_FRONT_AND_BACK, getPolygonDrawType(mesh.getRenderType()));
            glUniform3f(program.uniform("color"), mesh.getColor()(0), mesh.getColor()(1), mesh.getColor()(2));
            glUniform1i(program.uniform("flat_normal"), false);
            glUniform3f(program.uniform("lightPos"), -1.0, 0.0, -1.0);
            glUniform3f(program.uniform("viewPos"), camera.getEye()(0), camera.getEye()(1), camera.getEye()(2));

            glDrawArrays(GL_TRIANGLES, 0, mesh.getFaces().cols() * mesh.getFaces().rows());
        }

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
