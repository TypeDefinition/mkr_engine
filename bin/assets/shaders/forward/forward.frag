// IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin.

#version 460 core

/* An alternative to the layout qualifier is to use glBindFragDataLocation(GLuint program, GLuint colorNumber, const char * name) or
   glBindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const char *name) in the C++ code.
   By using glBindFragDataLocation(GLuint program, GLuint colorNumber, const char * name), it is not necessary to use the layout qualifier and vice-versa.
   However, if both are used and are in conflict, the layout qualifier in the vertex shader has priority. */
layout (location = 0) out vec4 out_colour;
layout (location = 1) out vec3 out_position;
layout (location = 2) out vec3 out_normal;

// Inputs
in VS_OUT {
    vec2 tex_coord;
    vec3 position;// Vertex position in camera space.
    vec3 normal;// Vertex normal in camera space.
    mat3 tbn_matrix;// Converts from tangent space to camera space.
} vs_out;

#include <light.frag>
#include <shadow.frag>
#include <parallax.frag>

// Transform
uniform mat4 u_view_matrix;
uniform mat4 u_inv_view_matrix;

// Material
uniform vec4 u_diffuse_colour;
uniform vec4 u_specular_colour;
uniform float u_displacement_scale;

// Textures
uniform bool u_has_texture_diffuse;
uniform bool u_has_texture_normal;
uniform bool u_has_texture_specular;
uniform bool u_has_texture_displacement;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_displacement;

vec2 get_tex_coord() {
    return u_has_texture_displacement ? parallax(u_texture_displacement, vs_out.tex_coord, u_displacement_scale, vs_out.position, vs_out.tbn_matrix, vs_out.normal) : vs_out.tex_coord;
}

vec3 get_normal(const in vec2 _tex_coord) {
    if (u_has_texture_normal) {
        const vec3 normal = 2.0f * texture(u_texture_normal, _tex_coord).rgb - vec3(1.0f, 1.0f, 1.0f); // Convert into the [-1, 1] range.
        return vs_out.tbn_matrix * normal; // Convert the normal from tangent space to camera space.
    }
    return vs_out.normal;
}

void get_diffuse(const in vec2 _tex_coord, inout vec3 _colour, inout float _alpha) {
    _colour = u_diffuse_colour.rgb;
    _alpha = u_diffuse_colour.a;
    if (u_has_texture_diffuse) {
        vec4 tex_val = texture(u_texture_diffuse, _tex_coord);
        _colour *= tex_val.rgb;
        _alpha *= tex_val.a;
    }
}

void get_specular(const in vec2 _tex_coord, inout vec3 _colour, inout float _gloss) {
    _colour = u_specular_colour.rgb;
    _gloss = u_specular_colour.a;
    if (u_has_texture_specular) {
        vec4 tex_val = texture(u_texture_specular, _tex_coord);
        _colour *= tex_val.rgb;
        _gloss *= tex_val.a;
    }
}

void main() {
    const vec2 tex_coord = get_tex_coord();
    const vec3 normal = get_normal(tex_coord);

    bool cast_shadow[max_lights];
    get_cast_shadow(vs_out.position, normal,
                    u_inv_view_matrix,
                    u_lights, u_num_lights,
                    cast_shadow);

    vec3 diffuse; float alpha;
    get_diffuse(tex_coord, diffuse, alpha);
    vec3 specular; float gloss;
    get_specular(tex_coord, specular, gloss);

    const vec3 ambient = diffuse * u_ambient_light.rgb;
    diffuse *= get_light_diffuse(vs_out.position, normal, cast_shadow);
    specular *= get_light_specular(vs_out.position, normal, gloss, cast_shadow);

    const vec3 colour = ambient + diffuse + specular;

    out_colour = vec4(colour, alpha);
    out_position = vs_out.position;
    out_normal = vs_out.normal;
}