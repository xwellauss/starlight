#version 300 es

precision mediump float;

in vec3 f_pos;
in vec4 f_color;
in vec2 f_tex_coord;
in vec3 f_normal;

out vec4 FragColor;

uniform vec3 cam_pos;
uniform vec3 model_pos;

// Copied from Learn OpenGL

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

struct Material
{
	sampler2D base_color_map; // albedo
	sampler2D metallic_roughness_map;
	sampler2D normal_map;

	vec4 base_color;
    float roughness_factor;
    float metallic_factor;

	int has_albedo_map;
    int has_metallic_roughness_map;
	int has_normal_map;
};

struct Light
{
	vec3 position;
	vec3 color;
	float intensity;
};

uniform Material material;
uniform Light light;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

vec3 GetNormalFromMap(sampler2D normal_map, vec2 tex_coord, vec3 world_normal, vec3 world_pos)
{
    vec3 tangent_normal = texture(normal_map, tex_coord).rgb * 2.0 - 1.0;

    vec3 Q1  = dFdx(world_pos);
    vec3 Q2  = dFdy(world_pos);
    vec2 st1 = dFdx(tex_coord);
    vec2 st2 = dFdy(tex_coord);

    vec3 N   = normalize(world_normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangent_normal);
}

void main()
{
	vec3 albedo = material.base_color.rgb;
	if(material.has_albedo_map == 1)
	{
		vec3 sampled_albedo = texture(material.base_color_map, f_tex_coord).rgb;
		albedo *= pow(sampled_albedo, vec3(2.2));
	}

	float metallic = material.metallic_factor;
	float roughness = material.roughness_factor;
	float ao = 1.0;

	if(material.has_metallic_roughness_map == 1)
	{
		vec3 sampled_mr = texture(material.metallic_roughness_map, f_tex_coord).rgb;


		float r_fac = (material.roughness_factor == 0.0) ? 1.0 : material.roughness_factor;
		float m_fac = (material.metallic_factor == 0.0) ? 1.0 : material.metallic_factor;
		roughness *= sampled_mr.g;
		metallic *= sampled_mr.b;

		ao = sampled_mr.r;
	}

	roughness = max(roughness, 0.05);

	vec3 N;
	if(material.has_normal_map == 1)
	{
		N = GetNormalFromMap(material.normal_map, f_tex_coord, f_normal, f_pos);
	}
	else
	{
		N = f_normal;
	}


	vec3 V = normalize(cam_pos - f_pos);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// Reflectance Equation
    vec3 Lo = vec3(0.0);

    // Calculate Per-Light Radiance (Including light intensity before tone mapping)
    vec3 L = normalize(light.position - f_pos);
    vec3 H = normalize(V + L);
    float distance = length(light.position - f_pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * light.intensity * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);
    
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;


	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));



	FragColor = vec4(color, 1.0);
}
