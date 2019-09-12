#version 330 core

in vec2 our_uv;
in vec3 our_normal;
in vec3 fragPos;

struct PositionalLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

uniform PositionalLight light;
uniform vec3 viewPos;
uniform sampler2D texture0;

void main(){
	
	vec3 ambient = light.ambient * vec3(texture(texture0, our_uv));

	vec3 normal = normalize(our_normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * light.diffuse * vec3(texture(texture0, our_uv));

	vec3 r = reflect(-lightDir, normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	float spec = pow(max(dot(r, viewDir), 0.0), 32.0);
	vec3 specular = spec * light.specular * vec3(texture(texture0, our_uv));

	vec3 result = ambient + diffuse + specular;

	color = vec4(result, 1.0);

}
