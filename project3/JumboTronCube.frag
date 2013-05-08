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

	float sDotN = max( dot(s,n), seeThrough != 1 ? 0.0 : dot(s, -n));
	vec3 diffuse = Ld * Kd * sDotN;

	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 )
		spec = Ls * Ks * pow( max( dot(r,v), 0.0 ), shininess );

	
	//assumes flat sides facing fully in x, y or z
	vec4 useColor;
	if(abs(origNormal.z) > .9)
		useColor = texture( Tex1, vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y) );
	else if(abs(origNormal.y) > .9)
		useColor = texture( Tex1, vec2(.5f - origNormal.y*origPosition.x, .5f + origPosition.z) );
	else
		useColor = texture( Tex1, vec2(.5f + origNormal.x*origPosition.y, .5f + origPosition.z) );
	
	return useColor*vec4(ambient + diffuse + spec, 1.0f); ;
}

void main()
{
	int shininess = int(M_ADSS.a);
	vec3 Ka = vec3(M_ADSS.x, M_ADSS.x, M_ADSS.x);
	vec3 Kd = vec3(M_ADSS.y, M_ADSS.y, M_ADSS.y);
	vec3 Ks = vec3(M_ADSS.z, M_ADSS.z, M_ADSS.z);
			
	//FragColor = ads(light_position_1, ADS_1, on_1, Ka, Kd, Ks, shininess) + ads(light_position_2, ADS_2, on_2, Ka, Kd, Ks, shininess) + ads(light_position_3, ADS_3, on_3, Ka, Kd, Ks, shininess);
	FragColor = texture( Tex1, vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y) );
	FragColor.a = 1.0f;
}