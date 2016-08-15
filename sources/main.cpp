#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <AntTweakBar.h>

#include "Shader.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "GLError.hpp"
#include "Camera.hpp"


// Function prototypes
void windowSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const std::string simpleVS = R"(
uniform mat4 mvp;
layout (location = PositionIndex) in vec3 position;
void main ()
{
    gl_Position = mvp * vec4(position, 1);
}
)";

const std::string simpleFS = R"(
out vec4 color;
void main()
{
    color = vec4(1, 0, 0, 1);
}
)";

const std::string vertexShaderSource = R"(//#version 410 core
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
layout (location = TangentIndex) in vec3 tangent;
layout (location = UVIndex) in vec2 uv;
//uniform sampler2D bumpTex;
uniform mat4 mvp;
uniform mat4 model;
out VS_OUT {
    vec3 normal;
    vec3 tangent;
    vec2 uv;
} vs_out;

void main()
{
    vs_out.normal = mat3(model) * normal;
    vs_out.tangent = mat3(model) * tangent;
    vs_out.uv = uv;
    gl_Position = mvp * vec4(position, 1.0);
}
)";
const std::string fragmentShaderSource = R"(//#version 410 core
const vec3 lightDir = normalize(vec3(1, 1, 1));
const float bumpiness = 0.2;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
in VS_OUT {
    vec3 normal;
    vec3 tangent;
    vec2 uv;
} vs_out;
out vec4 color;
void main()
{
    vec3 N = normalize(vs_out.normal);
    vec3 T = normalize(vs_out.tangent);
    vec3 B = normalize(cross(T, N));
    mat3 TBN = mat3(T, B, N);
    vec3 bump_normal;
    bump_normal.xy = 2.0 * texture(normalMap, vs_out.uv).gr - 1.0;
    bump_normal.z = sqrt(1.0 - dot(bump_normal.xy, bump_normal.xy));
    vec3 tangent_normal = mix(vec3(0, 0, 1), bump_normal, bumpiness);
    vec3 normal = TBN * tangent_normal;
    
    float c = clamp(dot(normal, lightDir), 0.0, 1.0);
    
    color.rgb = c * texture(diffuseMap, vs_out.uv).rgb;
    //color.rgb = vec3(c,c,c);
    color.a = 1.0f;
}
)";

const std::string skyBoxVert = R"(//#version 410 core
layout(location = 0) in vec3 position;
out vec3 uv;
uniform mat4 mvp;
void main()
{
    gl_Position = mvp * vec4(position, 1);
    uv = position.xyz;
    //uv.z = -uv.z;
}
)";

const std::string skyBoxFrag = R"(//#version 410 core
in vec3 uv;
layout(location = PositionIndex) out vec4 color;
uniform float intensity;
uniform samplerCube skyTex;
void main()
{
    color = texture(skyTex, normalize(uv));
    //color.rgb = normalize(uv);
    color.a = 1.0;
}
)";

const std::string tessVS = R"(
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
layout (location = UVIndex) in vec2 uv;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    vs_out.position = vec3( uModelMatrix * vec4(position, 1) );
    vs_out.normal = uNormalMatrix * normal;
    vs_out.uv = uv;
}
)";

const std::string tessTCS = R"(
layout(vertices = 3) out;
in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out[];
out TCS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} tcs_out[];
//out vec3 tcPosition[];
uniform float TessLevelInner;
uniform float TessLevelOuter;

#define ID gl_InvocationID

void main()
{
    //tcPosition[ID] = vs_out[ID].position;
    tcs_out[ID].position = vs_out[ID].position;
    tcs_out[ID].normal = vs_out[ID].normal;
    tcs_out[ID].uv = vs_out[ID].uv;
    
    //if (ID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    //}
}
)";

// phong tessellation
const std::string tessTES = R"(
layout(triangles, equal_spacing, ccw) in;
//in vec3 tcPosition[];
in TCS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} tcs_out[];
//out vec3 tePosition;
//out vec3 tePatchDistance;
out TES_OUT {
    vec3 position;
    vec3 normal;
    //vec3 patchDistance;
    vec2 uv;
} tes_out;
uniform mat4 uViewProjMatrix;
//uniform mat4 Projection;
//uniform mat4 Modelview;
//uniform mat3 NormalMatrix;
uniform sampler2D displacementMap;
uniform float bumpiness;
const float alpha = 0.75;   // phong tessellation control factor

vec3 Project(vec3 q, vec3 p, vec3 n) {
    return q - dot(q-p, n)*n;
}

void main()
{
    //tes_out.patchDistance = gl_TessCoord;
    vec3 linear_pos = mat3(tcs_out[0].position, tcs_out[1].position, tcs_out[2].position) * gl_TessCoord;
    vec3 proj_pos = mat3(Project(linear_pos, tcs_out[0].position, tcs_out[0].normal),
                         Project(linear_pos, tcs_out[1].position, tcs_out[1].normal),
                         Project(linear_pos, tcs_out[2].position, tcs_out[2].normal))
                    * gl_TessCoord;
    tes_out.position = mix(linear_pos, proj_pos, alpha);
    tes_out.normal = mat3(tcs_out[0].normal,
                          tcs_out[1].normal,
                          tcs_out[2].normal) * gl_TessCoord;
    tes_out.normal = normalize(tes_out.normal);
    tes_out.uv = gl_TessCoord.x * tcs_out[0].uv +
                 gl_TessCoord.y * tcs_out[1].uv +
                 gl_TessCoord.z * tcs_out[2].uv;
    tes_out.position += texture(displacementMap, tes_out.uv).r * bumpiness * tes_out.normal;
    gl_Position =  uViewProjMatrix * vec4(tes_out.position, 1);
}
)";

//const std::string tessGS = R"(
//uniform mat4 Modelview;
//uniform mat3 NormalMatrix;
//layout(triangles) in;
//layout(triangle_strip, max_vertices = 3) out;
//in vec3 tePosition[3];
//in vec3 tePatchDistance[3];
//out vec3 gFacetNormal;
//out vec3 gPatchDistance;
//out vec3 gTriDistance;
//
//void main()
//{
//    vec3 A = tePosition[2] - tePosition[0];
//    vec3 B = tePosition[1] - tePosition[0];
//    gFacetNormal = NormalMatrix * normalize(cross(A, B));
//    
//    gPatchDistance = tePatchDistance[0];
//    gTriDistance = vec3(1, 0, 0);
//    gl_Position = gl_in[0].gl_Position; EmitVertex();
//    
//    gPatchDistance = tePatchDistance[1];
//    gTriDistance = vec3(0, 1, 0);
//    gl_Position = gl_in[1].gl_Position; EmitVertex();
//    
//    gPatchDistance = tePatchDistance[2];
//    gTriDistance = vec3(0, 0, 1);
//    gl_Position = gl_in[2].gl_Position; EmitVertex();
//    
//    EndPrimitive();
//}
//)";

const std::string tessFS = R"(
out vec4 FragColor;
in TES_OUT {
    vec3 position;
    vec3 normal;
    //vec3 patchDistance;
    vec2 uv;
} tes_out;
in float gPrimitive;
//uniform vec3 LightPosition;
uniform sampler2D diffuseMap;
const vec3 lightDir = normalize(vec3(1, 1, 1));

void main()
{
    vec3 N = normalize(tes_out.normal);
    float NDotL = clamp(dot(N, lightDir), 0.0, 1.0);
    FragColor = texture(diffuseMap, tes_out.uv) * NDotL;
    //FragColor.rgb = vec3(NDotL);
    //FragColor.a = 1;
}
)";

const std::string visualizeNormalVert = R"(//#version 410 core
layout (location = PositionIndex) in vec3 position;
layout (location = NormalIndex) in vec3 normal;
out VS_OUT {
    vec3 normal;
} vs_out;
out vec3 fragNormal;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main() {
    gl_Position = proj * view * model * vec4(position, 1.0f);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(proj * vec4(normalMatrix*normal, 1.0)));
}
)";

const std::string visualizeNormalGS = R"(//#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.3f;

void GenerateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.f) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}
)";

const std::string visualizeNormalFrag = R"(//#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0, 0.5, 1.0, 1.0);
}
)";


Camera camera(glm::vec3(0, 0, 3));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


// The MAIN function, from here we start the application and run the game loop
int main()
{
    std::cout << "Starting GLFW context, OpenGL 4.1" << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    
    glCheckError();
    
    TwInit(TW_OPENGL_CORE, NULL);
    TwBar *myBar = TwNewBar("MyBar");
    unsigned int NumberOfTexUnits = 0; // query the hardware to get the number of available texture units
    // ...
    TwAddVarRO(myBar, "NumTexUnits", TW_TYPE_UINT32, &NumberOfTexUnits, " label='# of texture units' ");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.
    
    double fps = 0;
    uint32_t tessLevelInner = 3;
    uint32_t tessLevelOuter = 2;
    float bumpiness = 0.001f;
    double time = 0;// Current time and enlapsed time
    //double turn = 0;    // Model turn counter
    double speed = 0.3; // Model rotation speed
    int wire = 0;       // Draw model in wireframe?
    float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color
    unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)
    // Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
    TwAddVarRW(myBar, "speed", TW_TYPE_DOUBLE, &speed,
               " label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");
    
    // Add 'wire' to 'bar': it is a modifable variable of type TW_TYPE_BOOL32 (32 bits boolean). Its key shortcut is [w].
    TwAddVarRW(myBar, "wire", TW_TYPE_BOOL32, &wire,
               " label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");
    
    // Add 'time' to 'bar': it is a read-only (RO) variable of type TW_TYPE_DOUBLE, with 1 precision digit
    TwAddVarRO(myBar, "fps", TW_TYPE_DOUBLE, &fps, " label='fps' precision=1 help='FPS' ");
    
    // Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
    TwAddVarRW(myBar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");
    
    // Add 'cubeColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR32 (32 bits color) with alpha
    TwAddVarRW(myBar, "cubeColor", TW_TYPE_COLOR32, &cubeColor,
               " label='Cube color' alpha help='Color and transparency of the cube.' ");
    TwAddVarRW(myBar, "TessLevelInner", TW_TYPE_UINT32, &tessLevelInner, "label='TessLevelInner' min=1 max=32");
    TwAddVarRW(myBar, "TessLevelOuter", TW_TYPE_UINT32, &tessLevelOuter, "label='TessLevelOuter' min=1 max=32");
    TwAddVarRW(myBar, "Bumpiness", TW_TYPE_FLOAT, &bumpiness,
               " label='bumpinrdd' min=0 max=0.5 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");
    
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    GLint MaxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
    printf("Max supported patch vertices %d\n", MaxPatchVertices);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    
    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);

    //Camera camera(glm::vec3(0, 0, 3));
#if defined(_WIN32)
	const std::string root_dir = "../";
#else
    const std::string root_dir = "/Users/yushroom/program/graphics/RFGL/";
#endif
    //const std::string root_dir = "/Users/yushroom/Downloads/head2";
    const std::string models_dir = root_dir + "models/";
    const std::string textures_dir = root_dir + "textures/";
    //const std::string models_dir = root_dir;
    //const std::string textures_dir = root_dir;
    
    Shader shader;
    shader.FromString(vertexShaderSource, fragmentShaderSource);
    Shader skyShader;
    skyShader.FromString(skyBoxVert, skyBoxFrag);
    Shader visualizeNormalShader;
    visualizeNormalShader.FromString(visualizeNormalVert, visualizeNormalFrag, visualizeNormalGS);
    Shader simpleShader;
    simpleShader.FromString(simpleVS, simpleFS);
    
    Shader tessShader;
    tessShader.FromString(tessVS, tessTCS, tessTES, "", tessFS);
    //Model head("/Users/yushroom/program/graphics/SoftRenderer/Model/head/head.OBJ");
    //Model head(models_dir+"head/head_optimized.obj");
    Model head(models_dir+"/head/head.obj", VertexUsagePNUT);
    //head.SetVertexUsage(VertexUsagePNUT);
    glCheckError();
    
    Model& quad = Model::GetQuad();
    quad.SetVertexUsage(VertexUsagePNUT);
    
    Texture skyTex(textures_dir+"StPeters/DiffuseMap.dds");
    //auto& skyTex = Texture::GetSimpleTexutreCubeMap();
    //Texture diffuse(models_dir+"head/DiffuseMap_R8G8B8A8_1024_mipmaps.dds");
    Texture diffuse(models_dir+"/head/lambertian.jpg");
    //bump.png
    //Texture bumpTexture(models_dir + "/head/bump.png");
    Texture bumpTexture(models_dir + "/head/bump-lowRes.png");
    Texture normalMap(models_dir + "/head/NormalMap_RG16f_1024_mipmaps.dds");
    //Texture diffuse("/Users/yushroom/program/github/SeparableSSS/SeparableSSS/head/head_optimized.png");
    Texture bricksDiffuse(textures_dir + "/bricks_diffuse.jpg");
    Texture bricksDisplacementMap(textures_dir + "/bricks_displacementmap.png");
    
    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    double t = glfwGetTime();
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        Do_Movement();

        
        if (wire == 1)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glPatchParameteri(GL_PATCH_VERTICES, 3);

        static float angle = 0;
        glm::mat4 model = glm::rotate(glm::scale(glm::mat4(), glm::vec3(10, 10, 10)), angle, glm::vec3(0, 1, 0));
        //angle += 0.005f;
        if (angle > M_PI * 2.0f) angle -= M_PI * 2.0f;
        auto view = camera.GetViewMatrix();
        auto proj = glm::perspective(glm::radians(camera.Zoom), float(width)/height, 0.1f, 100.f);
        auto mvp = proj * view * model;
        
//        glDisable(GL_CULL_FACE);
//        auto sky_mvp = proj * view * glm::scale(glm::mat4(), glm::vec3(10, 10, 10));
//        skyShader.Use();
//        skyShader.BindUniformMat4("mvp", sky_mvp);
//        skyShader.BindUniformTexture("skyTex", skyTex.GetGLTexuture(), 0, GL_TEXTURE_CUBE_MAP);
//        Model::GetSphere().Render(skyShader);
//        glEnable(GL_CULL_FACE);
        
//        shader.Use();
//        shader.BindUniformMat4("mvp", mvp);
//        shader.BindUniformMat4("model", model);
//        //shader.BindUniformTexture("bumpTex", bumpTexture.GetGLTexuture(), 0);
//        shader.BindUniformTexture("diffuseMap", diffuse.GetGLTexuture(), 0);
//        shader.BindUniformTexture("normalMap", normalMap.GetGLTexuture(), 1);
//        head.Render(shader);
        
        //auto modelView = view * glm::rotate(glm::mat4(), -45.0f, glm::vec3(1, 0, 0));
        //auto modelView = view * model;
        auto modelView = view;
        tessShader.Use();
        tessShader.BindUniformMat4("uViewProjMatrix", proj * view);
        tessShader.BindUniformMat4("uModelMatrix", glm::mat4());
        tessShader.BindUniformMat3("uNormalMatrix", glm::mat3(modelView));
        //tessShader.BindUniformVec3("LightPosition", glm::vec3(5, 5, 5));
        tessShader.BindUniformFloat("TessLevelInner", tessLevelInner);
        tessShader.BindUniformFloat("TessLevelOuter", tessLevelOuter);
        tessShader.BindUniformFloat("bumpiness", bumpiness);
        tessShader.BindUniformTexture("displacementMap", bricksDisplacementMap.GetGLTexuture(), 0);
        tessShader.BindUniformTexture("diffuseMap", bricksDiffuse.GetGLTexuture(), 1);
        //Model::GetIcosahedron().RenderPatch(tessShader);
        //Model::GetBox().RenderPatch(tessShader);
        //head.RenderPatch(tessShader);
        sphere88.RenderPatch(tessShader);
        //quad.RenderPatch(tessShader);
        
//        glCheckError();
//        simpleShader.Use();
//        simpleShader.BindUniformMat4("mvp", proj * view);
//        //Model::GetIcosahedron().Render(simpleShader);
//        //Model::GetQuad().Render(simpleShader);
//        Model::GetBox().Render(simpleShader);
        
//        visualizeNormalShader.Use();
//        visualizeNormalShader.BindUniformMat4("model", model);
//        visualizeNormalShader.BindUniformMat4("view", view);
//        visualizeNormalShader.BindUniformMat4("proj", proj);
//        head.Render(shader);
        
        double new_t = glfwGetTime();
        fps = 1.0 / (new_t - t);
        t = new_t;
        if (wire == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        TwDraw();
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    TwEventMouseButtonGLFW(button, action);
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    //glViewport(0, 0, width, height);
    TwWindowSize(width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
    
    TwEventKeyGLFW(key, action);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    //camera.ProcessMouseMovement(xoffset, yoffset);
    //TwEventMousePosGLFW(xpos, ypos);
    TwEventMousePosGLFW(xpos*2, ypos*2); // for retina
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //printf("%le\n", yoffset);
    camera.ProcessMouseScroll(yoffset);
    TwEventMouseWheelGLFW(yoffset);
}

void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
