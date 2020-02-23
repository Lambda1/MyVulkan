#version 450
#extension GL_ARB_separete_shader_objects : enable

layout(location = 0) in vec3 frag_color;

void main()
{
    out_color = vec4(frag_color, 1.0);
}