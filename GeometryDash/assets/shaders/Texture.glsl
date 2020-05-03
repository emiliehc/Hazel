// basic texture shader


#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
out float v_TilingFactor;
out float v_XPos;

void main() {
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
    v_TexIndex = a_TexIndex;
    v_TilingFactor = a_TilingFactor;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    v_XPos = gl_Position.x;
    //v_Position = a_Position;
}



#type fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;
in float v_XPos;

//uniform float u_TilingFactor;
uniform sampler2D u_Textures[32];

void main() {
    vec4 texColor = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor);
    if (texColor == vec4(1.0f, 0.0f, 1.0f, 1.0f)) {
        color = vec4(1.0f);
    }
    else if (texColor.x == 1.0f && texColor.y == 0.0f && texColor.w == 1.0f && texColor.z - 0.99f < 0.000000000000001f) {
        color = vec4(1.0f, 1.0f, 1.0f, 1.0f - abs(v_XPos));
    }
    else {
        color = texColor * v_Color;
    }
}

