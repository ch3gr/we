#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;
uniform float thress;

void main()
{
	vec2 st = gl_TexCoord[0].st;

	vec4 cam = texture2DRect(tex0, st);
    vec4 bg = texture2DRect(tex1, st);
	vec4 faceMask = texture2DRect(tex2, st);
	

	// luminance rgb weights
	const vec3 W = vec3(0.2125, 0.7154, 0.0721);
	float camL = dot( vec3(cam), W );
	float bgL = dot( vec3(bg), W );
	float outL = abs(camL-bgL);
	outL += faceMask.r;


	if( outL > thress )
		outL = 255;
	else
		outL = 0;

	
	// black out 2 pixel border for better controus
	if( st.s<2 || st.s>638 || st.t<2 || st.t>478 )
		outL = 0;

	vec4 cOut = vec4(vec3(outL),255);

	
	gl_FragColor = cOut;


	

}