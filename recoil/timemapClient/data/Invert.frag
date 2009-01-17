uniform sampler2D tex;


void main()
{	
	vec4 color = (texture2D(tex, gl_TexCoord[0].xy));
	gl_FragColor = vec4(color.a, color.a, color.a, 1.0);
}
