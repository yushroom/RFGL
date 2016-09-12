//
//  Debug.hpp
//  PRT
//
//  Created by 俞云康 on 9/11/16.
//  Copyright © 2016 yushroom. All rights reserved.
//

#ifndef Debug_hpp
#define Debug_hpp

class Debug
{
public:
    // Logs message to the Console.
    static void Log(const char *format, ...);
    
    // A variant of Debug.Log that logs a warning message to the console.
    static void LogWarning(const char *format, ...);
    
    // A variant of Debug.Log that logs an error message to the console.
    static void LogError(const char *format, ...);

    static void Init();
};

#endif /* Debug_hpp */
