#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "GLError.hpp"
#include "Camera.hpp"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
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

    GLint MaxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
    printf("Max supported patch vertices %d\n", MaxPatchVertices);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    
    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

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
    //Model head("/Users/yushroom/program/graphics/SoftRenderer/Model/head/head.OBJ");
    //Model head(models_dir+"head/head_optimized.obj");
    Model head(models_dir+"/head/head.obj");
    head.SetVertexUsage(VertexUsagePNUT);
    glCheckError();
    
    Texture skyTex(textures_dir+"StPeters/DiffuseMap.dds");
    //auto& skyTex = Texture::GetSimpleTexutreCubeMap();
    //Texture diffuse(models_dir+"head/DiffuseMap_R8G8B8A8_1024_mipmaps.dds");
    Texture diffuse(models_dir+"/head/lambertian.jpg");
    //Texture bumpTexture(models_dir + "/head/bump-lowRes.png");
    Texture normalMap(models_dir + "/head/NormalMap_RG16f_1024_mipmaps.dds");
    //Texture diffuse("/Users/yushroom/program/github/SeparableSSS/SeparableSSS/head/head_optimized.png");
    
    // Uncommenting this call will result in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        Do_Movement();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static float angle = 0;
        glm::mat4 model = glm::rotate(glm::scale(glm::mat4(), glm::vec3(10, 10, 10)), angle, glm::vec3(0, 1, 0));
        angle += 0.005f;
        if (angle > M_PI * 2.0f) angle -= M_PI * 2.0f;
        auto view = camera.GetViewMatrix();
        auto proj = glm::perspective(camera.Zoom, float(width)/height, 0.1f, 100.f);
        auto mvp = proj * view * model;
        
        glDisable(GL_CULL_FACE);
        auto sky_mvp = proj * view * glm::scale(glm::mat4(), glm::vec3(10, 10, 10));
        skyShader.Use();
        skyShader.BindUniformMat4("mvp", sky_mvp);
        skyShader.BindUniformTexture("skyTex", skyTex.GetGLTexuture(), 0, GL_TEXTURE_CUBE_MAP);
        Model::GetSphere().Render(skyShader);
        glEnable(GL_CULL_FACE);
        
        shader.Use();
        shader.BindUniformMat4("mvp", mvp);
        shader.BindUniformMat4("model", model);
        //shader.BindUniformTexture("bumpTex", bumpTexture.GetGLTexuture(), 0);
        shader.BindUniformTexture("diffuseMap", diffuse.GetGLTexuture(), 0);
        shader.BindUniformTexture("normalMap", normalMap.GetGLTexuture(), 1);
        head.Render(shader);
        
//        visualizeNormalShader.Use();
//        visualizeNormalShader.BindUniformMat4("model", model);
//        visualizeNormalShader.BindUniformMat4("view", view);
//        visualizeNormalShader.BindUniformMat4("proj", proj);
//        head.Render(shader);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
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
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
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
