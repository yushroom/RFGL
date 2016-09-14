#ifndef Model_hpp
#define Model_hpp

#include "Object.hpp"
#include "RFGL.hpp"

enum VertexUsage {
    VertexUsagePosition = 1<<0,
    VertexUsageNormal   = 1<<1,
    VertexUsageUV       = 1<<2,
    VertexUsageTangent  = 1<<3,
    VertexUsagePN       = VertexUsagePosition | VertexUsageNormal,
    VertexUsagePNU      = VertexUsagePN | VertexUsageUV,
    VertexUsagePNUT     = VertexUsagePNU | VertexUsageTangent
};

class Mesh : public Object
{
public:
    Mesh();
    Mesh(std::vector<float> position_buffer, std::vector<uint32_t> index_buffer);
    Mesh(const int n_vertex, const int n_face, float* positions, uint32_t* indices);
    Mesh(const int n_vertex, const int n_face, float* positions, float* normals, uint32_t* indices);
    Mesh(const std::string& objModelPath, int vertexUsage);
    
    Mesh(const Mesh&) = delete;
    void operator=(const Mesh&) = delete;

    Mesh(Mesh&& m);
    
    ~Mesh();
    
    typedef std::shared_ptr<Mesh> PModel;

    static PModel CreateFromObjFile(const std::string path, int vertexUsage) {
        auto m = std::make_shared<Mesh>();
        m->FromObjFile(path, vertexUsage);
        return m;
    }

    void FromObjFile(const std::string path, int vertexUsage);

    void SetVertexUsage(int vertexUsage) {
        BindBuffer(vertexUsage);
    }
    
    void Render();
    //void renderPatch(const Shader& shader);
    
    //static Model& getQuad();
    //static Model& getBox();
    //static Model& getSphere();
    //static Model& getIcosahedron();
    
private:
    std::vector<float>      m_positionBuffer;
    std::vector<float>      m_normalBuffer;
    std::vector<float>      m_uvBuffer;
    std::vector<float>      m_tangentBuffer;
    std::vector<uint32_t>   m_indexBuffer;
    GLuint m_VAO;
    GLuint m_indexVBO;
    GLuint m_positionVBO;
    GLuint m_normalVBO;
    GLuint m_uvVBO;
    GLuint m_tangentVBO;
    
    void GenerateBuffer(int vertexUsage);
    void BindBuffer(int vertexUsage);
};

class DynamicModel
{

};

#endif /* Model_hpp */
