#ifndef Time_hpp
#define Time_hpp

class RenderSystem;

class Time
{
public:
    friend class RenderSystem;
    Time() = delete;
    
    static float DeltaTime() {
        return m_deltaTime;
    }
    
private:
    static float m_deltaTime;
};

#endif /* Time_hpp */
