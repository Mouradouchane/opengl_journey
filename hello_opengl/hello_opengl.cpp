
#include <iostream>
#include <string>

#define GLEW_STATIC
#include "glew/glew.h"
#include "glfw/glfw3.h"

// global variables

std::string vertex_shader_source_code = R"(
    #version 330 core
    layout(location = 0) in vec3 vertex;

    void main() {
        gl_Position = vec4(vertex.x, vertex.y, vertex.z, 1);
    }
)";

std::string pixle_shader_source_code = R"(
    #version 330 core

    out vec4 pixle;

    void main(){
         pixle = vec4(1.0f, 0.5f, 0.5f, 1.0f);
    }
)";

GLFWwindow* window = nullptr;
GLuint vao = 0;
GLuint vbo = 0;
GLuint ebo = 0; // element buffer object

GLuint vertex_shader = NULL;
GLuint pixle_shader  = NULL;

GLuint shader_program = NULL;

size_t v_size = 12;
float vertices[] = {
     0.5f,  0.5f, 0.0f, // top right
     0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f  // top left
};

size_t i_size = 6;
GLuint indices[] = {
    0, 1, 3, //  first triangle
    1, 2, 3  // second triangle
};

void init_gpu_buffers();
void process_input(GLFWwindow* window);
void render();

GLuint create_shader(GLenum shader_type = NULL , const char * source_code = nullptr);

GLuint link_shaders(GLuint v_shader, GLuint p_shader);

int main(){
     
    std::cout << "RUNNING...\n";
    
    /* Initialize the library */
    if (!glfwInit()) return -1;

    // few configs for window 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // window fixed size
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello OpenGL", NULL, NULL);

    if (window) {
        std::cout << "GLFW Success : " << glfwGetVersionString() << "\n";
    }
    else {
        char* error_message = nullptr;

        glfwGetError( (const char**) (&error_message) );
        std::cout << "GLFW Error : " << error_message << "\n";
        glfwTerminate();

        return -1;
    }

    // opengl context
    glfwMakeContextCurrent(window);

    // init glew 
    GLenum glew_init_result = glewInit();

    if (GLEW_OK != glew_init_result) {
        std::cout << "GLEW Error : " << glewGetErrorString(glew_init_result) << "\n";
    }
    else {
        std::cout << "GLEW Success : " << glewGetString(GLEW_VERSION) << "\n";
    }

    // setup view port "screen space"
    glViewport(0, 0, 800, 600);
    
    init_gpu_buffers();
   
    // create/compile shaders
    vertex_shader = create_shader(GL_VERTEX_SHADER  , vertex_shader_source_code.c_str());
    pixle_shader  = create_shader(GL_FRAGMENT_SHADER , pixle_shader_source_code.c_str());

    if (vertex_shader == NULL || pixle_shader == NULL) {
        return -1;
    }
   
    shader_program = link_shaders(vertex_shader, pixle_shader);
    
    // delete shaders object's after linking
    glDeleteShader(vertex_shader);
    glDeleteShader(pixle_shader);
    
    if (shader_program == NULL) {
        return -1;
    }

    // Loop until the user closes the window 
    while ( !glfwWindowShouldClose(window) ) {

        process_input(window);
   
        // rendering
        render();

        // Poll and process "events"
        glfwPollEvents();

    }

    glfwTerminate();

    return 0;
}

void init_gpu_buffers() {

    try {

        // VAO
        glGenVertexArrays(1, &vao); // create 
        glBindVertexArray(vao); // select 

        // VBO
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // copy data to VBO 
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);

        // EBO
        glGenBuffers(1, &ebo);  // create 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // select
        // copy indices to EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(GLuint) , indices, GL_STATIC_DRAW);

        // describe "shape of the data" in the VBO to the gpu
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr); 
        glEnableVertexAttribArray(0);

    }
    catch (std::exception&) {

        std::cout << "ERROR : " << glGetError() << "\n";
    }

}

void render() {

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // select shader
    glUseProgram(shader_program);

    glBindVertexArray(vao);

    // rendering 
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // to unbind selected vao
    glBindVertexArray(0);

    // swap buffers
    glfwSwapBuffers(window);

}

void process_input(GLFWwindow* window) {

    switch ( glfwGetKey(window, GLFW_KEY_ESCAPE) ){

        case GLFW_PRESS: {
            glfwSetWindowShouldClose(window, true);
        } break;

    }

}

GLuint create_shader(GLenum shader_type, const char* source_code) {

    try {

        // create shader object
        GLuint shader = glCreateShader(shader_type);

        if (shader == NULL) {
            throw std::exception("failed to create shader !");
        }

        // set shader "source code"
        glShaderSource(shader, 1, &source_code, NULL);

        // compile shader
        glCompileShader(shader);

        // check compiliation status
        GLint compile_status = NULL;
        char  compile_message[256] = {};

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

        if (compile_status == NULL) {
            glGetShaderInfoLog(shader, 256, NULL, compile_message);
            throw std::exception(compile_message);
        }
        else {
            return shader;
        }
    }
    catch (std::exception error) {

        std::cout << error.what() << "\n";
        return NULL;
    }

}

GLuint link_shaders(GLuint v_shader, GLuint p_shader) {

    try {
        // create program
        GLuint program = glCreateProgram();

        // attach shaders
        glAttachShader(program, v_shader);
        glAttachShader(program, p_shader);

        // link program
        glLinkProgram(program);

        // check linking status
        GLint linking_result = NULL;
        glGetProgramiv(program, GL_LINK_STATUS, &linking_result);

        if (linking_result == GL_FALSE) {
            GLsizei msg_len = 0;
            GLchar* message = nullptr;
            glGetProgramInfoLog(program, 256, &msg_len, message);

            throw std::exception(message);
        }

        return program;
    }
    catch (std::exception error) {

        std::cout << "linking shaders error : " << error.what() << "\n";
        return NULL;
    }

}

