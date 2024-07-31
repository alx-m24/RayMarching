#version 420 core

in vec3 pixelPos;
out vec4 fragColor;

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;

    vec3 color;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 color;
};

struct Sphere {
	float radius;
	vec3 center;
	vec3 color;
	float reflection;
};

struct Cube {
	vec3 halfSize;
	vec3 color;
	mat4 inverseTransormation;
	float reflection;
	float rounding;
};

struct Capsule {
	vec3 pos1;
	vec3 pos2;
	vec3 color;
	mat4 inverseTransormation;
	float reflection;
	float radius;
};

// Types: 
#define LIGHT 0
#define SPHERE 1
#define CUBE 2
#define CAPSULE 3

struct Object {
	int type;
	int idx;
};

struct Intersect {
	float dist;
	Object obj;
};

#define MAX_DIST 50.0
#define MAX_SHADOW_DIST 25.0
#define eplison 0.01
#define NORMAL_INCREMENT 0.01

#define LIGHT_NUM 2
#define SPHERE_NUM 2
#define CUBE_NUM 2
#define CAPSULE_NUM 1

// Objects
uniform Sphere spheres[SPHERE_NUM];
uniform Cube cubes[CUBE_NUM];
uniform Capsule capsules[CAPSULE_NUM];
uniform PointLight pointLights[LIGHT_NUM];
uniform DirLight dirLight;
// Camera
uniform vec2 iResolution;
uniform vec3 position;
uniform vec3 inDir;
uniform mat3 viewMatrix;

// Normal Increments
vec3 dx = {NORMAL_INCREMENT, 0, 0};
vec3 dy = {0, NORMAL_INCREMENT, 0};
vec3 dz = {0, 0, NORMAL_INCREMENT};

float sphereSDF(vec3 pos, Sphere sphere);
float cubeSDF(vec3 pos, Cube cube);
float capsuleSDF( vec3 pos, Capsule capsule);
float shadow(vec3 origin, vec3 lightPos, vec3 dir, Object obj);
float shadow(vec3 origin, vec3 dir, Object obj);
vec3 getBend(vec3 p, float k);
vec3 getSphereNormal(vec3 pos, Sphere sphere);
vec3 getCubeNormal(vec3 pos, Cube cube);
vec3 getColor(Intersect intersect, vec3 pos, vec3 dir);
vec3 rayMarch(vec3 pos, vec3 direction);
vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color, Object obj);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, Object obj);
vec3 calculateLight(vec3 pos, vec3 normal, vec3 inColor, Object obj);
Intersect sceneDist(vec3 pos, vec3 direction);
Intersect sceneDist(vec3 pos, vec3 direction, int type, int idx);

void main() {
	vec2 aspectRatio = vec2(iResolution.x / iResolution.y, 1.0) * 0.5;
	vec2 uv = 2.0 * gl_FragCoord.xy / iResolution - 1.0;
	uv *= aspectRatio;
    vec3 rayDirection = normalize(vec3(uv, -1.0)) * viewMatrix;

	vec3 color = rayMarch(position, rayDirection);

	fragColor = vec4(color, 1.0f);
}

float shadow(vec3 origin, vec3 lightPos, vec3 dir, Object obj) {
	float lightDist = distance(lightPos, origin);
	Intersect intersect = {lightDist, {0, LIGHT}};
	vec3 pos = origin;
	
	while (distance(pos, origin) < lightDist) {
		intersect = sceneDist(pos, dir, obj.type, obj.idx);

		if (intersect.dist < eplison) break;

		pos += dir * intersect.dist;
	}

	if (intersect.obj.type == LIGHT) return 1.0;
	return 0.0;
}

float shadow(vec3 origin, vec3 dir, Object obj) {
	Intersect intersect = {MAX_SHADOW_DIST, {0, LIGHT}};
	vec3 pos = origin;
	
	while (distance(pos, origin) < MAX_SHADOW_DIST) {
		intersect = sceneDist(pos, dir, obj.type, obj.idx);

		if (intersect.dist < eplison) {
			if (intersect.obj.type != LIGHT) return 0.0;
			else break;
		}

		pos += dir * intersect.dist;
	}

	return 1.0;
}

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color, vec3 fragPos, Object obj) {
    light.ambient *= light.color;
    light.diffuse *= light.color;
    light.specular *= light.color;

    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir - viewDir);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 10);

    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;

    // Shadow
    float shadow = shadow(fragPos, lightDir, obj);
    return ambient + ((diffuse + specular) * shadow(fragPos, lightDir, obj));
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, Object obj) {
    light.ambient *= light.color;
    light.diffuse *= light.color;
    light.specular *= light.color;

    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir - viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 10);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * spec * color;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ambient + ((diffuse + specular) * shadow(fragPos, light.position, lightDir, obj));
}

vec3 calculateLight(vec3 pos, vec3 normal, vec3 inColor, Object obj) {
	vec3 color = vec3(0.0);

	color += calculateDirLight(dirLight, normal, inDir, inColor, pos, obj);

	for (int i = 0; i < LIGHT_NUM; ++i)
		color += calculatePointLight(pointLights[i], normal, pos, inDir, inColor, obj);

	return color;
}

float sphereSDF(vec3 pos, Sphere sphere) {
	return length(pos - sphere.center) - sphere.radius;
}

float cubeSDF(vec3 pos, Cube cube) {
	pos = (cube.inverseTransormation * vec4(pos, 1.0)).xyz;

	vec3 d = abs(pos) - cube.halfSize;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0) - cube.rounding;
}

float capsuleSDF(vec3 pos, Capsule capsule) {
	pos = (capsule.inverseTransormation * vec4(pos, 1.0)).xyz;

	vec3 pa = (pos - capsule.pos1), ba = capsule.pos2 - capsule.pos1;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - capsule.radius;
}

vec3 getSphereNormal(vec3 pos, Sphere sphere) {
	return normalize(pos - sphere.center);
}

vec3 getCubeNormal(vec3 pos, Cube cube) {
	/*vec3 relativePos = pos - cube.center;
    vec3 absRelativePos = abs(relativePos / cube.halfSize);

    float maxAxis = max(absRelativePos.x, max(absRelativePos.y, absRelativePos.z));

    if (maxAxis == absRelativePos.x) {
        return vec3(sign(relativePos.x), 0.0, 0.0);
    } else if (maxAxis == absRelativePos.y) {
        return vec3(0.0, sign(relativePos.y), 0.0);
    } else {
        return vec3(0.0, 0.0, sign(relativePos.z));
    }
	return vec3(0.0);*/

	vec3 normal = vec3(
	(cubeSDF(pos + dx, cube) - cubeSDF(pos - dx, cube)),
	(cubeSDF(pos + dy, cube) - cubeSDF(pos - dy, cube)),
	(cubeSDF(pos + dz, cube) - cubeSDF(pos - dz, cube))
);
	return normalize(normal);
}

vec3 getCapsuleNormal(vec3 pos, Capsule capsule) {
	vec3 normal = vec3(
	(capsuleSDF(pos + dx, capsule) - capsuleSDF(pos - dx, capsule)),
	(capsuleSDF(pos + dy, capsule) - capsuleSDF(pos - dy, capsule)),
	(capsuleSDF(pos + dz, capsule) - capsuleSDF(pos - dz, capsule))
);
	return normalize(normal);
}

Intersect sceneDist(vec3 pos, vec3 direction) {
	Intersect ans = {MAX_DIST, {0, 0}};
		for (int i = 0; i < LIGHT_NUM; ++i) {
			float dist = sphereSDF(pos, Sphere(1.0, pointLights[i].position, pointLights[i].color, 0.0));

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = LIGHT;
			}
		}
		for (int i = 0; i < SPHERE_NUM; ++i) {
			float dist = sphereSDF(pos, spheres[i]);

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = SPHERE;
			}
		}
		for (int i = 0; i < CUBE_NUM; ++i) {
			float dist = cubeSDF(pos, cubes[i]);

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = CUBE;
			}
		}
		for (int i = 0; i < CAPSULE_NUM; ++i) {
			float dist = capsuleSDF(pos, capsules[i]);

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = CAPSULE;
			}
		}
	return ans;
}

Intersect sceneDist(vec3 pos, vec3 direction, int type, int idx) {
	Intersect ans = {MAX_DIST, {0, 0}};
		for (int i = 0; i < LIGHT_NUM; ++i) {
			if (type == LIGHT && idx == i) continue;

			float dist = sphereSDF(pos, Sphere(1.0, pointLights[i].position, pointLights[i].color, 0.0));

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = LIGHT;
			}
		}
		for (int i = 0; i < SPHERE_NUM; ++i) {
			if (type == SPHERE && idx == i) continue;

			float dist = sphereSDF(pos, spheres[i]);

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = SPHERE;
			}
		}
		for (int i = 0; i < CUBE_NUM; ++i) {
			if (type == CUBE && idx == i) continue;

			float dist = cubeSDF(pos, cubes[i]);

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = CUBE;
			}
		}
		for (int i = 0; i < CAPSULE_NUM; ++i) {
			if (type == CAPSULE && idx == i) continue;

			float dist = capsuleSDF(pos, capsules[i]);

			if (dist < ans.dist) {
				ans.dist = dist;
				ans.obj.idx = i;
				ans.obj.type = CAPSULE;
			}
		}
	return ans;
}

vec3 getReflection(vec3 origin, vec3 dir, Object obj) {
	Intersect intersect = {MAX_DIST, {0, 0}};
	vec3 pos = origin;

	while (length(pos - origin) < MAX_DIST) {
		intersect = sceneDist(pos, dir, obj.type, obj.idx);

		if (intersect.dist < eplison) {
			vec3 normal = vec3(0.0);
			vec3 color = vec3(0.0);

			switch (intersect.obj.type) {
				case LIGHT:
					return pointLights[intersect.obj.idx].color;
					break;
				case SPHERE:
					normal = getSphereNormal(pos, spheres[intersect.obj.idx]);
					return calculateLight(pos, normal, spheres[intersect.obj.idx].color, intersect.obj);
					break;
				case CUBE:
					normal = getCubeNormal(pos, cubes[intersect.obj.idx]);
					return calculateLight(pos, normal, cubes[intersect.obj.idx].color, intersect.obj);
					break;
				case CAPSULE:
					normal = getCapsuleNormal(pos, capsules[intersect.obj.idx]);
					return calculateLight(pos, normal, capsules[intersect.obj.idx].color, intersect.obj);
					break;
			}
		}

		pos += dir * intersect.dist;
	}

	return vec3(0.0);
}

vec3 getColor(Intersect intersect, vec3 pos, vec3 dir) {
	vec3 normal = vec3(0.0);
	vec3 color = vec3(0.0);
	float reflection = 0.0;

	switch (intersect.obj.type) {
		case LIGHT:
			return pointLights[intersect.obj.idx].color;
			break;
		case SPHERE:
			normal = getSphereNormal(pos, spheres[intersect.obj.idx]);
			color = calculateLight(pos, normal, spheres[intersect.obj.idx].color, intersect.obj);
			reflection = spheres[intersect.obj.idx].reflection;

			if (reflection == 0.0) return color;
			break;
		case CUBE:
			normal = getCubeNormal(pos, cubes[intersect.obj.idx]);
			color = calculateLight(pos, normal, cubes[intersect.obj.idx].color, intersect.obj);
			reflection = cubes[intersect.obj.idx].reflection;

			if (reflection == 0.0) return color;
			break;
		case CAPSULE:
			normal = getCapsuleNormal(pos, capsules[intersect.obj.idx]);
			color = calculateLight(pos, normal, capsules[intersect.obj.idx].color, intersect.obj);
			reflection = capsules[intersect.obj.idx].reflection;

			if (reflection == 0.0) return color;
			break;
	}

	return mix(color, getReflection(pos, reflect(dir, normal), intersect.obj), reflection);
}

vec3 rayMarch(vec3 pos, vec3 direction) {
	Intersect intersect = {MAX_DIST, {0, 0}};

	while (length(pos - position) < MAX_DIST) {
		intersect = sceneDist(pos, direction);

		if (intersect.dist < eplison) {
			return getColor(intersect, pos, direction);
		}

		pos += direction * intersect.dist;
	}

	return vec3(0.0);
}