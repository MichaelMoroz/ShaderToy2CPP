#version 330 core
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform int       iFrame;                // shader playback frame
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform vec4	  iMouseSpeed;           // mouse speed

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;

#define R iResolution.xy
#define pixel(ch, p) texture(ch,(p)/R)
#define ch0 iChannel0
#define ch1 iChannel1
#define ch2 iChannel2
#define ch3 iChannel3
#define PI 3.14159265

vec2 dir(float x)
{
    return vec2(cos(x), sin(x));
}

vec2 wind(vec2 p, float t)
{
    t*=15.;
    
    vec2 dx = 0.07*dir(dot(p, vec2(0.4, 0.7)) + t) + 
           0.04*dir(dot(p, vec2(0.8, -0.5)) + 0.625*t)+
           0.005*dir(dot(p, vec2(-3., 0.7)) + 3.14*t)+
           0.04*dir(dot(p, vec2(-0.6, -0.3)) + 1.133*t)+
           0.26*dir(dot(p, vec2(0.1, 0.11)) + 0.431*t)+
           0.35*dir(dot(p, vec2(-0.12, 0.1)) + 0.256*t);
    return 0.6*dx;
}

float G(vec2 dx)
{
	return exp(-dot(dx,dx));
}

vec3 Noise(vec2 p)
{
	vec2 size = textureSize(ch3, 0); 
	p = mod(p, size);
	return texture(ch3, p/size).xyz;
}

vec3 Wolf(vec2 p)
{
	vec4 s = pixel(ch0, p);
	return s.xyz*s.w;
}

vec3 WolfFur(vec2 p)
{
	vec4 s = pixel(ch2, p);
	return vec3(2.*s.xy - 1.,s.w);
}

#define H 23

vec4 fur(vec2 p, vec2 d, float fur_l)
{
    vec4 col = vec4(0.);
    vec2 dx = 1.5*wind(p*0.3, iTime);
    
    for(int i = 0; i < 23; i++)
    {
	    float k = float(i)/float(H); //the depth
      
        vec2 pos = p + fur_l*(d+dx)*(k-0.5)*float(H);  
        
        vec3 c = Wolf(pos); //color
		vec3 m = Noise(pos);
		c *= m.x;
       
        float alpha = tanh(0.07*length(c)); //transparency
     
        float B = 8.*k; //brightness
        col = mix(col, vec4(B*c, alpha), alpha); //blend
    }
    
    return col;
}

void mainImage( out vec4 O, in vec2 P )
{
	vec2 wolf_size = textureSize(ch0, 0);
	//P = R*vec2(0.25, 0.6) + 0.4*P;
	vec2 mouse = vec2(iMouse.x, R.y - iMouse.y);
	vec2 delta = (P - R*0.5)/R.x;
    float r = length(delta);
	float fur_l = 1. - 1.*tanh(1.*r);
    vec3 furparam = WolfFur(P); 
	vec4 wolf = fur(P, 2.*furparam.xy, furparam.z*fur_l);
	O = sqrt(wolf);
}

out vec4 FragColor;
in vec2 uv;

void main()
{    
	mainImage(FragColor, uv*iResolution.xy);
	FragColor.w = 1.;
}