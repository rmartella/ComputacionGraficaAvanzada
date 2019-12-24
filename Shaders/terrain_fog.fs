#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct  DirectionalLight{
    vec3 direction;
    Light light;
};

struct  PointLight{
    vec3 position;
    
	Light light;
	
	float constant;
    float linear;
    float quadratic;
};

struct  SpotLight{
	vec3 position;
	vec3 direction;

	Light light;

	float cutOff;
	float outerCutOff;

	float constant;
    float linear;
    float quadratic;

};

const int MAX_POINT_LIGHTS = 20;
const int MAX_SPOT_LIGHTS = 1;

out vec4 color;

in vec3 fragPos;  
in vec3 our_normal;
in vec2 our_uv;
in float visibility;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 viewPos;
  
uniform vec2 scaleUV;  
  
uniform sampler2D backgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMapTexture;

uniform vec3 fogColor;

vec3 calculateDirectionalLight(Light light, vec3 direction){
	vec2 tiledCoords = our_uv;
	if(tiledCoords.x != 0 && tiledCoords.y != 0)
		tiledCoords = scaleUV * tiledCoords;

	vec4 blendMapColor = texture(blendMapTexture, our_uv);
	float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec4 backgroundTextureColor = texture(backgroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTextureColor = texture(rTexture, tiledCoords) * blendMapColor.r;
	vec4 gTextureColor = texture(gTexture, tiledCoords) * blendMapColor.g;
	vec4 bTextureColor = texture(bTexture, tiledCoords) * blendMapColor.b;
	vec4 totalColor = backgroundTextureColor + rTextureColor + gTextureColor + bTextureColor;

	// Ambient
    vec3 ambient  = light.ambient * vec3(totalColor);
  	
    // Diffuse 
    vec3 normal = normalize(our_normal);
    vec3 lightDir = normalize(-direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * vec3(totalColor));
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * (spec * vec3(totalColor));  
        
    return (ambient + diffuse + specular);
}

vec3 calculatePointLights(){
	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < pointLightCount; i++){
		vec3 lightDir = normalize(fragPos - pointLights[i].position );
		float distance = length(pointLights[i].position - fragPos);
		float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
		result +=  attenuation * calculateDirectionalLight(pointLights[i].light, lightDir);
	}
	return result;
}

vec3 calculateSpotLights(){
	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < spotLightCount; i++){
		vec3 lightDir = normalize(spotLights[i].position - fragPos);
		float theta = dot(lightDir, normalize(-spotLights[i].direction));   
		float epsilon   = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);   
		float distance = length(spotLights[i].position - fragPos);
		float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * distance + spotLights[i].quadratic * distance * distance);
		result +=  intensity * attenuation * calculateDirectionalLight(spotLights[i].light, spotLights[i].direction);
	}
	return result;
}

void main()
{
    color = vec4(calculateDirectionalLight(directionalLight.light, directionalLight.direction) + calculatePointLights() + calculateSpotLights(), 1.0);
	color = mix(vec4(fogColor, 1.0), color, visibility);
}