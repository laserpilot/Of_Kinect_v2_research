#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform float xOffset;
uniform float xScale;


in vec2 texCoordVarying;

out vec4 outputColor;
 
void main()
{
    vec2 texCoordResize = texCoordVarying;
    texCoordResize.x = xOffset + texCoordResize.x/512 * xScale;
    texCoordResize.y = texCoordResize.y/424 * 1080.0;

//    texCoordResize.y =

    outputColor = texture(tex1, texCoordResize);
}