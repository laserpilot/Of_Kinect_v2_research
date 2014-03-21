#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform float xOffset;
uniform float xScale;
uniform int useColor;

in vec2 texCoordVarying;

out vec4 outputColor;
 
void main()
{
    vec2 texCoordResize = texCoordVarying;
    texCoordResize.x = xOffset + texCoordResize.x/512 * xScale;
    texCoordResize.y = texCoordResize.y/424 * 1080.0;

    if (useColor == 1) outputColor = texture(tex1, texCoordResize);
    else outputColor = texture(tex0, texCoordVarying);
}