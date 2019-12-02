#include <iostream>
#include <vector>
#include <map>
#define GLEW_STATIC
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
//#include <GL/glut.h>
#include <GL/glext.h>
#include </usr/include/GL/freeglut.h>

#include </usr/include/GLFW/glfw3.h>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SOIL/SOIL.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

struct Shader
{
    // program ID
    GLuint Program;

    // Constructor for linking and compilation shader program
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        ////////Получаем исходный код шейдера из filePath
        string vertexCode;
        string fragmentCode;
        ifstream vShaderFile;
        ifstream fShaderFile;

            // Открываем файлы
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        stringstream vShaderStream, fShaderStream;
            // Считываем данные в потоки
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
            // Закрываем файлы
        vShaderFile.close();
        fShaderFile.close();
            // Преобразовываем потоки в массив GLchar
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

        /////// shader linking
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];

        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        // check for errors
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        };

        // fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        // check for errors
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        };

        // shader program

        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);

        // check for errors
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // delete vertex & fragment
        glDeleteShader(vertex);
        glDeleteShader(fragment);


    }

    // program use
    void Use()
    {
        glUseProgram(this->Program);
    }

    void setVec3(const GLchar* str, vec3 data) {
        GLint Loc = glGetUniformLocation(this->Program, str);
        glUniform3f(Loc, data.x, data.y, data.z);

    }
    void setVec3(const GLchar* str, GLfloat x, GLfloat y, GLfloat z) {
        GLint Loc = glGetUniformLocation(this->Program, str);
        glUniform3f(Loc, x, y, z);

    }

    void setFloat(const GLchar* str, float data) {
        GLint Loc = glGetUniformLocation(this->Program, str);
        glUniform1f(Loc, data);

    }

    void setInt(const GLchar* str, GLint data) {
        GLint Loc = glGetUniformLocation(this->Program, str);
        glUniform1i(Loc, data);

    }

};

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{

    // load texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = SOIL_load_image(path, &width, &height, &nrComponents, 0);
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

        SOIL_free_image_data(data);
    }
    else
    {
        cout << "Texture failed to load at path: " << path << endl;
        SOIL_free_image_data(data);
    }

    return textureID;
}




// window size
const GLuint WIDTH = 800, HEIGHT = 600;

GLfloat deltaTime = 0.0f;	// Время, прошедшее между последним и текущим кадром
GLfloat lastFrame = 0.0f;  	// Время вывода последнего кадра

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);  // lamp coord in the world

bool keys[1024];
// camera
GLfloat Yaw   = -90.0f;
GLfloat Pitch = 0.0f;
GLfloat lastX =  WIDTH  / 2.0;
GLfloat lastY =  HEIGHT / 2.0;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{



    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if(Pitch > 89.0f)
        Pitch = 89.0f;
    if(-89.0f > Pitch)
        Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front);
}





void do_movement()
{
    // Camera controls

    GLfloat cameraSpeed = 5.0f * deltaTime;
    if(keys[GLFW_KEY_W])
        cameraPos += cameraSpeed * cameraFront;
    if(keys[GLFW_KEY_S])
        cameraPos -= cameraSpeed * cameraFront;
    if(keys[GLFW_KEY_A])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(keys[GLFW_KEY_D])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

glm::mat4 initMATRIX()
{
    glm :: mat4 tmp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            (i != j)? tmp[i][i] = 0.0: tmp[i][i] = 1.0;
        }
    }
    return tmp;
}



int main() {

    //Инициализация GLFW
    glfwInit();


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Выключение возможности изменения размера окна
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);



    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello world!", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    glewExperimental=GL_TRUE;
    glewInit();


    // viewport dimension
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Shader shad("/home/zarbog228/mashgraf/proj5sem/vertex.shader",
                "/home/zarbog228/mashgraf/proj5sem/fragment.shader");
    Shader lightShad("/home/zarbog228/mashgraf/proj5sem/lightVert.shader",
                     "/home/zarbog228/mashgraf/proj5sem/lightFrag.shader");
    Shader ObvodkaShad("/home/zarbog228/mashgraf/proj5sem/vertex.shader",
                     "/home/zarbog228/mashgraf/proj5sem/shaderSingleColor");
    Shader billboardShad("/home/zarbog228/mashgraf/proj5sem/bilboardVert.shader",
                       "/home/zarbog228/mashgraf/proj5sem/bilboardFrag.shader");


/*
    GLfloat vertices[] = {         // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
*/


    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3(-1.3f,  1.0f, -1.5f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            //light
            glm::vec3( 2.0f,  5.0f, -15.0f)
    };

    vector<glm::vec3> billboardPos;
    billboardPos.push_back(glm::vec3(-1.5f,  0.0f, 5.0f - 0.48f));
    billboardPos.push_back(glm::vec3( 1.5f,  0.0f, 5.0f + 0.51f));
    billboardPos.push_back(glm::vec3( 0.0f,  0.0f, 5.0f + 0.7f));
    billboardPos.push_back(glm::vec3(-0.3f,  0.0f, 5.0f -2.3f));
    billboardPos.push_back(glm::vec3( 0.5f,  0.0f, 5.0f + -0.6f));



    GLfloat verticesQUADR[] = {
            // Позиции          // Цвета             // Текстурные координаты
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Верхний правый
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Нижний правый
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Нижний левый
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Верхний левый
    };

    GLuint indices[] = {  // Помните, что мы начинаем с 0!
            0, 1, 3,   // Первый треугольник
            1, 2, 3    // Второй треугольник
    };



    //// drawing :


    // set scene VAO(and VBO) first
    GLuint VBO, sceneVAO;
    glGenVertexArrays(1, &sceneVAO);
    glGenBuffers(1, &VBO);


    // Bind sceneVAO
    glBindVertexArray(sceneVAO);

        // Set VBO inside sceneVAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);  // turn on attrib array from 0 position

        //normal attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        // TexCoord attrib
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

    // Unbind sceneVAO
    glBindVertexArray(0);
    // to draw a scene - bind sceneVAO, unbind afterwords





    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
        // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Set the vertex attributes (only position data for the lamp))
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    // set billboard's VAO (and index buf object)
    GLuint billboardVAO, billboardVBO, IBO;
    glGenBuffers(1, &IBO);
    glGenBuffers(1, &billboardVBO);
    glGenVertexArrays(1, &billboardVAO);

    glBindVertexArray(billboardVAO);

        glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesQUADR), verticesQUADR, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        //texCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    //


    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("/home/zarbog228/mashgraf/proj5sem/borat.jpg");
    unsigned int specularMap = loadTexture("/home/zarbog228/mashgraf/proj5sem/borat.jpg");
    //unsigned int specularMap = loadTexture("/home/zarbog228/mashgraf/proj5sem/container2_specular.png");
    unsigned int billboardTex = loadTexture("/home/zarbog228/mashgraf/proj5sem/blending_transparent_window.png");



/*
    // load texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int tWidth, tHeight;
    unsigned char* image = SOIL_load_image("/home/zarbog228/mashgraf/proj5sem/Container.jpg", &tWidth, &tHeight, nullptr, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
*/

    shad.Use();
    shad.setInt("material.diffuse", 0);
    shad.setInt("material.specular", 1);

    while(!glfwWindowShouldClose(window))
    {

        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glfwPollEvents();
        do_movement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glStencilMask(0x00); // checking light is not changing stencil buf


//// using texture
        /*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shad.Program, "ourTexture"), 0);
        //glUseProgram(shaderProgram);
       */

        glm::mat4 view;
        // Обратите внимание, что мы смещаем сцену в направлении обратном тому, в котором мы хотим переместиться
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, 3.0f));
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


        GLfloat screenWidth = 800, screenHeight = 600;
        glm::mat4 projection;
        projection = glm::perspective(radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);



        // for obvodka
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        //
        // drawing lamp

        lightShad.Use();

        GLint modelLoc = glGetUniformLocation(lightShad.Program, "model");
        GLint viewLoc  = glGetUniformLocation(lightShad.Program, "view");
        GLint projLoc  = glGetUniformLocation(lightShad.Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        mat4 model=glm::mat4();
        model = glm::translate(model, lightPos);
        model = scale(model, vec3(0.2f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        // for obvodka
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        //

        shad.Use();


        shad.setVec3("light.position", lightPos);
        shad.setVec3("viewPos", cameraPos);
/*
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
        shad.setVec3("light.ambient",  ambientColor);
        shad.setVec3("light.diffuse",  diffuseColor); // darken the light a bit to fit the scene
        shad.setVec3("light.specular", vec3(1.0f, 1.0f, 1.0f));
*/
        // light properties
        shad.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
        shad.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
        shad.setVec3("light.specular", vec3(1.0f, 1.0f, 1.0f));

        // material properties
        //shad.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        //shad.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        //shad.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        shad.setFloat("material.shininess", 64.0f);




        modelLoc = glGetUniformLocation(shad.Program, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        viewLoc = glGetUniformLocation(shad.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        projLoc = glGetUniformLocation(shad.Program, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        GLint lightPosLoc = glGetUniformLocation(shad.Program, "lightPos");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

/*
        glm::mat4 trans;
        //trans = initMATRIX();
        trans = glm::rotate(trans, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        GLuint transformLoc = glGetUniformLocation(shad.Program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
*/
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(sceneVAO);

        for(GLuint i = 0; i < 10; i++)
        {

            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            //GLfloat angle = radians((GLfloat)glfwGetTime() *20.0f * (i + 1));
            //GLfloat angle = radians(20.0f * (i + 1));
            //model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);




        // for obvodka
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        ObvodkaShad.Use();


        modelLoc = glGetUniformLocation(ObvodkaShad.Program, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        viewLoc = glGetUniformLocation(ObvodkaShad.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        projLoc = glGetUniformLocation(ObvodkaShad.Program, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        float scale = 1.1;
        glBindVertexArray(sceneVAO);
        for(GLuint i = 0; i < 10; i++)
        {

            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            //GLfloat angle = radians((GLfloat)glfwGetTime() *20.0f * (i + 1));
            //GLfloat angle = radians(20.0f * (i + 1));
            //model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);



        // for obvodka
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);

        billboardShad.Use();
        billboardShad.setInt("texture1", 2);
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < billboardPos.size(); i++)
        {
            float distance = length(cameraPos - billboardPos[i]);
            sorted[distance] = billboardPos[i];
        }

        modelLoc = glGetUniformLocation(billboardShad.Program, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        viewLoc = glGetUniformLocation(billboardShad.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        projLoc = glGetUniformLocation(billboardShad.Program, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, billboardTex);
        glBindVertexArray(billboardVAO);
        for(std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        glStencilMask(0xFF);
        //glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &sceneVAO);
    glDeleteBuffers(1, &VBO);


    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
    glfwTerminate();
    return 0;

}