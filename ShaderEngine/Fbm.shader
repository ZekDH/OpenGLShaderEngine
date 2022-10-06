#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main(){
    gl_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 iResolution;
uniform float iTime;

uniform vec3 baseColour;
uniform vec3 streakColour;
uniform vec3 ColourOne;
uniform vec3 ColourTwo;
uniform vec3 ColourThree;
uniform vec3 ColourFour;


float noise( in vec2 p )
{
    return sin(p.x)*sin(p.y);
}

const mat2 mtx = mat2( 0.80,  0.60, -0.60,  0.80 );

float fbm( vec2 p )
{
    float f = 0.0;
    f += 0.500000*noise( p ); p = mtx*p*2.02;
    f += 0.250000*noise( p ); p = mtx*p*2.03;
    f += 0.125000*noise( p ); p = mtx*p*2.01;
    f += 0.062500*noise( p ); p = mtx*p*2.04;
    f += 0.031250*noise( p ); p = mtx*p*2.01;
    f += 0.015625*noise( p );
    return f/0.96875;
}

float Distance(vec2 p1, vec2 p2)
{
    return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

float pattern(in vec2 p, in float t, in vec2 uv, out vec2 q, out vec2 r, out vec2 g)
{
    q = vec2(fbm(p), fbm(p + vec2(10, 1.3)));
    
    float s = dot(uv.x + 0.5, uv.y + 0.5);
    r = vec2(fbm(p + 4.0 * q + vec2(t) + vec2(1.7, 9.2)), fbm(p + 4.0 * q + vec2(t) + vec2(8.3, 2.8)));
    g = vec2(fbm(p + 2.0 * r + vec2(t * 20.0) + vec2(2, 6)), fbm(p + 2.0 * r + vec2(t * 10.0) + vec2(5, 3)));
    return fbm(p + 5.5 * g + vec2(-t * 7.0));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
    // noise
    vec2 q, r, g;
    float noise = pattern(fragCoord * vec2(.002), iTime * 0.003, uv, q, r, g);
    //vec3 colTime = 0.5 + 0.5*cos((iTime*0.5)+uv.xyx*4.0+vec3(0,2,4));
    //colTime += noise * 0.6;
    
    
    // base color based on main noise
    vec3 col = mix(baseColour, streakColour, smoothstep(0.0, 1.0, noise));
    
    // other lower-octave colors and mixes
    col = mix(col, ColourOne, dot(q, q) * 1.0);
    col = mix(col, ColourTwo, 0.2*g.y*g.y);
    col = mix(col, ColourThree, smoothstep(0.0, .6, 0.6*r.g*r.g));
    col = mix(col, ColourFour, 0.1*g.x);
    
    // some dark outlines/contrast and different steps
    col = mix(col, vec3(0), smoothstep(0.3, 0.5, noise) * smoothstep(0.5, 0.3, noise));
    col = mix(col, vec3(0), smoothstep(0.7, 0.8, noise) * smoothstep(0.8, 0.7, noise));
    
    // contrast
    col += noise*0.2;
    
    // vignette
    col *= 0.70 + 0.65 * sqrt(70.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y));
    
    // Output to screen
    fragColor = vec4(col,1.0);
}

void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
    