#include "shader.h"
#include <filesystem>

Shader::Shader(const char *vertex_shader_path, const char *fragment_shader_path)
{
    std::string vertex_shader_src;
    std::string fragment_shader_src;
    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;
    vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // try reading the shader files
    try
    {
        std::filesystem::path cwd = std::filesystem::current_path();
        std::cout << "Current working directory: " << cwd << std::endl;

        vertex_shader_file.open(vertex_shader_path);
        fragment_shader_file.open(fragment_shader_path);

        std::stringstream vertex_shader_stream;
        std::stringstream fragment_shader_stream;

        vertex_shader_stream << vertex_shader_file.rdbuf();
        fragment_shader_stream << fragment_shader_file.rdbuf();

        vertex_shader_file.close();
        fragment_shader_file.close();

        vertex_shader_src = vertex_shader_stream.str();
        fragment_shader_src = fragment_shader_stream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char *vertex_shader_code = vertex_shader_src.c_str();
    const char *fragment_shader_code = fragment_shader_src.c_str();

    unsigned int vertex_shader;
    unsigned int fragment_shader;

    // compile vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    check_compile_errors(vertex_shader, "VERTEX");

    // compile fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    check_compile_errors(fragment_shader, "FRAGMENT");

    // attach and link the shaders to a program
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
    check_compile_errors(program_id, "PROGRAM");

    // cleanup since the shaders are already linked at this point
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader()
{
    glDeleteProgram(program_id);
}

void Shader::use()
{
    glUseProgram(program_id);
}

void Shader::set_bool(const std::string &name, bool value) const
{
    int location = glGetUniformLocation(program_id, name.c_str());
    glUniform1i(location, (int)value);
}

void Shader::set_int(const std::string &name, int value) const
{
    int location = glGetUniformLocation(program_id, name.c_str());
    glUniform1i(location, value);
}

void Shader::set_float(const std::string &name, float value) const
{
    int location = glGetUniformLocation(program_id, name.c_str());
    glUniform1f(location, value);
}

void Shader::set_mat4(const std::string &name, glm::mat4 &value) const
{
    unsigned int location = glGetUniformLocation(program_id, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string &name, glm::vec3 value) const
{
    unsigned int location = glGetUniformLocation(program_id, name.c_str());
    glUniform3f(location, value[0], value[1], value[2]);
}

void Shader::check_compile_errors(unsigned int shader, std::string type)
{
    int success;
    char info_log[log_size];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, log_size, NULL, info_log);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, log_size, NULL, info_log);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
