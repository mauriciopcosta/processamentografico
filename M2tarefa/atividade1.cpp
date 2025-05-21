/* Hello Triangle - Maurício Pereira da Costa */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>

using namespace std;

// Bibliotecas para OpenGL
#include <glad/glad.h>   // Loader das funções OpenGL
#include <GLFW/glfw3.h>  // Biblioteca para janela, contexto e entrada
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <cmath>

// Callback para sair ao apertar ESC
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
int setupShader();
int setupGeometry();

// Tamanho da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Vertex Shader
const GLchar *vertexShaderSource = "#version 400\n"
    "layout (location = 0) in vec3 position;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * model * vec4(position, 1.0);\n"
    "}\0";

// Fragment Shader
const GLchar *fragmentShaderSource = "#version 400\n"
    "uniform vec4 inputColor;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "color = inputColor;\n"
    "}\n\0";

int main()
{
    // Inicializa o GLFW
    glfwInit();

    // Cria a janela
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- Maurício", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Registra a callback de teclado
    glfwSetKeyCallback(window, key_callback);

    // Carrega funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Exibe informações da GPU e versão da OpenGL
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version supported " << glGetString(GL_VERSION) << endl;

    // Define o tamanho da viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compila shaders e cria programa
    GLuint shaderID = setupShader();
    glUseProgram(shaderID);

    // Localiza a variável uniform "inputColor" no fragment shader
    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

    // Define uma cor azul
    glUniform4f(colorLoc, 0.0f, 0.0f, 1.0, 1.0f);

    // Matriz de projeção ortográfica
    mat4 projection = ortho(-1.0, 1.0, -1.0, 1.0);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Matriz de modelo como identidade (sem transformação)
    mat4 model = mat4(1.0);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

    // Cria e armazena VAOs de triângulos
    vector<GLuint> VAOs;
    VAOs.push_back(createTriangle(-0.65, 0.33, -0.27, 0.53, -0.61, 0.79));

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Limpa a tela com fundo preto
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        // Desenha os triângulos armazenados
        for (GLuint vao : VAOs)
        {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int setupGeometry()
{
    GLfloat vertices[] = {
        -0.5, -0.5, 0.0,
         0.5, -0.5, 0.0,
         0.0,  0.5, 0.0
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{
    GLfloat vertices[] = {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x2, y2, 0.0f
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}