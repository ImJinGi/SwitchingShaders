#version 400

in vec3 VertexPosition;
in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix; 
uniform mat4 MVP;

void main(void)
{
   Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1));
   Normal = normalize(NormalMatrix * VertexNormal);

   gl_Position = MVP * vec4(VertexPosition, 1);
}
