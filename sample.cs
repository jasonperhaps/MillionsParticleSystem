#version 430

layout(std140, binding=1)buffer Pos
{
    vec4 pos[];
};

layout(std140, binding=2)buffer Vel
{
    vec4 vel[];
};

layout(local_size_x=128, local_size_y=1, local_size_z=1)in;

uniform sampler3D U_Texture;


vec3 GetVel(vec3 texcoord)
{
    return texture(U_Texture, texcoord/2.0f).xyz;
}

vec3 CalculateV(vec3 p)
{
    vec3 vel=vec3(0.0);
    float gain=1.0;
    float expand=1.0f;
    for(int i=0;i<4;i++)
    {
        vel+=GetVel(p*expand)*gain;
        expand*=2.0;
        gain*=0.5;
    }
    return vel;
}

void main()
{
    uint particleID=gl_GlobalInvocationID.x;
    if(particleID>=1048576) return;

    vec3 p=pos[particleID].xyz;
    vec3 v=vel[particleID].xyz;

    v+=CalculateV(p)*0.0002;
    p+=v;
    v*=0.90;

    pos[particleID]=vec4(p, 1.0);
    vel[particleID]=vec4(v, 1.0);

    
}