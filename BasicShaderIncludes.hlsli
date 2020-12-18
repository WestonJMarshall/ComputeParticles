#ifndef __BASIC_SHADER_INCLUDES__
#define __BASIC_SHADER_INCLUDES__

static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; 

// 6 zeros after decimal
// Handy to have this as a constant
static const float PI = 3.14159265359f;

struct VertexShaderInput
{
	float3 position		: POSITION; 
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float2 uv           : TEXCOORD;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;	
	float4 worldPos     : POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
};

struct NormalVTP
{
	float4 position		: SV_POSITION;	
	float4 worldPos     : POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float2 uv           : TEXCOORD;
};


/*--------
 LIGHTING
--------*/

#define MAX_LIGHT_COUNT 16

struct Light
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float4 Direction;
	float4 Position;
	float4 Valid;
};

float SpecularPhong(float3 normal, float3 lightDirection, float3 visionVector, float exponent)
{
	return pow(saturate(dot(reflect(lightDirection, normal), visionVector)), exponent);
}

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}


// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// specular - Specular color (including light color)
// metalness - surface metalness amount
//
// Metals should have an albedo of (0,0,0)...mostly
// See slide 65: http://blog.selfshadow.com/publications/s2014-shading-course/hoffman/s2014_pbs_physics_math_slides.pdf
float3 DiffuseEnergyConserve(float diffuse, float3 specular, float metalness)
{
	return diffuse * ((1 - saturate(specular)) * (1 - metalness));
}


// GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float SpecDistribution(float3 n, float3 h, float roughness)
{
	// Pre-calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, MIN_ROUGHNESS);
	// Applied after remap!
	// ((n dot h)^2 * (a^2 - 1) + 1)
	float denomToSquare = NdotH2 * (a2 - 1) + 1;
	// Can go to zero if roughness is 0 and NdotH is 1
	// Final value
	return a2 / (PI * denomToSquare * denomToSquare);
}


// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n (full specular color)
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 Fresnel(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
	float VdotH = saturate(dot(v, h));
	// Final value
	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}


// Geometric Shadowing - Schlick-GGX (based on Schlick-Beckmann)
// - k is remapped to a / 2, roughness remapped to (r+1)/2
//
// n - Normal
// v - View vector
//
// G(l,v,h)
float GeometricShadowing(float3 n, float3 v, float3 h, float roughness)
{
	// End result of remapping:
	float k = pow(roughness + 1, 2) / 8.0f; float NdotV = saturate(dot(n, v));
	// Final value
	return NdotV / (NdotV * (1 - k) + k);
}


// Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - part of the denominator are canceled out by numerator (see below)
//
// D() - Spec Dist - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float metalness, float3 specColor)
{
	// Other vectors
	float3 h = normalize(v + l);
	// Grab various functions
	float D = SpecDistribution(n, h, roughness);
	float3 F = Fresnel(v, h, specColor);
	float G = GeometricShadowing(n, v, h, roughness) * GeometricShadowing(n, l, h, roughness);
	// Final formula
	// Denominator dot products partially canceled by G()!
	// See page 16: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
	return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));
}

float4 StandardPBRLighting(NormalVTP input, Light directionalLights[MAX_LIGHT_COUNT], Light pointLights[MAX_LIGHT_COUNT],
	float4 colorTint, float3 cameraPosition, Texture2D diffuseTexture,
	Texture2D roughnessTexture, Texture2D metalnessTexture, SamplerState textureSampler)
{
	float3 pixelAlbedo = pow(diffuseTexture.Sample(textureSampler, input.uv).rgb, 2.2f);
	float pixelRoughness = roughnessTexture.Sample(textureSampler, input.uv).r;
	float pixelMetalness = metalnessTexture.Sample(textureSampler, input.uv).r;
	float3 pixelSpecular = lerp(F0_NON_METAL.rrr, pixelAlbedo.rgb, pixelMetalness);

	float3 newNormals = normalize(input.normal);

	float3 visionVector = normalize(cameraPosition - input.worldPos.rgb);

	int i = 0;
	float3 lightDirection = float3(0, 0, 0);
	float lightAmount = 0;
	float3 finalColor = float3(0, 0, 0);

	for (i = 0; i < MAX_LIGHT_COUNT && directionalLights[i].Valid.r > 0.5f; i++)
	{
		lightDirection = normalize(-directionalLights[i].Direction.rgb);
		float3 spec = MicrofacetBRDF(newNormals, lightDirection, visionVector, pixelRoughness, pixelMetalness, pixelSpecular);

		float diff = DiffusePBR(newNormals, lightDirection);
		float3 balancedDiff = DiffuseEnergyConserve(diff, spec, pixelMetalness);

		lightAmount = dot(newNormals, lightDirection);
		lightAmount = saturate(lightAmount);

		float3 total = (balancedDiff * pixelAlbedo + spec) * lightAmount * directionalLights[i].DiffuseColor.rgb;

		finalColor += total;
	}

	for (i = 0; i < MAX_LIGHT_COUNT && pointLights[i].Valid.r > 0.5f; i++)
	{
		lightDirection = normalize(pointLights[i].Position.rgb - input.worldPos.rgb);
		float3 spec = MicrofacetBRDF(newNormals, lightDirection, visionVector, pixelRoughness, pixelMetalness, pixelSpecular);

		float diff = DiffusePBR(newNormals, lightDirection);
		float3 balancedDiff = DiffuseEnergyConserve(diff, spec, pixelMetalness);

		lightAmount = dot(newNormals, lightDirection);
		lightAmount = saturate(lightAmount);

		float3 total = (balancedDiff * pixelAlbedo + spec) * lightAmount * pointLights[i].DiffuseColor.rgb;

		finalColor += total;
	}

	finalColor = finalColor * colorTint.rgb;
	return float4(pow(finalColor, 1.0f / 2.2f), 1);
}

float4 NormalPBRLighting(NormalVTP input, Light directionalLights[MAX_LIGHT_COUNT], Light pointLights[MAX_LIGHT_COUNT],
	float4 colorTint, float3 cameraPosition, Texture2D diffuseTexture, Texture2D normalTexture, 
	Texture2D roughnessTexture, Texture2D metalnessTexture, SamplerState textureSampler)
{
	float3 pixelNormals = normalTexture.Sample(textureSampler, input.uv).rgb * 2 - 1;
	float3 pixelAlbedo = pow(diffuseTexture.Sample(textureSampler, input.uv).rgb, 2.2f);
	float pixelRoughness = roughnessTexture.Sample(textureSampler, input.uv).r;
	float pixelMetalness = metalnessTexture.Sample(textureSampler, input.uv).r;
	float3 pixelSpecular = lerp(F0_NON_METAL.rrr, pixelAlbedo.rgb, pixelMetalness);

	float3 N = input.normal;
	float3 T = input.tangent;
	T = normalize(T - N * dot(T, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	float3 newNormals = mul(pixelNormals, TBN);
	newNormals = normalize(newNormals);

	float3 visionVector = normalize(cameraPosition - input.worldPos.rgb);

	int i = 0;
	float3 lightDirection = float3(0, 0, 0);
	float lightAmount = 0;
	float3 finalColor = float3(0, 0, 0);

	for (i = 0; i < MAX_LIGHT_COUNT && directionalLights[i].Valid.r > 0.5f; i++)
	{
		lightDirection = normalize(-directionalLights[i].Direction.rgb);
		float3 spec = MicrofacetBRDF(newNormals, lightDirection, visionVector, pixelRoughness, pixelMetalness, pixelSpecular);

		float diff = DiffusePBR(newNormals, lightDirection);
		float3 balancedDiff = DiffuseEnergyConserve(diff, spec, pixelMetalness);

		lightAmount = dot(newNormals, lightDirection);
		lightAmount = saturate(lightAmount);

		float3 total = (balancedDiff * pixelAlbedo + spec) * lightAmount * directionalLights[i].DiffuseColor.rgb;

		finalColor += total;
	}

	for (i = 0; i < MAX_LIGHT_COUNT && pointLights[i].Valid.r > 0.5f; i++)
	{
		lightDirection = normalize(pointLights[i].Position.rgb - input.worldPos.rgb);
		float3 spec = MicrofacetBRDF(newNormals, lightDirection, visionVector, pixelRoughness, pixelMetalness, pixelSpecular);

		float diff = DiffusePBR(newNormals, lightDirection);
		float3 balancedDiff = DiffuseEnergyConserve(diff, spec, pixelMetalness);

		lightAmount = dot(newNormals, lightDirection);
		lightAmount = saturate(lightAmount);

		float3 total = (balancedDiff * pixelAlbedo + spec) * lightAmount * pointLights[i].DiffuseColor.rgb;

		finalColor += total;
	}

	finalColor = finalColor * colorTint.rgb;
	return float4(pow(finalColor, 1.0f / 2.2f), 1);
}

float4 CellLighting(NormalVTP input, Light directionalLights[MAX_LIGHT_COUNT], Light pointLights[MAX_LIGHT_COUNT],
	float4 colorTint, float3 cameraPosition, Texture2D diffuseTexture, Texture2D normalTexture,
	Texture2D roughnessTexture, Texture2D metalnessTexture, SamplerState textureSampler)
{
	float3 pixelAlbedo = pow(diffuseTexture.Sample(textureSampler, input.uv).rgb, 2.2f);
	float pixelRoughness = roughnessTexture.Sample(textureSampler, input.uv).r;
	float pixelMetalness = metalnessTexture.Sample(textureSampler, input.uv).r;
	float3 pixelSpecular = lerp(F0_NON_METAL.rrr, pixelAlbedo.rgb, pixelMetalness);

	float3 visionVector = normalize(cameraPosition - input.worldPos.rgb);

	int i = 0;
	float3 lightDirection = float3(0, 0, 0);
	float lightAmount = 0;
	float3 finalColor = float3(0, 0, 0);

	for (i = 0; i < MAX_LIGHT_COUNT && directionalLights[i].Valid.r > 0.5f; i++)
	{
		lightDirection = normalize(-directionalLights[i].Direction.rgb);
		float3 spec = MicrofacetBRDF(input.normal, lightDirection, visionVector, pixelRoughness, pixelMetalness, pixelSpecular);

		float diff = DiffusePBR(input.normal, lightDirection);
		float3 balancedDiff = DiffuseEnergyConserve(diff, spec, pixelMetalness);

		lightAmount = dot(input.normal, lightDirection);
		lightAmount = saturate(lightAmount);

		float3 total = (balancedDiff + spec) * lightAmount * directionalLights[i].DiffuseColor.rgb;

		finalColor += total;
	}

	for (i = 0; i < MAX_LIGHT_COUNT && pointLights[i].Valid.r > 0.5f; i++)
	{
		lightDirection = normalize(pointLights[i].Position.rgb - input.worldPos.rgb);
		float3 spec = MicrofacetBRDF(input.normal, lightDirection, visionVector, pixelRoughness, pixelMetalness, pixelSpecular);

		float diff = DiffusePBR(input.normal, lightDirection);
		float3 balancedDiff = DiffuseEnergyConserve(diff, spec, pixelMetalness);

		lightAmount = dot(input.normal, lightDirection);
		lightAmount = saturate(lightAmount);

		float3 total = (balancedDiff + spec) * lightAmount * pointLights[i].DiffuseColor.rgb;

		finalColor += total;
	}

	finalColor = finalColor * colorTint.rgb;
	return float4(pow(finalColor, 1.0f / 2.2f), 1);
}

#endif