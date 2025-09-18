#version 330 core

in vec2 fragTexCoords;

out vec4 fragColor;

void main() {
	/// @@TODO: Use tex coords as color
	float s = fragTexCoords[0];
	float t = fragTexCoords[1];
	fragColor = vec4(s, t, 0.0, 1.0);
}
