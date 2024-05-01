#version 300 es

precision highp float; // highp mediump lowp

in vec2 textureCoords;
in vec3 normal;
in vec3 fragPos;

out vec4 outColor;

uniform sampler2D diffuseTexture;

uniform vec3 sunLightDir;
uniform vec3 cameraPos;

uniform float ambientLight;

void main()
{
    // diffuse
    float diffuse = max(dot(normal, -sunLightDir), 0.0f);

    vec3 textureCollor = texture(diffuseTexture, textureCoords).rgb;
    outColor = vec4((ambientLight + diffuse) * textureCollor, 1.0f);
}
