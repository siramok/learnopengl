#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // fields
    unsigned int program_id;

    // methods
    Shader(const char *vertex_shader_path, const char *fragment_shader_path);
    ~Shader();
    void use();
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_mat4(const std::string &name, glm::mat4 &value) const;

private:
    const int log_size = 1024;
    void check_compile_errors(unsigned int shader, std::string type);
};

#endif