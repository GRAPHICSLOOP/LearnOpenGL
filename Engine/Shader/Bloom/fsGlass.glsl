#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform sampler2D hdrTexture;
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);


void main()
{           
    vec2 pixleSize = 1.f / textureSize(hdrTexture,0);
    vec3 result = texture(hdrTexture,fs_in.TexCoords).rgb * weight[0];

    if(horizontal)
    {   
        for(int i = 1 ; i < 5 ; i++)
        {   
            result += texture(hdrTexture,fs_in.TexCoords + vec2(pixleSize.x * i,0.f)).rgb * weight[i];
            result += texture(hdrTexture,fs_in.TexCoords - vec2(pixleSize.x * i,0.f)).rgb * weight[i];
        }
    }
    else
    {   
        for(int i = 1 ; i < 5 ; i++)
        {   
            result += texture(hdrTexture,fs_in.TexCoords + vec2(0.f,pixleSize.y * i)).rgb * weight[i];
            result += texture(hdrTexture,fs_in.TexCoords - vec2(0.f,pixleSize.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(result,1.f);
}