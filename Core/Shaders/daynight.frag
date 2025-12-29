uniform sampler2D texture;
uniform float time;
uniform vec3 nightColor;
uniform vec3 sunriseColor;
uniform vec3 dayColor;
uniform vec3 sunsetColor;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    vec3 tint = vec3(1.0, 1.0, 1.0);

    if (time >= 5.0 && time < 6.0) {
        float t = (time - 5.0) / 1.0;
        tint = mix(nightColor, sunriseColor, t);
    }
    else if (time >= 6.0 && time < 7.0) {
        float t = (time - 6.0) / 1.0;
        tint = mix(sunriseColor, dayColor, t);
    }
    else if (time >= 7.0 && time < 17.0) {
        tint = dayColor;
    }
    else if (time >= 17.0 && time < 19.0) {
        float t = (time - 17.0) / 2.0;
        tint = mix(dayColor, sunsetColor, t);
    }
    else if (time >= 19.0 && time < 21.0) {
        float t = (time - 19.0) / 2.0;
        tint = mix(sunsetColor, nightColor, t);
    }
    else {
        tint = nightColor;
    }

    gl_FragColor = vec4(pixel.rgb * tint, pixel.a);
}
