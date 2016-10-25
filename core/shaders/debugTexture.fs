#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_tex;
varying vec2 uv;

void main() {
    gl_FragColor = vec4(texture2D(u_tex, uv).xyz, 0.8);
}

