#ifndef SkyBox_hpp
#define SkyBox_hpp

#include <vector>
#include <string>

// Includem GLM pentru matrici
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Includem OpenGL si Shader-ul tau
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Shader.hpp" // Asigura-te ca Shader.hpp e in acelasi folder

namespace gps {

    class SkyBox {
    public:
        // Constructor
        SkyBox();

        // Functia principala de incarcare (primeste cele 6 nume de fisiere)
        void Load(std::vector<const GLchar*> cubeMapFaces);

        // Functia de desenare
        void Draw(gps::Shader& shader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

        // Getter pentru ID-ul texturii (optional, dar util)
        GLuint GetTextureId();

    private:
        GLuint skyboxVAO;
        GLuint skyboxVBO;
        GLuint cubemapTexture;

        // Functii ajutatoare interne
        GLuint LoadSkyBoxTextures(std::vector<const GLchar*> skyBoxFaces);
        void InitSkyBox();
    };

}

#endif /* SkyBox_hpp */