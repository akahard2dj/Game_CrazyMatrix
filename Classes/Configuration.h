//
//  Configuration.h
//  Tile Hunter
//
//  Created by 정 도원 on 2014. 5. 25..
//
//

#ifndef __Tile_Hunter__Configuration__
#define __Tile_Hunter__Configuration__

#include <iostream>

class Configuration {
    
public:
    static Configuration* getInstance();
    
private:
    static Configuration* obj;
    bool backgroundMusic;
    
};

#endif /* defined(__Tile_Hunter__Configuration__) */
