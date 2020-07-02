#version 330 core
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform int       iFrame;                // shader playback frame
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform vec4	  iMouseSpeed;           // mouse speed

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

#define R iResolution.xy
#define pixel(ch, p) texture(ch,(p)/R)
#define ch0 iChannel0
#define ch1 iChannel1
#define PI 3.14159265

vec2 dir(float x)
{
    return vec2(cos(x), sin(x));
}

vec2 wind(vec2 p, float t)
{
    t*=5.;
    
    vec2 dx = 0.07*dir(dot(p, vec2(0.4, 0.7)) + t) + 
           0.04*dir(dot(p, vec2(0.8, -0.5)) + 0.625*t)+
           0.01*dir(dot(p, vec2(-3., 0.7)) + 3.14*t)+
           0.04*dir(dot(p, vec2(-0.6, -0.3)) + 1.133*t)+
           0.26*dir(dot(p, vec2(0.013, 0.005)) + 0.431*t)+
           0.35*dir(dot(p, vec2(-0.01, 0.012)) + 0.256*t);
    return 0.6*dx;
}

vec4 removewhite(vec4 c)
{
	float d = distance(c.xyz, vec3(1.)); //distance to white 
	return step(0.5, d)*c; 
}

float kernel(vec2 dx)
{
	return exp(-dot(dx,dx));
}

#define H 23

vec4 fur(vec2 p, vec2 d, vec2 de, float fur_l)
{
    vec4 col = vec4(0.);
    vec2 dx = 2.*wind(p*de, 4.*iTime);
    
    for(int i = 0; i < 23; i++)
    {
	    float k = float(i)/float(H); //the depth
      
        vec2 pos = p + fur_l*(d+dx)*(k-0.5)*float(H)/de;  
        
        vec3 c = removewhite(texture(ch0, pos)).xyz; //color
       
        float alpha = tanh(0.07*length(c)); //transparency
     
        float B = 8.*k; //brightness
        col = mix(col, vec4(B*c, alpha), alpha); //blend
    }
    
    return col;
}

void mainImage( out vec4 O, in vec2 P )
{
	vec2 wolf_size = textureSize(ch0, 0);
	vec2 uv = P/R; uv = vec2(uv.x, 1. - uv.y);
	
	vec3 dx = vec2(-1., 0., 1.);
	vec2 grad = vec2(pixel(ch0, P+dx.zy)-pixel(ch0, P+dx.xy),pixel(ch0, P+dx.yz)-pixel(ch0, P+dx.yx));
	float b = pixel(ch0, P);
	vec3 n = normalize(vec3(grad, b));
	O.xyz = 0.5 + 0.5*n; 
}

out vec4 FragColor;
in vec2 uv;

void main()
{    
	mainImage(FragColor, uv*iResolution.xy);
	FragColor.w = 1.;
}