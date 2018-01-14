#version 430
layout(std140,binding=1)buffer Pos{
    vec4 pos[];
};


uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
varying vec2 V_Texcoord;

void main()
{
    uint particleID=gl_VertexID>>2;
    vec4 p=pos[particleID];

    vec2 texcoord=vec2(((gl_VertexID-1)&2)>>1, (gl_VertexID&2)>>1);
    vec4 posInView=V*M*p;
    float spriteSize=0.010;
    vec4 fixedPos=posInView+vec4(vec2(texcoord.x*2.0-1.0, texcoord.y*2.0-1.0)*spriteSize, 0.0, 1.0);
    V_Texcoord=texcoord;
    gl_Position=P*fixedPos;
}