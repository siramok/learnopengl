#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
void process_input(GLFWwindow *window);

// settings
const unsigned int WINDOW_WIDTH = 1600;
const unsigned int WINDOW_HEIGHT = 1200;
const unsigned int N_POINT_LIGHTS = 4;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;
bool first_call = true;

// timing
float delta_time = 0.0f; // Time between current frame and last frame
float last_frame = 0.0f; // Time of last frame

// lighting
glm::vec3 light_position(1.2f, 1.0f, 2.0f);

// paths
const char *container_path = "assets/textures/container.jpg";
const char *container2_path = "assets/textures/container2.png";
const char *container2_specular_path = "assets/textures/container2_specular.png";
const char *awesomeface_path = "assets/textures/awesomeface.png";
const char *vertex_shader_path = "assets/shaders/main.vert";
const char *fragment_shader_path = "assets/shaders/main.frag";
const char *light_vertex_shader_path = "assets/shaders/light.vert";
const char *light_fragment_shader_path = "assets/shaders/light.frag";
const char *model_path = "assets/models/backpack/backpack.obj";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader main_shader = Shader(vertex_shader_path, fragment_shader_path);
    Shader light_shader = Shader(light_vertex_shader_path, light_fragment_shader_path);

    // load model
    Model obj_model(model_path);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // time
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        main_shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();
        main_shader.set_mat4("projection", projection);
        main_shader.set_mat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        main_shader.set_mat4("model", model);
        obj_model.draw(main_shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &cube_VAO);
    // glDeleteVertexArrays(1, &light_VAO);
    // glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.process_keyboard(FORWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.process_keyboard(BACKWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.process_keyboard(LEFT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.process_keyboard(RIGHT, delta_time);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double x_pos_in, double y_pos_in)
{
    float x_pos = static_cast<float>(x_pos_in);
    float y_pos = static_cast<float>(y_pos_in);
    // std::cout << "last_frame = " << last_frame << ", last_x = " << last_x << ", last_y = " << last_y << ", x_pos = " << x_pos << ", y_pos = " << y_pos << std::endl;

    // workaround an annoying bug where the first frame produces a big offset delta
    if (first_call)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_call = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos;
    last_x = x_pos;
    last_y = y_pos;

    camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    camera.process_mouse_scroll(static_cast<float>(y_offset));
}
