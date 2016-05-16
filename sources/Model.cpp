//
//  Model.cpp
//  PRT
//
//  Created by 俞云康 on 5/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#include "Model.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;


Model::Model(const int n_vertex, const int n_face, float* positions, uint32_t* indices)
    :   m_positionBuffer(positions, positions+n_vertex*3),
        m_indexBuffer(indices, indices+n_face*3)
{
    GenerateBuffer();
    BindBuffer();
}

Model::Model(std::vector<float> position_buffer, std::vector<uint32_t> index_buffer)
: m_positionBuffer(position_buffer), m_indexBuffer(index_buffer)
{
    GenerateBuffer();
    BindBuffer();
}

Model::Model(const int n_vertex, const int n_face, float* positions, float* normals, uint32_t* indices)
    :   m_positionBuffer(positions, positions+n_vertex*3),
        m_normalBuffer(normals, normals+n_vertex*3),
        m_indexBuffer(indices, indices+n_face*3)
{
    GenerateBuffer();
    BindBuffer();
}

Model::Model(const std::string& objModelPath)
{
    Assimp::Importer importer;
    const char* path = objModelPath.c_str();
    unsigned int load_potion =
    aiProcess_Triangulate
    | aiProcess_SortByPType
    | aiProcess_GenSmoothNormals
    | aiProcess_CalcTangentSpace
    | aiProcess_JoinIdenticalVertices
    | aiProcess_FixInfacingNormals
    | aiProcess_OptimizeGraph
    | aiProcess_OptimizeMeshes
    | aiProcess_FlipUVs
    | aiProcess_CalcTangentSpace
    ;
    const aiScene* scene = importer.ReadFile(path, load_potion);
    if (!scene) {
        abort();
    }
    
    int n_vertices = 0;
    int n_triangles = 0;
    
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        n_vertices += mesh->mNumVertices;
        n_triangles += mesh->mNumFaces;
    }
    
    m_positionBuffer.reserve(n_vertices * 3);
    m_normalBuffer.reserve(n_vertices * 3);
    m_uvBuffer.reserve(n_vertices * 2);
    m_indexBuffer.reserve(n_triangles * 3);
    m_tangentBuffer.reserve(n_triangles * 3);
    int idx = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        bool has_uv = mesh->HasTextureCoords(0);
        if (!has_uv)
            printf("mesh[%d] do not have uv!\n", i);
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            auto& v = mesh->mVertices[j];
            m_positionBuffer.push_back(v.x);
            m_positionBuffer.push_back(v.y);
            m_positionBuffer.push_back(v.z);
            
            auto& n = mesh->mNormals[j];
            m_normalBuffer.push_back(n.x);
            m_normalBuffer.push_back(n.y);
            m_normalBuffer.push_back(n.z);
            
            auto& uv = mesh->mTextureCoords[0][j];
            m_uvBuffer.push_back(uv.x);
            m_uvBuffer.push_back(uv.y);
            
            auto& t = mesh->mTangents[j];
            m_tangentBuffer.push_back(t.x);
            m_tangentBuffer.push_back(t.y);
            m_tangentBuffer.push_back(t.z);
        }
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            auto& face = mesh->mFaces[j];
            assert(face.mNumIndices == 3);
            for (int fi = 0; fi < 3; ++fi)
                m_indexBuffer.push_back(face.mIndices[fi] + idx);
        }
        idx += mesh->mNumFaces;
    }
    
    GenerateBuffer();
    BindBuffer();
}

Model::~Model() {
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_positionVBO);
    glDeleteBuffers(1, &m_normalVBO);
    glDeleteBuffers(1, &m_tangentVBO);
    glDeleteBuffers(1, &m_indexVBO);
}

void Model::Render(const Shader& shader) {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::BindBuffer(int vertexUsage/* = VertexUsagePN*/) {
    //glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glVertexAttribPointer(PositionIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(PositionIndex);
    
    if (vertexUsage & VertexUsageNormal) {
        glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
        glVertexAttribPointer(NormalIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(NormalIndex);
    }
    
    if (vertexUsage & VertexUsageUV) {
        glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
        glVertexAttribPointer(UVIndex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(UVIndex);
    }
    
    if (vertexUsage & VertexUsageTangent) {
        glBindBuffer(GL_ARRAY_BUFFER, m_tangentVBO);
        glVertexAttribPointer(TangentIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(TangentIndex);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

Model& Model::GetQuad()
{
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // Top Right
        0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f   // Top Left
    };
    
    uint32_t indices[] = {  // Note that we start from 0!
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle
    };
    static Model quad(sizeof(vertices)/12, sizeof(indices)/12, vertices, indices);
    return quad;
}

Model& Model::GetBox()
{
#if defined(_WIN32)
	static Model box("D:/program/RFGL/models/cube.obj");
#else
	static Model box("/Users/yushroom/program/graphics/PRT/cube.obj");
#endif
    return box;
}

Model& Model::GetSphere()
{
#if defined(_WIN32)
	static Model sphere("D:/program/RFGL/models/Sphere.obj");
#else
    static Model sphere("/Users/yushroom/program/github/SeparableSSS/SeparableSSS/Models/Sphere.obj");
#endif
    return sphere;
}
