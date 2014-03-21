#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

in vec2 texCoordVarying;

out vec4 outputColor;
 
void main()
{
    vec2 texCoordResize = texCoordVarying;
    texCoordResize.x = 250 + texCoordResize.x/512 * 1520;
    texCoordResize.y = texCoordResize.y/424 * 1080.0;

//    texCoordResize.y =

    outputColor = texture(tex1, texCoordResize);
}