#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 iResolution = vec2(0., 0.);
uniform float iTime = 0.;

float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.345, 67.89))) * 98765.1234);
}

float noise(vec2 n) {
    const vec2 d = vec2(0.0, 1.0);
    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

void main() {
    vec2 p_d = fragTexCoord;
    p_d.y -= iTime * 0.2;
    
    vec2 dest_offset = vec2(noise(p_d * vec2(3)));
    dest_offset -= vec2(.5, .5);
    dest_offset *= 2. * 0.01 * (1. - fragTexCoord.t);

    vec2 finalCoord = fragTexCoord + dest_offset;
    finalCoord.x = clamp(finalCoord.x, 0., 0.99);
    finalCoord.y = clamp(finalCoord.y, 0., 0.99);

    finalColor = texture(texture0, finalCoord);
}
