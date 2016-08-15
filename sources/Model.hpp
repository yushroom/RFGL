//
//  Model.hpp
//  PRT
//
//  Created by 俞云康 on 5/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp

#include <gl/glew.h>
#include <vector>
#include "Shader.hpp"

enum VertexUsage {
    VertexUsagePosition = 1<<0,
    VertexUsageNormal   = 1<<1,
    VertexUsageUV       = 1<<2,
    VertexUsageTangent  = 1<<3,
    VertexUsagePN       = VertexUsagePosition | VertexUsageNormal,
    VertexUsagePNU      = VertexUsagePN | VertexUsageUV,
    VertexUsagePNUT     = VertexUsagePNU | VertexUsageTangent
};

class Model
{
public:
    Model(std::vector<float> position_buffer, std::vector<uint32_t> index_buffer);
    Model(const int n_vertex, const int n_face, float* positions, uint32_t* indices);
    Model(const int n_vertex, const int n_face, float* positions, float* normals, uint32_t* indices);
    Model(const std::string& objModelPath, int vertexUsage);
    
    Model(const Model&) = delete;
    void operator=(const Model&) = delete;
    
    ~Model();
    
    void SetVertexUsage(int vertexUsage) {
        BindBuffer(vertexUsage);
    }
    
    void Render(const Shader& shader);
    void RenderPatch(const Shader& shader);
    
    static Model& GetQuad();
    static Model& GetBox();
    static Model& GetSphere();
    static Model& GetIcosahedron();
    
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

#endif /* Model_hpp */
