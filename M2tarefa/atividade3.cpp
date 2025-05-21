/* Hello Triangle - Maurício Pereira da Costa */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Struct para armazenar posição e cor do triângulo
struct Triangle {
    float x, y;    // posição
    float r, g, b; // cor RGB
};

vector<Triangle> triangles;
GLuint VAO;
GLuint shaderID;
mat4 projection;

const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar *vertexShaderSource = "#version 400\n"
    "layout (location = 0) in vec3 position;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * model * vec4(position, 1.0);\n"
    "}\0";

const GLchar *fragmentShaderSource = "#version 400\n"
    "uniform vec4 inputColor;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "color = inputColor;\n"
    "}\n\0";

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
GLuint setupShader();
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio 3 - Triangulos com clique", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    shaderID = setupShader();
    glUseProgram(shaderID);

    projection = ortho(0.0f, float(WIDTH), float(HEIGHT), 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Corrigido: Cria um triângulo maior (100x100 unidades)
    VAO = createTriangle(-50.0f, -50.0f, 50.0f, -50.0f, 0.0f, 50.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        for (const Triangle& t : triangles) {
            mat4 model = mat4(1.0f);
            model = translate(model, vec3(t.x, t.y, 0.0f));
            // Corrigido: opcional se triângulo for pequeno, mas aqui não precisa pois já aumentamos a geometria.
            // Se quiser garantir: model = scale(model, vec3(50.0f, 50.0f, 1.0f));

            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
            glUniform4f(glGetUniformLocation(shaderID, "inputColor"), t.r, t.g, t.b, 1.0f);
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        Triangle tri;
        tri.x = static_cast<float>(xpos);
        tri.y = static_cast<float>(ypos);
        tri.r = static_cast<float>(rand() % 100) / 100.0f;
        tri.g = static_cast<float>(rand() % 100) / 100.0f;
        tri.b = static_cast<float>(rand() % 100) / 100.0f;

        triangles.push_back(tri);
    }
}

GLuint setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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