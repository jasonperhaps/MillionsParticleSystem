varying vec2 V_Texcoord;

void main()
{
    float distance=length(V_Texcoord*2.0f-vec2(1.0))*3.0;
    float alpha=exp(-distance*distance);
    if(alpha<0.01) discard;
    
    gl_FragColor=vec4(0.1, 0.3, 0.7, alpha);
}