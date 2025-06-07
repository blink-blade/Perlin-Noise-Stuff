#version 460 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec3 ourColor; // output a color to the fragment shader
out vec2 TexCoord;
uniform float time;
uniform mat4 view;
uniform mat4 projection;

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

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

void main()
{
    //mat4 translation = mat4(1.0, 0.0, 0.0, xTrans,
      //          0.0, 1.0, 0.0, yTrans,
        //        0.0, 0.0, 1.0, ZTrans,
          //      0.0, 0.0, 0.0, 1.0);
    gl_Position = projection * view * instanceMatrix * (vec4(aPos.x, aPos.y, aPos.z, 1.0) * rotationMatrix(time, 0.0, time));
    //gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    //ourColor = aColor; // set ourColor to the input color we got from the vertex data
    TexCoord = aTexCoord;
}