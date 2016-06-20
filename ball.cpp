//
//  ball.cpp
//  SimpleMotionBuffer
//
//  Created by Mauritius Seeger on 20/06/2016.
//
//

#include "ball.hpp"

// this initialises the values
// and sets the size of the bounce area
// this must be in pixels and be the same
// as the frame difference image used for motion detection
void ball::init(int w, int h)
{
    //set width and height
    _w = w;
    _h = h;
    
    // initialise acceleration
    _ax = 0;
    _ay = 0.05;
    
    // initialise position (centre of area)
    _x = w/2;
    _y = h/2;
    
    
    // initialise the velocity (something random)
    _vx = -3.4;
    _vy = 3.5;
    
    // the radius
    _rad = 50;
}


// this takes the thresholded motion image
// and determines the acceleration of the ball
void ball::motion(ofPixels& th)
{
    // get the width and height of the image
    int h = th.getHeight();
    int w = th.getWidth();
    
    // make sure size of our bound area is the
    // same as that of the frame difference image
    if(h != _h || w != _w)
    {
        return;
    }
    
    float sumx = 0;
    float sumy = 0;
    
    // there the x (c) and y (r) position is important
    // so we go row and column wise throught the image
    for(int r = 0 ; r < h; r++)
    {
        for(int c = 0 ; c < w; c++)
        {
            //distance of this pixel to centre of ball
            float dx = c - _x; // in x direction
            float dy = r - _y; // in y direction
            
            // using pythagoras i.e c2 = a2 + b2
            // find all pixels lying withing the ball area
            // (notice there is no need to take the square root! this would be slow
            // and we always want to avoid that!)
            if(dx*dx + dy*dy < _rad*_rad)
            {
                //if the pixels inside the ball are also motion
                //pixels add them to x and x sum
                if(th[r*w+c] > 128)
                {
                    sumx+=dx;
                    sumy+=dy;
                    
                    // if motion pixels are predominantly at the bottom the circle 
                }
                
            }
        }
    }
    
    //update acceleration

    _ax = -sumx / 10000.0;
    _ay = 0.1 /*add gravity term*/ - sumy / 10000.0;
}

// the updates the ball dynamics
void ball::update()
{
    // update the velocity with acceleration
    _vx += _ax;
    _vy += _ay;
    
    // update position with velocity
    _x += _vx;
    _y += _vy;
    
    // damping: reduce the velocity slowly
    // this mimics air and contact friction
    _vx *= 0.99;
    _vy *= 0.99;
    
    //check boundaries
    
    if(_x >= _w)
    {
        _x = _w - 1;  //set back inside area
        _vx = -_vx;    //reverse velocity
    }
    
    if(_x < 0)
    {
        _x = 0;
        _vx = -_vx;
    }
    
    if(_y >= _h)
    {
        _y = _h - 1;
        _vy = -_vy;
    }
    
    if(_y < 0)
    {
        _y = 0;
        _vy = -_vy;
    }
}

// draw the ball
// offsets the area by ox and oy
// and scales the size by sx and sy
void ball::draw(float ox, float oy, float sx, float sy)
{
    ofSetColor(255,255,255,180);
    ofDrawCircle(ox+ _x * sx,oy+_y * sy,50);
    
    ofSetColor(255,0,0);
    ofSetLineWidth(5);
    ofDrawLine(ox+ _x * sx,oy+_y * sy,ox+ _x * sx + _ax * 30,oy+_y * sy + _ay*30);
}
