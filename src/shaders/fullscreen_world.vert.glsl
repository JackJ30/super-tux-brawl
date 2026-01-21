#version 450

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
    vec2 pos = positions[vert];
	gl_Position = vec4(pos, 0.0f, 1.0f);
}
