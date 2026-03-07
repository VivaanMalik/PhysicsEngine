#version 330 core
out vec4 FragColor;
uniform vec4 objectColor;

void main() {
    // float glow = 1.0;

    // // // Only apply circular math to Particles
    if (gl_PointCoord != vec2(0.0)) {
        float dist = distance(gl_PointCoord, vec2(0.5));
        if (dist > 0.5) discard;
    }

    FragColor = objectColor;
}