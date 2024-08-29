#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main()
{
   gl_Position = position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
   vec3 baseColor = vec3(224, 189, 212);
   color = vec4(baseColor/255,1.0);
};