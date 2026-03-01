#version 450

layout(set = 1, binding = 0) uniform View
{
    mat4 view;
};

layout(set = 3, binding = 0) uniform EntityInfo
{
    vec4 xywh; // x, y, width, height
};

const vec2 QUAD_POSITIONS[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

void main()
{
    vec2 local = QUAD_POSITIONS[gl_VertexIndex];
    vec2 world = xywh.xy + local * xywh.zw;
    gl_Position = view * vec4(world, 0.0, 1.0);
}
