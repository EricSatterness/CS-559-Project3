/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code is directly copied,
	also based upon code from the OpenGL Cookbook
*/
#version 400

layout (location = 0) out vec4 FragColor;

uniform sampler2D Tex1;

in vec3 origPosition;
in vec3 origNormal;

in vec3 color;
in vec3 position;
in vec3 normal;
flat in uint seeThrough;
in vec4 M_ADSS;

in vec3 light_position_1;
in vec3 ADS_1;
flat in uint on_1;

in vec3 light_position_2;
in vec3 ADS_2;
flat in uint on_2;

in vec3 light_position_3;
in vec3 ADS_3;
flat in uint on_3;

void main()
{
	vec4 useColor;
	if(origNormal.z == 1)
	{
		vec2 coords = vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y);
		coords.y = 1 - coords.y;
		coords.y = coords.y/3.0f;
		coords.y += 2.0f/3.0f;
		coords.x = coords.x/4;
		coords.x += .25f;
		useColor = texture( Tex1, coords);
	}
	else if(origNormal.z == -1)
	{
		vec2 coords = vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y);
		coords.y = 1 - coords.y;
		coords.y = coords.y/3.0f;
		coords.x = coords.x/4.0f;
		coords.x += .25f;
		useColor = texture( Tex1, coords);
	}
	else if(origNormal.y == 1)
	{
		vec2 coords = vec2(.5f - origNormal.y*origPosition.x, .5f + origPosition.z);
		coords.x = 1 - coords.x;		
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4.0f;
		coords.x += .25f;
		useColor = texture( Tex1, coords);
	}
	else if(origNormal.x == -1)
	{
		vec2 coords = vec2(.5f + origNormal.x*origPosition.y, .5f + origPosition.z);
		coords.x = 1 - coords.x;
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4.0f;
		useColor = texture( Tex1, coords);
	}
	else if(origNormal.x == 1)
	{
		vec2 coords = vec2(.5f + origNormal.x*origPosition.y, .5f + origPosition.z);
		coords.x = 1 - coords.x;
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4;
		coords.x += .5f;
		useColor = texture( Tex1, coords);
	}
	else if(origNormal.y == -1)
	{
		vec2 coords = vec2(.5f - origNormal.y*origPosition.x, .5f + origPosition.z);
		coords.x = 1 - coords.x;		
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4.0f;
		coords.x += .75f;
		useColor = texture( Tex1, coords);
	}
	else 
		useColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	FragColor = useColor;
}