/*
    Shader.h, based on learnopengl.com

    TODO Implement Rule of Five
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_SHADER_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_SHADER_H_INCLUDED

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#define VERTEX_SHADER_EXT ".vert"
#define FRAGMENT_SHADER_EXT ".frag"

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;

    explicit Shader(const GLchar* path)
    {
        Setup(path);
    }

    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        Setup(vertexPath, fragmentPath);
    }

    void Setup(const GLchar* path)
    {
        auto vertexPath = path + std::string(VERTEX_SHADER_EXT);
        auto fragmentPath = path + std::string(FRAGMENT_SHADER_EXT);
        Setup(vertexPath.c_str(), fragmentPath.c_str());
    }

    void Setup(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        auto vShaderCode = vertexCode.c_str();
        auto fShaderCode = fragmentCode.c_str();
        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        // Print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        // Print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Shader Program
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        // Print linking errors if any
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Uses the current shader
    void Use() const
    {
        glUseProgram(program);
    }

    const GLuint& operator()() const
    {
        return program;
    }

private:
    GLuint program;
};

#endif
