#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform sampler2D hdrTexture;

float exposure = 1.1f;
float gamma = 2.2f;
void main()
{           
    vec3 color = texture(hdrTexture, fs_in.TexCoords).rgb;

    // Reinhard色调映射
    // vec3 mapped = color / (color + 1.f);

    // 曝光色调映射
    vec3 mapped = vec3(1.0) - exp(-color * exposure);

    mapped  = pow(mapped,vec3(1/gamma));

    FragColor = vec4(mapped, 1.0);
}