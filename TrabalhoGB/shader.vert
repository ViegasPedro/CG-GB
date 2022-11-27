#version 330 core
layout (location = 0) in vec3 values;

uniform mat4 projection;
out vec3 color_from_vertex;

void main() {
	color_from_vertex = vec3(0.0, 0.0, values.z);
    gl_Position = projection * vec4(values.x, values.y, 0.0, 1.0);
}    