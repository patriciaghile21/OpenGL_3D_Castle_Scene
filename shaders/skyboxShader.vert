#version 410 core

layout (location = 0) in vec3 vPosition;

out vec3 textureCoordinates;

uniform mat4 projection;
uniform mat4 view;

void main()
{

    textureCoordinates = vPosition;

    vec4 pos = projection * view * vec4(vPosition, 1.0f);

    gl_Position = pos.xyww;
}

//cod din lab