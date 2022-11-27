#version 330 core
out vec4 FragColor;  
in vec3 color_from_vertex;


void main() {
    FragColor = vec4(color_from_vertex, 1.0);
}