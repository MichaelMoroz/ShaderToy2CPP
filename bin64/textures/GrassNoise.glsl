#ifdef VS
uniform mediump vec3 msk[4];
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec4 gl_Position;
out vec2 result_uv;

void program_main_vs()
{
	gl_Position = vec4(position.xyz, 1.0); 
	result_uv = uv;
}
#endif

#ifdef PS
uniform sampler2D	base_texture;
uniform sampler2D	noise_texture;
uniform sampler2D	floor_texture;
uniform vec2		uResolution;
uniform highp mat4	InverseViewProjectionMatrix;

in vec2	result_uv;

out vec4 Color;
out vec4 Normal;

void program_main_ps()
{	
	vec4 ndcPosition = vec4(
        (gl_FragCoord.x / uResolution.x - 0.5) * 2.0,
        (gl_FragCoord.y / uResolution.y - 0.5) * 2.0,
        (gl_FragCoord.z - 0.5) * 2.0,
        1.0);

	vec4 wPosition = InverseViewProjectionMatrix * ndcPosition;
	wPosition /= wPosition.w;

	float alpha = texture2D(base_texture, result_uv).a;
	vec3 grassColor = vec3(73.0, 86.0, 46.0) / 255.0;
	vec3 floorColor = texture2D(floor_texture, gl_FragCoord.xy / uResolution).xyz;

	vec2 v2TileSize = vec2(400, 400);
	vec2 v2TexCoords = wPosition.xy / v2TileSize;
	float fNoiseIntensity = 0.8 - texture2D(noise_texture, v2TexCoords).r;

	Color.rgb = mix(grassColor, floorColor, fNoiseIntensity);

	v2TileSize = vec2(200, 200);
	v2TexCoords = wPosition.xy / v2TileSize;
	fNoiseIntensity = 1.0 - texture2D(noise_texture, v2TexCoords).r;

	Color.rgb *= fNoiseIntensity;
	Color.a = alpha;

	Normal = vec4(0.5, 0.5, 1.0, Color.a);
}
#endif