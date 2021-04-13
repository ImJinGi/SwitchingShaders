#version 440

in vec3 VertexPosition;
in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

out VS_OUT
{
	vec4 color;
}vs_out;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix; 
uniform mat4 MVP;

uniform ModelViewProjection
{
	mat4 modelViewMat;
	mat3 normalMat;
	mat4 mvp;
}_mvp;


void main()
{
	vs_out.color = vec4(VertexNormal, 1.0);

	Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1));
    Normal = normalize(NormalMatrix * VertexNormal);

	gl_Position = MVP * vec4(VertexPosition, 1);
}
