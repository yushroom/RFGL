//
//  Time.hpp
//  PRT
//
//  Created by 俞云康 on 9/10/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Time_hpp
#define Time_hpp

class RenderSystem;

class Time
{
public:
    friend class RenderSystem;
    Time() = delete;
    
    static float getDeltaTime() {
        return m_deltaTime;
    }
    
private:
    static float m_deltaTime;
};

#endif /* Time_hpp */
