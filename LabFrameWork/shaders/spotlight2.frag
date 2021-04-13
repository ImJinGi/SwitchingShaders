#version 400
in vec3 Position;
in vec3 Normal;

struct LightInfo {
	vec3 position; // Light position in eye coords.
	vec3 direction; 		//빛의 방향
	vec3 La;       // Ambient light intensity
	vec3 Ld;       // Diffuse light intensity
	vec3 Ls;       // Specular light intensity

	vec3 intensity;
	float exponent;    		 //약화계수
	float cutOff; 			//빛의 각도
	
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


void main() 
{
	vec3 norm = normalize(Normal);
	vec3 ambient = Light.La + Material.Ka;
	
	vec3 L = normalize(Light.position - Position);
	float cosAng = dot( -L, normalize(Light.direction));
	float angle = acos(cosAng);
	float spotScale = 0.0;
	
	vec3 diffuse = vec3(0), specular = vec3(0);

	if(angle < Light.cutOff ) 
	{
		spotScale = pow( cosAng, Light.exponent );
		float sDotN = max( dot(L, norm), 0.0 );
		diffuse = Light.Ld * Material.Kd * sDotN;
		if( sDotN > 0.0 ) 
		{
			vec3 V = normalize(-Position.xyz);
			vec3 H = normalize( V + L );
			specular = Light.Ls * Material.Ks * pow( max( dot(H, norm), 0.0 ), Material.shiness );
		}
	}

	FragColor = vec4(ambient + spotScale * Light.intensity * (diffuse + specular), 1);
}
