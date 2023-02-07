#version 460 core
#pragma optimize(off)


// uniform blocks >= 10 are reserved for internal use only
layout(std140, binding = 10) uniform RendererInput {
    ivec2 resolution;     // viewport size in pixels
    ivec2 cursor_pos;     // cursor position relative to viewport's upper-left corner
    float near_clip;      // frustum near clip distance
    float far_clip;       // frustum far clip distance
    float time;           // number of seconds since window created
    float delta_time;     // number of seconds since the last frame
    bool  depth_prepass;  // early z test
    uint  shadow_index;   // index of the shadow map render pass (one index per light source)
} rdr_in;

// default-block (loose) uniform locations >= 1000 are reserved for internal use only
struct self_t {
    mat4 transform;    // 1000, model matrix of the current entity
    uint material_id;  // 1001, current mesh's material id
    uint ext_1002;
    uint ext_1003;
    uint ext_1004;
    uint ext_1005;
    uint ext_1006;
    uint ext_1007;
};

layout(location = 1000) uniform self_t self;

////////////////////////////////////////////////////////////////////////////////

#ifdef vertex_shader

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 uv2;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 binormal;
layout(location = 6) in ivec4 bone_id;
layout(location = 7) in vec4 bone_wt;

layout(location = 100) uniform mat4 bone_transform[150];  // up to 150 bones
layout(location = 1008) uniform bool iSBone;

mat4 CalcBoneTransform() {
    mat4 T = mat4(0.0);
    for (uint i = 0; i < 4; ++i) {
        if (bone_id[i] >= 0) {
            T += (bone_transform[bone_id[i]] * bone_wt[i]);
        }
    }
    // if (length(T[0]) == 0) { return mat4(1.0); }
    return T;
}

void main() {
    
    mat4 BT = iSBone ? CalcBoneTransform() : mat4(1.0);
    gl_Position = self.transform * BT * vec4(position, 1.0);  // keep in world space
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef geometry_shader

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(location = 0) out vec4 world_position;
layout(location = 250) uniform mat4 light_transform[6];

void main() {
    // render target is a cubemap texture, so we render 6 times, once for each face
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face;
        uint index = uint(face);

        for (uint i = 0; i < 3; ++i) {
            world_position = gl_in[i].gl_Position;
            gl_Position = light_transform[index] * world_position;  // project into light frustum
            EmitVertex();
        }

        EndPrimitive();
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef fragment_shader

layout(location = 0) in vec4 world_position;

layout(std140, binding = 1) uniform PL {
    vec4  color;
    vec4  position;
    float intensity;
    float linear;
    float quadratic;
    float range;
} pl;


void main() {
    gl_FragDepth = distance(world_position.xyz, pl.position.xyz) / rdr_in.far_clip;
}

#endif
