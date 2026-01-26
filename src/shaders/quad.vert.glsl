#version 450

layout(set = 1, binding = 0) uniform CameraInfo
{
    mat4 view;
};

layout(set = 1, binding = 1) uniform EntityInfo
{
    vec2 position;
};

const vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0)
);

void main()
{
    uint vert = uint(gl_VertexIndex);
	gl_Position = view * vec4(positions[vert] * 0.3f + position, 0.0f, 1.0f);
}
