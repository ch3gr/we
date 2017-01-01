#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;

void main()
{
	vec2 st = gl_TexCoord[0].st;

	vec4 c0 = texture2DRect(tex0, st);
    vec4 c1 = texture2DRect(tex1, st);
	//vec4 c2 = texture2DRect(tex2, st);

	//gl_FragColor = (c0+c1+c2)/3;
	gl_FragColor = c0*c1;

}