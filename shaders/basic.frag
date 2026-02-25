#version 410 core 

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

// Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 lightPos2;
uniform vec3 lightColor2;


uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

float computeFog(float dist)
{
    float fogDensity = 0.003f; 

    float fogFactor = exp(-pow(dist * fogDensity, 2));
    return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
   
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);
    vec3 viewDir = normalize(-fPosEye.xyz);

    vec3 lightDirN = normalize(vec3(view * vec4(lightDir, 0.0f)));
    

    vec3 ambient1 = ambientStrength * lightColor;
    

    vec3 diffuse1 = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
    
    vec3 reflectDir1 = reflect(-lightDirN, normalEye);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0f), shininess);
    vec3 specular1 = specularStrength * spec1 * lightColor;

    vec3 lightPosEye2 = vec3(view * vec4(lightPos2, 1.0f));
    vec3 lightDir2 = normalize(lightPosEye2 - fPosEye.xyz);
    float dist = length(lightPosEye2 - fPosEye.xyz);
    
    // attenuation
    float att = 1.0 / (1.0 + 0.1 * dist + 0.05 * dist * dist);

    vec3 ambient2 = ambientStrength * lightColor2;

    vec3 diffuse2 = max(dot(normalEye, lightDir2), 0.0f) * lightColor2;

    vec3 reflectDir2 = reflect(-lightDir2, normalEye);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0f), shininess);
    vec3 specular2 = specularStrength * spec2 * lightColor2;

    vec3 finalAmbient = ambient1 + (ambient2 * att);
    vec3 finalDiffuse = diffuse1 + (diffuse2 * att);
    vec3 finalSpecular = specular1 + (specular2 * att);

    vec3 texDiffuse = texture(diffuseTexture, fTexCoords).rgb;
    vec3 texSpecular = texture(specularTexture, fTexCoords).rgb;

    vec3 color = min((finalAmbient + finalDiffuse) * texDiffuse + finalSpecular * texSpecular, 1.0f);

   //fog

    float distToCamera = length(fPosEye.xyz);
    float fogFactor = computeFog(distToCamera);
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f); // Gri

    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);

}