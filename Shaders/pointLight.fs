#version 330 core

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 color;

in vec3 fragPos;  
in vec3 our_normal;
in vec2 our_uv;
 
uniform vec3 viewPos;
uniform Light light;  
uniform sampler2D texture1;

void main()
{
    // Ambient
    vec3 ambient  = light.ambient * vec3(texture(texture1, our_uv));
  	
    // Diffuse 
    vec3 norm = normalize(our_normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * vec3(texture(texture1, our_uv)));
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * (spec * vec3(texture(texture1, our_uv)));  
        
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}
