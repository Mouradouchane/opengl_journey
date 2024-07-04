
#include <iostream>
#include <string>

#define GLEW_STATIC
#include "glew/glew.h"
#include "glfw/glfw3.h"

#define INIT_SHADERS_FAILED 0
#define INIT_BUFFERS_FAILED 0
#define INIT_SHADERS_SUCCESSED 1
#define INIT_BUFFERS_SUCCESSED 1

void inputs();
void physics();
void render();

short init_buffers();
short init_shaders();

GLFWwindow* window = nullptr;

GLuint shader_program_id  = NULL;
GLuint vertex_shader_id   = NULL;
GLuint fragment_shader_id = NULL;

GLuint vbo = NULL;
GLuint vao = NULL;
GLuint ebo = NULL;

std::string vertex_shader_code = R"(
	#version 330 core

	layout(location=0) in vec3 vertex;
	out vec4 pvertex;

	void main(){
		pvertex = vec3(vertex , 1.0f);
	}
)";

std::string fragment_shader_code = R"(
	#version 330 core

	in  vec3 vertex;
	out vec4 color;

	void main(){
		color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
)";

float vertices[] = {
	 0.5f,  0.5f, 0.0f, // top right
	 0.5f, -0.5f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f  // top left
};

GLuint indices[] = {
	0, 1, 3,
	1, 2, 3
};


int main(int argc, char** argv) {

	glfwInitHint(GLFW_DOUBLEBUFFER , GL_TRUE);
	glfwInitHint(GLFW_OPENGL_FORWARD_COMPAT , GL_TRUE);

	// init glfw
	if ( !glfwInit() ) {
		std::cout << "glfw init error \n";
		return -1;
	}
	
	// hints for glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE);
	// for window fixed size
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	// create window
	window = glfwCreateWindow(800, 600, "hello triangle", NULL, NULL);

	if (window == nullptr) {
		std::cout << "glfw create window error \n";
		return -1;
	}

	// get opengl functions
	glfwMakeContextCurrent(window);

	// note : glew init after context creation
	if ( glewInit() != GLEW_OK ) {
		std::cout << "glew init error \n";
		return -1;
	}
	
	if (init_buffers() == INIT_BUFFERS_FAILED) {
		std::cout << "failed to init buffers \n";
		return -1;
	}

	if (init_shaders() == INIT_SHADERS_FAILED) {
		std::cout << "failed to init shaders \n";
		return -1;
	}

	// main loop
	while ( !glfwWindowShouldClose(window) ) {

		// input handle
		inputs();

		// physics 
		physics();

		// rendering
		render();

		glfwPollEvents();
	}

	// free resource
	glfwDestroyWindow(window);

	return 0;
}

inline void check_error(GLenum& error) {

	error = glGetError();

	if (error != GL_NO_ERROR) {
		throw error;
	}
	else error = GL_NO_ERROR;

}

short init_buffers() {

	try {
		GLenum error = GL_NO_ERROR;
		
		// VAO
		glGenVertexArrays(1, &vao);
		check_error(error);
		glBindVertexArray(vao);
		check_error(error);

		// VBO
		glGenBuffers(1, &vbo);
		check_error(error);
		// copy data to vbo
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);

		check_error(error);

		// descirbe vbo data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3 , nullptr);
		glEnableVertexAttribArray(0);

		check_error(error);

		return INIT_BUFFERS_SUCCESSED;
	}
	catch (GLenum error) {

		std::cout << "init buffers failed : " << error << "\n";
		return INIT_BUFFERS_FAILED;
	}

}

short init_shaders() {

	try {

		// for catching errors
		GLenum error = GL_NO_ERROR;

		// create shader objects
		vertex_shader_id   = glCreateShader(GL_VERTEX_SHADER);
		fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

		check_error(error);

		// copy shader's "source-code" to "shader object's"
		const char* code_ptr = vertex_shader_code.c_str();
		GLint       length   = vertex_shader_code.size();

		glShaderSource(vertex_shader_id, 1, &code_ptr, &length);
		check_error(error);

		code_ptr = fragment_shader_code.c_str();
		length   = fragment_shader_code.size();

		glShaderSource(fragment_shader_id, 1, &code_ptr, &length);
		check_error(error);
		
		// create program
		shader_program_id = glCreateProgram();
		check_error(error);

		// compile shaders
		glCompileShader(vertex_shader_id);
		check_error(error);

		glCompileShader(fragment_shader_id);
		check_error(error);
	
		// attach shader's
		glAttachShader(shader_program_id, vertex_shader_id);
		check_error(error);
		
		glAttachShader(shader_program_id, fragment_shader_id);
		check_error(error);

		// link shader's into program object
		glLinkProgram(shader_program_id);
		check_error(error);

		return INIT_SHADERS_SUCCESSED;

	}
	catch (GLenum error_code) {
		
		std::cout << "init shaders error : " << error_code << "\n";
		return INIT_SHADERS_FAILED;
	}

}

void physics() {
}
void inputs() {
}

void render() {

	// clear frame buffer
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader_program_id);

	// draw calls
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 3 , GL_UNSIGNED_INT , 0);

	glBindVertexArray(0);

	// swap buffer
	glfwSwapBuffers(window);

}