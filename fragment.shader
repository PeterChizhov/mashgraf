#version 330 core
struct Material {
    //vec3 ambient;
    //vec3 diffuse;
    sampler2D diffuse;  // diffuse map
    //vec3 specular;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
//in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;


uniform vec3 lightPos;
uniform vec3 viewPos;


//uniform sampler2D ourTexture;

void main()
{
//    color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
