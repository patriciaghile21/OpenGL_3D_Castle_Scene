#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

gps::Window myWindow;

GLenum polygonMode = GL_FILL;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;


// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

glm::vec3 lightPos2;
glm::vec3 lightColor2;
GLint lightPos2Loc;
GLint lightColor2Loc;


gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 2.0f, 15.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.5f;

GLboolean pressedKeys[1024];

// mouse handling
bool firstMouse = true;
float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
float castleOffset = 0.0f; 
float sceneTranslation = 0.0f; 
float cameraOrbitAngle = 0.0f;
bool isOrbiting = false; 

gps::Model3D castle;   
gps::Model3D ground;  
gps::Model3D myTower;
gps::Model3D myMoon;
gps::Model3D myTree;
gps::Model3D light;
gps::Model3D wolf;


GLfloat angle;
float scaleFactor = 1.0f;
bool startAnimation = false;
bool animateLight = true; 

gps::Shader myBasicShader;

GLuint moonTextureID;
GLuint treeTextureID;

struct RainDrop {
    float x, y, z;
    float speed;
};

const int nrDrops = 2000;
RainDrop rain[nrDrops];
GLuint rainTextureID; 

//cod din lab
unsigned int LoadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    //function from lab
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture not found at: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        startAnimation = !startAnimation;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        polygonMode = GL_LINE; 

    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        polygonMode = GL_FILL;

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        polygonMode = GL_POINT;
  
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        animateLight = !animateLight;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isOrbiting = !isOrbiting; 
        if (isOrbiting) cameraOrbitAngle = 0.0f;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
}

void processMovement() {
    if (isOrbiting) {
     
        cameraOrbitAngle += 0.008f;
      
        float radius = 50.0f;
        float camX = sin(cameraOrbitAngle) * radius;
        float camZ = cos(cameraOrbitAngle) * radius;
        float camY = 20.0f;


        myCamera.setCameraPosition(glm::vec3(camX, camY, camZ));
        myCamera.setCameraTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    }
    else {
      
        if (pressedKeys[GLFW_KEY_W]) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_S]) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_A]) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_D]) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        }
    }

    if (pressedKeys[GLFW_KEY_Q]) angle -= 1.0f;
    if (pressedKeys[GLFW_KEY_E]) angle += 1.0f;
    if (pressedKeys[GLFW_KEY_Z]) scaleFactor += 0.02f;
    if (pressedKeys[GLFW_KEY_X]) {
        scaleFactor -= 0.02f;
        if (scaleFactor < 0.1f) scaleFactor = 0.1f;
    }
    if (pressedKeys[GLFW_KEY_C]) castleOffset += 0.1f;
}

void initOpenGLWindow() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    myWindow.Create(screenWidth, screenHeight, "OpenGL Project - Castle Scene");

}
void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void initModels() {
    // models
    castle.LoadModel("models/castle/Castle.obj");
    ground.LoadModel("models/ground/ground.obj");
    myMoon.LoadModel("models/moon/Moon_2K.obj");
    myTree.LoadModel("models/tree/tree.obj");

    // textures
    moonTextureID = LoadTexture("models/moon/Diffuse_2K.png");
    treeTextureID = LoadTexture("models/tree/brown.png");


    light.LoadModel("models/light/light.obj");
    wolf.LoadModel("models/wolf/wolf.obj");

    // load texture for rain once
    rainTextureID = LoadTexture("models/rain/rain.png");

    // initializing rain drop position
    for (int i = 0; i < nrDrops; i++) {

        // generate a bigger radius 
        rain[i].x = (rand() % 600) - 300;
        rain[i].z = (rand() % 600) - 300;

        rain[i].y = (rand() % 50) + 20;
        rain[i].speed = 0.5f + (rand() % 10) / 10.0f;
    }
}

void initSkyBox() {
    
    skyboxShader.loadShader(
        "C:\\GP\\project\\project\\shaders\\skyboxShader.vert",
        "C:\\GP\\project\\project\\shaders\\skyboxShader.frag"
    );

    if (skyboxShader.shaderProgram == 0) {
        std::cerr << "Error skybox shader didn't compile!" << std::endl;
        std::cerr << "Verify shader files." << std::endl;
    
        exit(-1);
    }

    // prepare the faces 
    std::vector<const GLchar*> faces;
    faces.push_back("C:\\GP\\project\\project\\models\\skybox\\right.png");
    faces.push_back("C:\\GP\\project\\project\\models\\skybox\\left.png");
    faces.push_back("C:\\GP\\project\\project\\models\\skybox\\top.png");
    faces.push_back("C:\\GP\\project\\project\\models\\skybox\\bottom.png");
    faces.push_back("C:\\GP\\project\\project\\models\\skybox\\back.png");
    faces.push_back("C:\\GP\\project\\project\\models\\skybox\\front.png");

    mySkyBox.Load(faces);
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // initialize model matrix 
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");


    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 1000.0f); 
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // directional light
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // point light
    lightPos2 = glm::vec3(0.0f, 2.0f, 5.0f);
    lightPos2Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos2");
    glUniform3fv(lightPos2Loc, 1, glm::value_ptr(lightPos2));

    lightColor2 = glm::vec3(1.0f, 0.8f, 0.6f); // orange color
    lightColor2Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor2");
    glUniform3fv(lightColor2Loc, 1, glm::value_ptr(lightColor2));
}

void renderCastle(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);

    // Aplicăm translația folosind castleOffset pe axa Z
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, castleOffset));

    // Apoi rotația și scalarea (ordinea contează!)
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    castle.Draw(shader);
}

void renderGround(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));

    model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    ground.Draw(shader);
}

void renderTree(glm::vec3 position, glm::vec3 scale, float rotationAngle) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::scale(model, scale);

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, treeTextureID);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseTexture"), 0);

    myTree.Draw(myBasicShader);
}

void renderRain() {
   
    glUseProgram(myBasicShader.shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rainTextureID);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseTexture"), 0);

    // take the location of model matrix once for speed 
    GLint modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    for (int i = 0; i < nrDrops; i++) {

        // move the rain drop
        rain[i].y -= rain[i].speed;
        if (rain[i].y < 0.0f) rain[i].y = 60.0f;

        // create model matrix 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(rain[i].x, rain[i].y, rain[i].z));

        model = glm::scale(model, glm::vec3(0.01f, 0.6f, 0.01f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // draw using the  tree model
        myTree.Draw(myBasicShader);
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    myBasicShader.useShaderProgram();

    // lights setup

    glm::vec3 lampLightPos = glm::vec3(4.0f, 11.5f, 18.0f);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightPos2"), 1, glm::value_ptr(lampLightPos));

    // high intensity
    glm::vec3 orangeLight = glm::vec3(3.0f, 2.0f, 0.5f);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightColor2"), 1, glm::value_ptr(orangeLight));

    if (startAnimation) { angle += 0.5f; }

    glm::mat4 view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // drawing the objects 

    renderGround(myBasicShader);

    glDisable(GL_CULL_FACE);
    renderCastle(myBasicShader);
    glEnable(GL_CULL_FACE);

   // draw the light 

    glDisable(GL_CULL_FACE);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glBindTexture(GL_TEXTURE_2D, 0);

    glm::mat4 modelLight = glm::mat4(1.0f);
    modelLight = glm::translate(modelLight, glm::vec3(4.0f, 0.0f, 18.0f));
    modelLight = glm::scale(modelLight, glm::vec3(200.0f));
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelLight));
    light.Draw(myBasicShader);

    // drawing the wolf in the castle yard 

    glDisable(GL_CULL_FACE);

    glm::mat4 modelWolf = glm::mat4(1.0f);
  
    modelWolf = glm::translate(modelWolf, glm::vec3(-5.0f, -8.3f, 10.0f));
    modelWolf = glm::rotate(modelWolf, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelWolf = glm::scale(modelWolf, glm::vec3(1.0f));

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelWolf));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moonTextureID); // using moon's texture 
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "useTexture"), 1);

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

    wolf.Draw(myBasicShader);

    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_CULL_FACE);

    // moon

    glm::mat4 modelLuna = glm::mat4(1.0f);
    modelLuna = glm::translate(modelLuna, glm::vec3(80.0f, 60.0f, -20.0f));
    modelLuna = glm::rotate(modelLuna, (float)glfwGetTime() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    modelLuna = glm::scale(modelLuna, glm::vec3(2.0f));
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelLuna));
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, moonTextureID);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "diffuseTexture"), 0);
    myMoon.Draw(myBasicShader);
    glBindTexture(GL_TEXTURE_2D, 0);

    // forests
    glm::mat4 resetModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(resetModel));
    for (int i = 0; i < 30; i++) {
        renderTree(glm::vec3(45.0f + (i % 10 * 4.5f), 0.0f, -30.0f + (i / 3 * 4.0f)), glm::vec3(1.0f), i * 43.0f);
        renderTree(glm::vec3(-(45.0f + (i % 10 * 4.5f)), 0.0f, -30.0f + (i / 3 * 4.0f)), glm::vec3(1.0f), i * 77.0f);
    }

    // skybox
    if (skyboxShader.shaderProgram != 0) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        skyboxShader.useShaderProgram();
        glm::mat4 viewSkybox = glm::mat4(glm::mat3(view));
        mySkyBox.Draw(skyboxShader, viewSkybox, projection);
    }

    myBasicShader.useShaderProgram();
    renderRain();
}

void cleanup() {
    myWindow.Delete();
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();

    initSkyBox();

    setWindowCallbacks();

    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glCheckError();
    
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}