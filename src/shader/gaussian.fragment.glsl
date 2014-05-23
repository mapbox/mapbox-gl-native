uniform sampler2D u_image;

varying vec2 v_coords[5];

void main()
{
    vec4 sum = vec4(0.0);
    sum += texture2D(u_image, v_coords[0]) * 0.204164;
    sum += texture2D(u_image, v_coords[1]) * 0.304005;
    sum += texture2D(u_image, v_coords[2]) * 0.304005;
    sum += texture2D(u_image, v_coords[3]) * 0.093913;
    sum += texture2D(u_image, v_coords[4]) * 0.093913;
    gl_FragColor = sum;
}