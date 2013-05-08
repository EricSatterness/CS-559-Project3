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


vec4 ads(vec3 light_position, vec3 L_ADS, uint on, vec3 Ka, vec3 Kd, vec3 Ks, int shininess)
{

	vec3 n = normal;
	if (!gl_FrontFacing)
		n = -n;
	vec3 s = normalize(vec3(light_position - position));
	vec3 v = normalize(-position.xyz);
	vec3 r = reflect(-s, n);

	vec3 La = vec3(L_ADS.x, L_ADS.x, L_ADS.x);
	vec3 Ld = vec3(L_ADS.y, L_ADS.y, L_ADS.y);
	vec3 Ls = vec3(L_ADS.z, L_ADS.z, L_ADS.z);

	vec3 ambient = La * Ka;

	if(on != 1)
		return vec4(0.0, 0.0, 0.0, 1.0f);

	float sDotN = max( dot(s,n), 0.0);
	vec3 diffuse = Ld * Kd * sDotN;

	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 )
		spec = Ls * Ks * pow( max( dot(r,v), 0.0 ), shininess );

	
	vec4 useColor;
	if(origNormal.z > .9)
	{
		vec2 coords = vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y);
		coords.y = 1 - coords.y;
		coords.y = coords.y/3.0f;
		coords.y += 2.0f/3.0f;
		coords.x = coords.x/4;
		coords.x += .25f;
		useColor = texture( Tex1, coords);
		//useColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if(origNormal.z < -.9)
	{
		vec2 coords = vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y);
		coords.y = 1 - coords.y;
		coords.y = coords.y/3.0f;
		coords.x = coords.x/4.0f;
		coords.x += .25f;
		useColor = texture( Tex1, coords);
		//useColor = vec4(1.0f, 0.0f, .5f, 1.0f);
	}
	else if(origNormal.y > .9)
	{
		vec2 coords = vec2(.5f - origNormal.y*origPosition.x, .5f + origPosition.z);
		coords.x = 1 - coords.x;		
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4.0f;
		coords.x += .25f;
		useColor = texture( Tex1, coords);
		//useColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	}
	else if(origNormal.x < -.9)
	{
		vec2 coords = vec2(.5f + origNormal.x*origPosition.y, .5f + origPosition.z);
		coords.x = 1 - coords.x;
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4.0f;
		useColor = texture( Tex1, coords);
		//useColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if(origNormal.x > .9)
	{
		vec2 coords = vec2(.5f + origNormal.x*origPosition.y, .5f + origPosition.z);
		coords.x = 1 - coords.x;
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4;
		coords.x += .5f;
		useColor = texture( Tex1, coords);
		//useColor = vec4(.5f, 0.0f, 1.0f, 1.0f);
	}
	else if(origNormal.y < -.9)
	{
		vec2 coords = vec2(.5f - origNormal.y*origPosition.x, .5f + origPosition.z);
		coords.x = 1 - coords.x;		
		coords.y = coords.y/3.0f;
		coords.y += 1.0f/3.0f;
		coords.x = coords.x/4.0f;
		coords.x += .75f;
		useColor = texture( Tex1, coords);
		//useColor = vec4(0.0f, 1.0f, .5f, 1.0f);
	}
	else 
		useColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);


	if(seeThrough == 1)
		return useColor*vec4(ambient + diffuse + spec, 1.0f); 
	else
		return useColor/(on_1 + on_2 + on_3);
}

void main()
{
	int shininess = int(M_ADSS.a);
	vec3 Ka = vec3(M_ADSS.x, M_ADSS.x, M_ADSS.x);
	vec3 Kd = vec3(M_ADSS.y, M_ADSS.y, M_ADSS.y);
	vec3 Ks = vec3(M_ADSS.z, M_ADSS.z, M_ADSS.z);
			
	FragColor = ads(light_position_1, ADS_1, on_1, Ka, Kd, Ks, shininess) + ads(light_position_2, ADS_2, on_2, Ka, Kd, Ks, shininess) + ads(light_position_3, ADS_3, on_3, Ka, Kd, Ks, shininess);
	FragColor.a = 1.0f;
}
