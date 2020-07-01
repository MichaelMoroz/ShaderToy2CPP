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
	vec2 wolf_center = R*vec2(0.5, 0.7);
	float wolf_scale = 0.3;
	vec2 wolf_world_size = wolf_size*wolf_scale;
	vec2 sampling_pos = clamp((P - (wolf_center - wolf_world_size*0.5))/wolf_world_size, vec2(0.), vec2(1.));
	sampling_pos = vec2(sampling_pos.x, 1. - sampling_pos.y);
	vec4 eyes = texture(ch1, sampling_pos);
	
	vec2 mouse = vec2(iMouse.x, R.y - iMouse.y);
	vec2 delta = (P - wolf_center)/R.x;
    float r = length(delta);
	float fur_l = 1. - 1.*tanh(3.5*r);
    delta = 3.5*vec2(-1, 1)*normalize(delta)*atan(6.*r); // point of view stuff
	
	vec4 wolf = fur(sampling_pos, delta, 0.4*wolf_size, fur_l);
	
	vec4 eye_bloom = vec4(0.);
	for(int i = -6; i <= 6; i++)
	{
		for(int j = -6; j <= 6; j++)
		{
			vec2 dx = vec2(i,j);
			vec4 eye = texture(ch1, sampling_pos + dx/vec2(340,1600));
			eye_bloom += kernel(dx/4.)*vec4(removewhite(eye).xyz,1.);
		}
	}
	eye_bloom.xyz /= eye_bloom.w;
	O = wolf + 0.*(0.7 + 0.3*sin(vec4(1,2,3,4)*(3.*iTime + 0.02*(P.x + 0.5*P.y))))*eye_bloom + 0.*removewhite(eyes);
}

out vec4 FragColor;
in vec2 uv;

void main()
{    
	mainImage(FragColor, uv*iResolution.xy);
	FragColor.w = 1.;
}