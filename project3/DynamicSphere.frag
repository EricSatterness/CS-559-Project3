/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code is directly copied,
	also based upon code from the OpenGL Cookbook
*/
#version 400

layout (location = 0) out vec4 FragColor;

uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform int hit;
uniform float time;

in mat3 normalMatrix;
in vec3 origPosition;

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

float theta, phi;
vec3 d;
vec3 norm;
vec2 coord;
vec4 ads(vec3 light_position, vec3 L_ADS, uint on, vec3 Ka, vec3 Kd, vec3 Ks, int shininess)
{

	vec3 n = normalize(normalMatrix * norm);

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

	int tmp = int(float(time)/5000);
	float t = float(int(time) - 5000*tmp)/5000;
	t*= 2;
	float c = t + coord.y;
	if(c > 2)
		c = c - 2;
	else if(c > 1)
		c  = 2 - c;
	vec4 useColor = vec4(0.0f, 1.0, .5 + .5*sin(2*4*3.141592654*(c-.5)), 1.0f);
	if(hit==1)
		useColor.x = .75;
	return useColor*vec4(ambient + diffuse + spec, 1.0f); 
}
vec3 calc(float phi, float theta)
{
	vec2 coord; 
	coord.y = theta/6.2831853;
	coord.x = (phi + 1.570796327);
	if(origPosition.x > 0)
		coord.x = 3.141592654 - coord.x;
	coord.x = coord.x/6.2831853;
	coord.x = coord.x/2;
	if(origPosition.x > 0)
		coord.x = .5 - coord.x;
	coord.x = 2.0f*coord.x;
	coord.y = 2.0f*coord.y;
	coord.y = 1.0f - coord.y;
	vec3 t = texture( Tex1, coord).xyz;
	float z = 0.2989f*t.x +  0.5870f*t.y + 0.1140f*t.z;
	return vec3(coord.x, coord.y, z);
}
void main()
{
	int shininess = int(M_ADSS.a);
	vec3 Ka = vec3(M_ADSS.x, M_ADSS.x, M_ADSS.x);
	vec3 Kd = vec3(M_ADSS.y, M_ADSS.y, M_ADSS.y);
	vec3 Ks = vec3(M_ADSS.z, M_ADSS.z, M_ADSS.z);

	float r = sqrt(origPosition.x*origPosition.x + origPosition.y*origPosition.y + origPosition.z*origPosition.z);
	theta = acos(origPosition.z/r);
	phi = atan(origPosition.y/origPosition.x);
	
	coord.y = theta/6.2831853;
	coord.x = (phi + 1.570796327);
	if(origPosition.x > 0)
		coord.x = 3.141592654 - coord.x;
	coord.x = coord.x/6.2831853;
	coord.x = coord.x/2;
	if(origPosition.x > 0)
		coord.x = .5 - coord.x;
	coord.x = 2.0f*coord.x;
	coord.y = 2.0f*coord.y;
	coord.y = 1.0f - coord.y;
	
	vec3 p1, p2, p3, p4;
	p1 = calc(phi-.03, theta - .03);
	p2 = calc(phi-.03, theta + .03); 
	p1 = calc(phi+.03, theta + .03);
	p2 = calc(phi+.03, theta - .03); 
			
	vec3 u1 = p2 - p1;
	vec3 v1 = p4 - p1;
	vec3 u2 = p2 - p4;
	vec3 v2 = p3 - p4;

	vec3 n1 = vec3(u1.y*v1.z - u1.z*v1.y, u1.z*v1.x - u1.x*v1.z, u1.x*v1.y - u1.y*v1.x);
	vec3 n2 = vec3(u2.y*v2.z - u2.z*v2.y, u2.z*v2.x - u2.x*v2.z, u2.x*v2.y - u2.y*v2.x);

	n1 = -n1;
	n2 = -n2;

	d = vec3(0.0f ,1.0f, 0.0f);
	//d = normal*((n1+n2)/2);
	//norm = (n1+n2)/2;
	//norm = normal*((n1+n2)/2);
	//norm = normal;
	norm = normal*texture( Tex2, coord).xyz;

	FragColor = ads(light_position_1, ADS_1, on_1, Ka, Kd, Ks, shininess) + ads(light_position_2, ADS_2, on_2, Ka, Kd, Ks, shininess) + ads(light_position_3, ADS_3, on_3, Ka, Kd, Ks, shininess);
	FragColor.a = 1.0f;
	//FragColor = vec4(norm, 1.0f);
}