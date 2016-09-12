#ifndef Texture_hpp
#define Texture_hpp

#include "RFGL.hpp"
#include <string>

class Texture
{
public:
    Texture() {}
    //Texture(GLuint texture) : m_texture(texture) {};
    Texture(const std::string& path);
    Texture(const Texture&) = delete;
    void operator=(const Texture&) = delete;
    ~Texture();

    typedef std::shared_ptr<Texture> PTexture;
    
    //static Texture& GetSimpleTexutreCubeMap();

    static PTexture CreateFromFile(const std::string& path) {
        auto t = std::make_shared<Texture>();
        t->FromFile(path);
        return t;
    }

    void FromFile(const std::string& path);
    
    GLuint GLTexuture() const {
        return m_texture;
    }
    
private:
    GLuint m_texture = 0;
};

#endif /* Texture_hpp */
