#ifdef VS
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 gl_Position;

out VertexData
{
	float		LOD;
	float 		height;
	float 		startAngle;
	flat int 	bladeType;
	vec3		dir;
} vs_out;

void program_main_vs()
{
	gl_Position = vec4(position.xy, 0.0, 1.0); 

	vs_out.LOD = 2;
	vs_out.height = position.z;

	vs_out.startAngle = uv.x;
	vs_out.bladeType = int(uv.y);

	vs_out.dir = normalize(normal);
}
#endif

#ifdef GS
uniform highp mat4 ViewProjectionMatrix;
uniform highp mat4 InverseViewProjectionMatrix;
uniform float iTime;

layout(points) in;
layout(triangle_strip, max_vertices=18) out;

in VertexData
{
	float		LOD;
	float 		height;
	float 		startAngle;
	flat int 	bladeType;
	vec3		dir;
} gs_in[];

out GeometryData
{
	flat int bladeType;
	vec2 texCoords;
} gs_out;

struct grassVertexData
{
    vec4 position;
    vec2 texCoords;
} tri[8];

vec3 rotateZ(float a, vec3 v)
{
	return vec3(cos(a) * v.x + sin(a) * v.y, cos(a) * v.y - sin(a) * v.x, v.z);
}

void program_main_gs()
{	
	float LODLevel = 2;// gs_in[0].LOD;
	float height = 40;//gs_in[0].height /1.6;
	float width = 12;//max(min(height * 0.3, 23), 25);
	float startAngle = gs_in[0].startAngle;
	vec3  bladeDir = gs_in[0].dir;
	
	vec3 dir = bladeDir + vec3(1.0, 0.0, -1.0) * cos((iTime / 200.0) * 3.1457) * 0.4;
	dir = normalize(dir);
	
	float angle = -atan(dir.y, dir.x);

	vec4 v4WorldPointPosition = InverseViewProjectionMatrix * gl_in[0].gl_Position;

	tri[0].position = v4WorldPointPosition;
	tri[0].texCoords = vec2(0.0, 0.0);

	vec4 offsetVec = vec4(rotateZ(startAngle, vec3(width, 0, 0)), 0) * 1;
	tri[1].position = tri[0].position + offsetVec;
	tri[1].texCoords = vec2(1.0, 0.0);

	for(int i = 2; i < 8; i++)
	{
		offsetVec = vec4(0.0, height / (LODLevel + 1), 0.0, 0.0) + vec4(tri[i - 2].texCoords * vec2(width, height), 0.0, 0.0);
		offsetVec.y = -offsetVec.y;

		vec4 rotatedVec = vec4(rotateZ(startAngle + angle + angle * (i / 2) * 0.1, offsetVec.xyz), 0.0);

		tri[i].position = tri[0].position + rotatedVec;
		tri[i].texCoords = vec2(offsetVec.x / width, -offsetVec.y / height);
	}

	float offsetZ = dir.z / 15.0; 
	tri[6].position.z += -offsetZ;
	tri[7].position.z += -offsetZ;

	tri[4].position.z += -offsetZ * 0.66;	
	tri[5].position.z += -offsetZ * 0.66;

	tri[2].position.z += -offsetZ * 0.33;	
	tri[3].position.z += -offsetZ * 0.33;

	tri[0].position.z += -offsetZ * 0.1;	
	tri[1].position.z += -offsetZ * 0.1;


	tri[0].position = (ViewProjectionMatrix * tri[0].position);
	tri[1].position = (ViewProjectionMatrix * tri[1].position);
	tri[2].position = (ViewProjectionMatrix * tri[2].position);
	if(LODLevel > 0) {
		tri[3].position = (ViewProjectionMatrix * tri[3].position);
		tri[4].position = (ViewProjectionMatrix * tri[4].position);
	}
	if(LODLevel > 1) {
		tri[5].position = (ViewProjectionMatrix * tri[5].position);
		tri[6].position = (ViewProjectionMatrix * tri[6].position);
		tri[7].position = (ViewProjectionMatrix * tri[7].position);
	}

	////////////OUT VERTICES///////////
	gl_Position.xy = tri[0].position.xy; 
	gs_out.texCoords = tri[0].texCoords;
	gs_out.bladeType = gs_in[0].bladeType;
	EmitVertex();

    gl_Position.xy = tri[1].position.xy; 
	gs_out.texCoords = tri[1].texCoords;
	gs_out.bladeType = gs_in[0].bladeType;
	EmitVertex();

    gl_Position.xy = tri[2].position.xy;
	gs_out.texCoords = tri[2].texCoords;
	gs_out.bladeType = gs_in[0].bladeType;
	EmitVertex();
	EndPrimitive();

	gl_Position.xy = tri[2].position.xy; 
	gs_out.texCoords = tri[2].texCoords;
	gs_out.bladeType = gs_in[0].bladeType;
	EmitVertex();

	gl_Position.xy = tri[1].position.xy;
	gs_out.texCoords = tri[1].texCoords;
	gs_out.bladeType = gs_in[0].bladeType;
	EmitVertex();

	gl_Position.xy = tri[3].position.xy; 
	gs_out.texCoords = tri[3].texCoords;
	gs_out.bladeType = gs_in[0].bladeType;
	EmitVertex();
	EndPrimitive();
	
	if(LODLevel > 0)
	{
		gl_Position.xy = tri[3].position.xy; 
		gs_out.texCoords = tri[3].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

    	gl_Position.xy = tri[2].position.xy;
		gs_out.texCoords = tri[2].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

    	gl_Position.xy = tri[4].position.xy; 
		gs_out.texCoords = tri[4].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();
    	EndPrimitive();

		gl_Position.xy = tri[4].position.xy; 
		gs_out.texCoords = tri[4].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

		gl_Position.xy = tri[3].position.xy;
		gs_out.texCoords = tri[3].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

		gl_Position.xy = tri[5].position.xy; 
		gs_out.texCoords = tri[5].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();
		EndPrimitive();
	}

	if(LODLevel > 1)
	{
		gl_Position.xy = tri[5].position.xy; 
		gs_out.texCoords = tri[5].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

		gl_Position.xy = tri[4].position.xy;
		gs_out.texCoords = tri[4].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

		gl_Position.xy = tri[6].position.xy; 
		gs_out.texCoords = tri[6].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();
		EndPrimitive();

		gl_Position.xy = tri[6].position.xy; 
		gs_out.texCoords = tri[6].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

		gl_Position.xy = tri[5].position.xy;
		gs_out.texCoords = tri[5].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();

		gl_Position.xy = tri[7].position.xy; 
		gs_out.texCoords = tri[7].texCoords;
		gs_out.bladeType = gs_in[0].bladeType;
		EmitVertex();
		EndPrimitive();
	}
}
#endif

#ifdef PS
uniform sampler2D grass_blade_0;
uniform sampler2D grass_blade_1;
uniform sampler2D grass_blade_2;
uniform sampler2D grass_blade_3;
uniform sampler2D grass_blade_4;
uniform sampler2D grass_blade_5;

in GeometryData
{
	flat int bladeType;
	vec2 texCoords;
} fs_in;

layout(location=0) out vec4 Color;
layout(location=1) out vec4 Normal;

void program_main_ps()
{	
	vec2 texCoords = vec2(fs_in.texCoords.x, 1.0 -  fs_in.texCoords.y);

	switch(fs_in.bladeType)
	{
		case 0:
			Color = texture2D(grass_blade_0, texCoords);
		break;

		case 1:
			Color = texture2D(grass_blade_1, texCoords);
		break;

		case 2:
			Color = texture2D(grass_blade_2, texCoords);
		break;

		case 3:
			Color = texture2D(grass_blade_3, texCoords);
		break;

		case 4:
			Color = texture2D(grass_blade_4, texCoords);
		break;

		case 5:
			Color = texture2D(grass_blade_5, texCoords);
		break;

		default:
		Color = texture2D(grass_blade_0, texCoords);
	}

	Color.rgb *= 1.2;
	Color.rgb /= Color.a;
	Color.a *= smoothstep(0, 0.2, 1.0 - texCoords.y);
	//Color.a = 0.0;
	Normal = vec4(0.5, 0.5, 1, 1.0);
}
#endif