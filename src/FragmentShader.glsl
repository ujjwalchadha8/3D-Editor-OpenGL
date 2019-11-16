#version 150 core\n
in vec3 Normal;
in vec3 FragPos;
in vec3 objectColor;
out vec4 outColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool flat_normal;
void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
      float ambientStrength = 0.01f;
      vec3 ambient = ambientStrength * lightColor;
      vec3 norm = normalize(Normal);
      vec3 lightDir = normalize(lightPos - FragPos);
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diff * lightColor;
      if(flat_normal){
         vec3 result = (ambient + diffuse) * objectColor;
         outColor = vec4(result, 1.0);
       }else {

          float specularStrength = 0.5f;
          vec3 viewDir = normalize(viewPos - FragPos);
          vec3 reflectDir = reflect(-lightDir, norm);
          float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
          vec3 specular = specularStrength * spec * lightColor;
          vec3 result = (ambient + diffuse + specular) * objectColor;
          outColor = vec4(result, 1.0);
      }
}
