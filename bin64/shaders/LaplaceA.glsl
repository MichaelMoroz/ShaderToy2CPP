#version 330 core
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform int       iFrame;                // shader playback frame
uniform float     iChannelTime[4];       // channel playback time (in seconds)
uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform vec4      iDate;                 // (year, month, day, time in seconds)
uniform float     iSampleRate;           // sound sample rate (i.e., 44100)

uniform sampler2D iChannel0;
#define R iResolution.xy
#define A(U) texture(iChannel0,(U)/R)
#define PI 3.14159265
//use max possible dt without sim breaking
#define dt sqrt(2.)
//apparently its sqrt(2)



void mainImage( out vec4 Q, in vec2 U )
{
    Q = A(U);
    vec4 m = 0.25*(A(U+vec2(0,1))+A(U-vec2(0,1))+A(U+vec2(1,0))+A(U-vec2(1,0)));
	
    //wave 
    float DT = dt*(1. - 0.5*exp(-distance(R*vec2(0.5, 0.5), U)/150.));
    
    Q.y += DT*(m.x-Q.x);
    Q.x += DT*Q.y;

    //cell interaction
    vec4 mval = mod(4.*m,9.);
    Q.z = mval.z;
 
    if (iFrame < 5) {
    	Q = exp(-pow(40.*length(U-vec2(0.9, 0.8)*R)/R.x, 2.))*vec4(sin(PI*0.5*U.x),PI*0.5*cos(PI*0.5*U.x),0,0);
    }
}

out vec4 FragColor;
in vec2 uv;

void main()
{    
	mainImage(FragColor, uv*iResolution.xy);
	FragColor.w = 1.;
}