#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#if defined(_DEBUG)
#define ASSERT(x) if (!x) { __debugbreak(); }
#else
#define ASSERT(x)
#endif

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError()
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error]" << error << std::endl;
        return false;
    }
    return true;
}


struct ShaderProgramSource
{
    std::string vertex;
    std::string fragment;
};

static ShaderProgramSource ParseShader(const std::string &filepath)
{
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];

    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        } else
            ss[(int) type] << line << "\n";
    }

    return {
            ss[0].str(), ss[1].str()
    };
}


static unsigned int CompileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "[OpenGL Error] Filed to compile "
                  << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                  << " Shader, the OpenGL Error message ↓"
                  << std::endl;

        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string &vertex, const std::string &fragment)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main()
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(480, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) return -2;

    std::cout << "OpenGL Version >> " << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f,
    };

    unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    auto shaderCode = ParseShader("res/shaders/basic.shader");
    auto shaderId = CreateShader(shaderCode.vertex, shaderCode.fragment);
    glUseProgram(shaderId);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

//        glDrawArrays(GL_TRIANGLES, 0, 6);
        GLClearError();
        glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr);
        ASSERT(GLCheckError())

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shaderId);

    glfwTerminate();
    return 0;
}