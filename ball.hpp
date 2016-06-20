//
//  ball.hpp
//  SimpleMotionBuffer
//
//  Created by Mauritius Seeger on 20/06/2016.
//
//

#ifndef ball_hpp
#define ball_hpp

#include <stdio.h>
#include "ofMain.h"

class ball
{
    
public:
    
    void init(int w, int h);
    void motion(ofPixels& th);
    void update();
    void draw(float ox, float oy, float sx, float sy);

private:
    
    // size of the bounce area
    int _w;
    int _h;
    
    // position
    float _x;
    float _y;
    
    //velocity
    float _vx;
    float _vy;
    
    //acceleration
    float _ax;
    float _ay;
    
    //radius
    float _rad;
};


#endif /* ball_hpp */
