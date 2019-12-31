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
in vec4 fragPosLightSpace;
in vec3 our_normal;
in vec2 our_uv;
in float visibility;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 viewPos;  
uniform sampler2D texture1;

uniform sampler2D shadowMap;

uniform vec3 fogColor;

float calculateShadow(vec3 lightDir){
	// Perform perspective division in the case orthographic projection is w is 1
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Perfomr transform to middle NDC [0, 1]
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // This is the first version of the shadow mapping with shadow acne
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    // This is the second version of the shadow mapping with shadow bias to correct the shadow acne
    // float bias = 0.005;
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	// This is the third version of the shadow mapping with shadow bias to correct the shadow acne
	// vec3 normal = normalize(our_normal);
	// float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);  
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	// This is the four version of the shadow mapping with shadow bias to correct the shadow acne
	// vec3 normal = normalize(our_normal);
	// float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);  
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// if(projCoords.z > 1.0)
    //	shadow = 0.0;


    // This is the final version of the shadow mapping with shadow bias to correct the shadow acne
    vec3 normal = normalize(our_normal);
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);  
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;
	if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 calculateDirectionalLight(Light light, vec3 direction){
	// Ambient
    vec3 ambient  = light.ambient * vec3(texture(texture1, our_uv));
  	
    // Diffuse 
    vec3 normal = normalize(our_normal);
    vec3 lightDir = normalize(-direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * vec3(texture(texture1, our_uv)));
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * (spec * vec3(texture(texture1, our_uv)));
    
    float shadow = calculateShadow(-lightDir);
        
    return (ambient + (1.0 - shadow) * (diffuse + specular));
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
	vec4 colorText = texture(texture1, our_uv);
	if(colorText.a < 0.1)
		discard;
    color = vec4(calculateDirectionalLight(directionalLight.light, directionalLight.direction) + calculatePointLights() + calculateSpotLights(), colorText.a);
    color = mix(vec4(fogColor, 1.0), color, visibility);
}