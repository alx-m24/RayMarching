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


struct Sphere {
	float radius;
	vec3 center;
	vec3 color;
};

struct Cube {
	vec3 halfSize;
	vec3 center;
	vec3 color;
};

// Types: 
#define LIGHT 0
#define SPHERE 1
#define CUBE 2

struct Object {
	int type;
	int idx;
};

struct Intersect {
	float dist;
	Object obj;
};

#define MAX_DIST 100.0
#define eplison 0.01

#define LIGHT_NUM 2
#define SPHERE_NUM 2
#define CUBE_NUM 2

// Objects
uniform Sphere spheres[SPHERE_NUM];
uniform Cube cubes[CUBE_NUM];
uniform PointLight pointLights[LIGHT_NUM];
// Camera
uniform vec2 iResolution;
uniform vec3 position;
uniform vec3 inDir;
uniform mat3 viewMatrix;

float sphereSDF(vec3 pos, Sphere sphere);
float cubeSDF(vec3 pos, Cube cube);
float shadow(vec3 origin, vec3 lightPos, vec3 dir, Object obj);
vec3 getSphereNormal(vec3 pos, Sphere sphere);
vec3 getCubeNormal(vec3 pos, Cube cube);
vec3 getColor(Intersect intersect, vec3 pos);
vec3 rayMarch(vec3 pos, vec3 direction);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, Object obj);
vec3 calculateLight(vec3 pos, vec3 normal, vec3 inColor, Object obj);
Intersect sceneDist(vec3 pos, vec3 direction);
Intersect sceneDist(vec3 pos, vec3 direction, int type, int idx);

void main() {
	vec2 aspectRatio = vec2(iResolution.x / iResolution.y, 1.0) * 0.25;
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

	for (int i = 0; i < LIGHT_NUM; ++i)
		color += calculatePointLight(pointLights[i], normal, pos, inDir, inColor, obj);

	return color;
}

float sphereSDF(vec3 pos, Sphere sphere) {
	return length(pos - sphere.center) - sphere.radius;
}

float cubeSDF(vec3 pos, Cube cube) {
	vec3 d = abs(pos - cube.center) - cube.halfSize;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

vec3 getSphereNormal(vec3 pos, Sphere sphere) {
	return normalize(pos - sphere.center);
}

vec3 getCubeNormal(vec3 pos, Cube cube) {
	vec3 relativePos = pos - cube.center;
    vec3 absRelativePos = abs(relativePos / cube.halfSize);

    float maxAxis = max(absRelativePos.x, max(absRelativePos.y, absRelativePos.z));

    if (maxAxis == absRelativePos.x) {
        return vec3(sign(relativePos.x), 0.0, 0.0);
    } else if (maxAxis == absRelativePos.y) {
        return vec3(0.0, sign(relativePos.y), 0.0);
    } else {
        return vec3(0.0, 0.0, sign(relativePos.z));
    }
	return vec3(0.0);
}

Intersect sceneDist(vec3 pos, vec3 direction) {
	Intersect ans = {MAX_DIST, {0, 0}};
		for (int i = 0; i < LIGHT_NUM; ++i) {
			float dist = sphereSDF(pos, Sphere(1.0, pointLights[i].position, pointLights[i].color));

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
	return ans;
}

Intersect sceneDist(vec3 pos, vec3 direction, int type, int idx) {
	Intersect ans = {MAX_DIST, {0, 0}};
		for (int i = 0; i < LIGHT_NUM; ++i) {
			if (type == LIGHT && idx == i) continue;

			float dist = sphereSDF(pos, Sphere(1.0, pointLights[i].position, pointLights[i].color));

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
	return ans;
}

vec3 getColor(Intersect intersect, vec3 pos) {
	vec3 normal = vec3(0.0);

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
	}
	return normal;
}

vec3 rayMarch(vec3 pos, vec3 direction) {
	Intersect intersect = {MAX_DIST, {0, 0}};

	while (length(pos - position) < MAX_DIST) {
		intersect = sceneDist(pos, direction);

		if (intersect.dist < eplison) {
			return getColor(intersect, pos);
		}

		pos += direction * intersect.dist;
	}

	return vec3(0.0);
}