#version 460 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal; 
out vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 model;


void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    //gl_Position = projection * view * vec4(FragPos, 1.0);  
    //TexCoord = aTexCoord;
}
