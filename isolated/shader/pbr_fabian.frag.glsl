#version 410

const float PI = 3.14159265358979323846;

in vec3 Position;
in vec3 Normal;

uniform struct LightInfo {
    vec4 Position;  // Light position in cam. coords.
    vec3 L;         // Intensity
} Light[3];

uniform struct MaterialInfo {
    float Rough;    // Roughness
    float Alpha;    // Aplha value for NDF-Function (Roughness to the power of 2 
                    // from Disneys suggestions for what alpha should be)
    bool Metal;     // Metallic (true) or dielectric (false)
    vec3 Color;     // Diffuse color for dielectrics, f0 for metallic
} Material;

layout( location = 0 ) out vec4 FragColor;

float ggxDistribution( float nDotH ) {
    float alphaSquare = Material.Alpha * Material.Alpha; 
    float denom = (nDotH * nDotH) * (alphaSquare - 1) + 1;
    denom = PI * denom * denom;
    return alphaSquare / denom;
}

float GeometrySchlickGGX(float nDotX, float k)
{
    float denom = nDotX * (1.0 - k) + k;
    return 1.0 / denom;
}

float geomSmith( float nDotV, float nDotL ) {
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    return GeometrySchlickGGX(nDotV, k) * GeometrySchlickGGX(nDotL, k);
}

vec3 schlickFresnel( float hDotV ) {
    vec3 f0 = vec3(0.04);
    f0 = mix(f0, Material.Color, Material.Metal);
    return (f0 + (1.0 - f0) * pow(1.0 - hDotV, 5.0));
}

vec3 calculateLightAttenuation( vec3 l , vec3 lightI ) {
    float dist = length(l);
    return lightI /= (dist * dist);
}

vec3 microfacetModel( int lightIndex, vec3 position, vec3 n ) {  
    vec3 diffuseBrdf = vec3(0.0);  // Metallic
    vec3 l = vec3(0.0); 
    vec3 lightIntensity = Light[lightIndex].L;

    if( !Material.Metal ) {
        diffuseBrdf = Material.Color;
    }

    if( Light[lightIndex].Position.w == 0.0 ) // Directional light
    { 
        l = normalize(Light[lightIndex].Position.xyz);
    } 
    else                                    // Positional light, need for light Attenuation
    {                             
        l = Light[lightIndex].Position.xyz - position;
        lightIntensity = calculateLightAttenuation(l, lightIntensity); 
        l = normalize(l);
    }

    vec3 v = normalize( -position );
    vec3 h = normalize( v + l );
    float nDotH = dot( n, h );
    float hDotV = dot( h, v );
    float nDotL = max( dot( n, l ), 0.0 );
    float nDotV = dot( n, v );
  
    vec3 F = schlickFresnel(hDotV);         
    
    vec3 kD = mix((vec3(1.0) - F), vec3(0.0), Material.Metal);

    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * F * geomSmith(nDotV, nDotL);

    return (kD * diffuseBrdf /  PI  + specBrdf) * lightIntensity * nDotL;
}

void main() {
    vec3 sum = vec3(0);
    vec3 n = normalize(Normal);

    for( int i = 0; i < 3; i++ ) {
        sum += microfacetModel(i, Position, n);
    }

    // Gamma 
    sum = pow( sum, vec3(1.0/2.2) );

    FragColor = vec4(sum, 1);
}
