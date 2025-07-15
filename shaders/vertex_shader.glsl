#version 460 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords; 
layout (location = 3) in float aType; 

out vec3 NormalC;
out float typeC;
out vec2 uvs;

mat4 rotationMatrix(float angleX, float angleY, float angleZ) {
    float cX = cos(angleX);
    float sX = sin(angleX);
    float cY = cos(angleY);
    float sY = sin(angleY);
    float cZ = cos(angleZ);
    float sZ = sin(angleZ);
    return mat4(cZ, sZ, 0.0, 0.0,
                -sZ, cZ, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0) * 
            mat4(cY, 0.0, -sY, 0.0,
                0.0, 1.0, 0.0, 0.0,
                sY, 0.0, cY, 0.0,
                0.0, 0.0, 0.0, 1.0) *
            mat4(1.0, 0.0, 0.0, 0.0,
                0.0, cX, sX, 0.0,
                0.0, -sX, cX, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    //mat4 translation = mat4(1.0, 0.0, 0.0, -1000.0f,
          //      0.0, 1.0, 0.0, 0.0,
        //        0.0, 0.0, 1.0, -1000.0f,
      //          0.0, 0.0, 0.0, 1.0);
    //mat4 rotMatrix = rotationMatrix(0.0f, 0.0f, 0.0f);
    //gl_Position = projection * view * (vec4(aPos.x + (sin(time + (aPos.z / 25)) * 25), aPos.y + (cos(time + (aPos.y / 25)) * 25), aPos.z + (cos(time + (aPos.x / 25)) * 25), 1.0) * translation * rotMatrix);
    //gl_Position = projection * view * (vec4(aPos.x, aPos.y, aPos.z, 1.0));
    //gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    //ourColor = vec3(aColor.x, aColor.y + (cos(time + (aPos.y / 25)) * 5), aColor.z + (sin(time + (aPos.z / 25))));
    typeC = aType;
    NormalC = aNormal;
    uvs = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // Need to fix this later, but if there were a non-uniform scale transform on the vertices, then the normals would need fixed. This is covered in the second page of the lighting section in learnopengl.com
}