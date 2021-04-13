#version 440

in vec3 Position;
in vec3 Normal;

struct LightInfo {
  vec4 Position; // Light position in eye coords.
  vec3 La;       // Ambient light intensity
  vec3 Ld;       // Diffuse light intensity
  vec3 Ls;       // Specular light intensity
};

uniform LightInfo Light;

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float shiness;    // Specular shininess factor (phong exponent)
};
uniform MaterialInfo Material;

out vec4 FragColor;

void main() {
    vec3 L = normalize(vec3(Light.Position - vec4(Position,1)));
	vec3 V = normalize(-1 * vec3(Position));

	vec3 R = normalize(reflect(-1 * L, Normal));

	vec3 ambient = Light.La * Material.Ka;
	vec3 diffuse = Light.Ld * Material.Kd * max(dot(L,Normal),0.0);
	vec3 spec = Light.Ls * Material.Ks * pow(max(dot(R,V), 0.0), Material.shiness);
	
    FragColor = vec4(ambient + diffuse + spec, 1);
}
