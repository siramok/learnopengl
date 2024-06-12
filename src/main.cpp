#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
void process_input(GLFWwindow *window);
unsigned int load_texture(const char *path);

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

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    glm::vec3 cube_positions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glm::vec3 point_light_positions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f),
    };

    unsigned int cube_VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cube_VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texel attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int light_VAO;
    glGenVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuse_map = load_texture(container2_path);
    unsigned int specular_map = load_texture(container2_specular_path);

    main_shader.use();
    main_shader.set_int("material.diffuse", 0);
    main_shader.set_int("material.specular", 1);

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
        // camera
        main_shader.set_vec3("camera_position", camera.position);
        main_shader.set_float("material.shininess", 1.0f);
        // directional light
        main_shader.set_vec3("directional_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        main_shader.set_vec3("directional_light.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        main_shader.set_vec3("directional_light.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        main_shader.set_vec3("directional_light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        // point light 1
        main_shader.set_vec3("point_lights[0].position", point_light_positions[0]);
        main_shader.set_vec3("point_lights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        main_shader.set_vec3("point_lights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        main_shader.set_vec3("point_lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        main_shader.set_float("point_lights[0].constant", 1.0f);
        main_shader.set_float("point_lights[0].linear", 0.09f);
        main_shader.set_float("point_lights[0].quadratic", 0.032f);
        // point light 2
        main_shader.set_vec3("point_lights[1].position", point_light_positions[1]);
        main_shader.set_vec3("point_lights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        main_shader.set_vec3("point_lights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        main_shader.set_vec3("point_lights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        main_shader.set_float("point_lights[1].constant", 1.0f);
        main_shader.set_float("point_lights[1].linear", 0.09f);
        main_shader.set_float("point_lights[1].quadratic", 0.032f);
        // point light 3
        main_shader.set_vec3("point_lights[2].position", point_light_positions[2]);
        main_shader.set_vec3("point_lights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        main_shader.set_vec3("point_lights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        main_shader.set_vec3("point_lights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        main_shader.set_float("point_lights[2].constant", 1.0f);
        main_shader.set_float("point_lights[2].linear", 0.09f);
        main_shader.set_float("point_lights[2].quadratic", 0.032f);
        // point light 4
        main_shader.set_vec3("point_lights[3].position", point_light_positions[3]);
        main_shader.set_vec3("point_lights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        main_shader.set_vec3("point_lights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        main_shader.set_vec3("point_lights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        main_shader.set_float("point_lights[3].constant", 1.0f);
        main_shader.set_float("point_lights[3].linear", 0.09f);
        main_shader.set_float("point_lights[3].quadratic", 0.032f);
        // spot light
        main_shader.set_vec3("spot_light.position", camera.position);
        main_shader.set_vec3("spot_light.direction", camera.front);
        main_shader.set_vec3("spot_light.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        main_shader.set_vec3("spot_light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        main_shader.set_vec3("spot_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        main_shader.set_float("spot_light.constant", 1.0f);
        main_shader.set_float("spot_light.linear", 0.09f);
        main_shader.set_float("spot_light.quadratic", 0.032f);
        main_shader.set_float("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
        main_shader.set_float("spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();
        main_shader.set_mat4("projection", projection);
        main_shader.set_mat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        main_shader.set_mat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        glBindVertexArray(cube_VAO);
        for (int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            main_shader.set_mat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        light_shader.use();
        light_shader.set_mat4("projection", projection);
        light_shader.set_mat4("view", view);

        glBindVertexArray(light_VAO);
        for (int i = 0; i < N_POINT_LIGHTS; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, point_light_positions[i]);
            model = glm::scale(model, glm::vec3(0.3f));
            light_shader.set_mat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteVertexArrays(1, &light_VAO);
    glDeleteBuffers(1, &VBO);

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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int load_texture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}