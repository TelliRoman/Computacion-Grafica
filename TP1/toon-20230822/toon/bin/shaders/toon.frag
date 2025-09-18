#version 330 core

// propiedades del material
uniform vec3 objectColor;
uniform float shininess;
uniform float specularStrength;

// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec4 lightPosition;

// propiedades de la camara
uniform vec3 cameraPosition;

// propiedades del fragmento interpoladas por el vertex shader
in vec3 fragNormal;
in vec3 fragPosition;

// resultado
out vec4 fragColor;

// phong simplificado
void main() {
	
	// ambient
	vec3 ambient = lightColor * ambientStrength * objectColor ;
	
	// diffuse
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(vec3(lightPosition)); // luz directional (en el inf.)
	
	float coseno_angulo = max(dot(norm,lightDir),0.f);
	float coseno_aux = 0.f;
	if (coseno_angulo == 0.f) coseno_aux = 0.3f;
	if (coseno_angulo > 0.f && coseno_angulo <= 0.5f) coseno_aux = 0.6f;
	if (coseno_angulo > 0.5f && coseno_angulo <= 1.f) coseno_aux = 0.8f;
	if (coseno_angulo == 1.f) coseno_aux = 0.9f;
	
	vec3 diffuse = lightColor * objectColor * coseno_aux;
	
	// specular
	vec3 specularColor = specularStrength * vec3(1.f,1.f,1.f);
	vec3 viewDir = normalize(cameraPosition-fragPosition);
	vec3 halfV = normalize(lightDir + viewDir); // blinn
	
	float coseno_angulo_2 = pow(max(dot(norm,halfV),0.f),shininess);
	float coseno_aux_2 = 0.f;
	//if (coseno_angulo_2 > 0.f && coseno_angulo_2 <= 0.5f) coseno_aux_2 = 0.f;
	if (coseno_angulo_2 > 0.5f) coseno_aux_2 = 1.f;
	
	vec3 specular = lightColor * specularColor * coseno_aux_2;
	
	// result
	fragColor = vec4(ambient+diffuse+specular,1.f);
}

