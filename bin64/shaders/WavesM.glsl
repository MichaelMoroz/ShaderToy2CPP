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


// Fork of "Time Inversion " by wyatt. https://shadertoy.com/view/wsySDh
// 2019-12-19 20:34:57

void mainImage( out vec4 Q, in vec2 U )
{
    vec4 t =A(U); 
     float DT = dt*(1. - 0.5*exp(-distance(R*vec2(0.5, 0.5), U)/150.));
    Q = vec4(sqrt(dot(t.xy,t.xy))*4.);
    Q.z /= 0.5*DT;
    Q.x += 0.7*(dt - DT)/dt;
}


out vec4 FragColor;
in vec2 uv;

void main()
{    
	mainImage(FragColor, uv*iResolution.xy);
	FragColor.w = 1.;
}