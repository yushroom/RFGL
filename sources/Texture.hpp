//
//  Texture.hpp
//  PRT
//
//  Created by 俞云康 on 5/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Texture_hpp
#define Texture_hpp

#include "PRT.hpp"
#include <string>

class Texture
{
public:
    //Texture(GLuint texture) : m_texture(texture) {};
    Texture(const std::string& path);
    Texture(const Texture&) = delete;
    void operator=(const Texture&) = delete;
    ~Texture();
    
    //static Texture& GetSimpleTexutreCubeMap();
    
    GLuint GetGLTexuture() const {
        return m_texture;
    }
    
private:
    GLuint m_texture = 0;
};

#endif /* Texture_hpp */
