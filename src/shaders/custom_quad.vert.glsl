#version 450

layout(set = 1, binding = 1) uniform EntityInfo
{
    vec4 xywh;  // x, y, width, height
};

void main()
{
    uint vert = uint(gl_VertexIndex);
    vec2 pos = positions[vert];           // -1 to 1 unit quad
    vec2 scaled = pos * 0.5 + 0.5;       // remap to 0..1
    vec2 world = xywh.xy + scaled * xywh.zw;  // place at x,y with size w,h
    gl_Position = view * vec4(world, 0.0f, 1.0f);
}
