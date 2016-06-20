#pragma once

#include "ofMain.h"
#include "ball.hpp"

class ofApp : public ofBaseApp
{

	public:
    
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    private: // this makes the following member variables private
    
        int             _camWidth;
        int             _camHeight;
    
        ofVideoGrabber  _vidGrabber;
    
    
        ofTexture       _videoTexture;
        ofTexture       _cameraTexture;
        ofTexture       _greyTexture;
        ofTexture       _prevTexture;
        ofTexture       _diffTexture;
        ofTexture       _thTexture;
    
        ofPixels        _cameraImage;
        ofPixels        _greyImage;
        ofPixels        _prevGreyImage;
        ofPixels        _difImage;
        ofPixels        _thImage;
    
        ball            _ball;
    
        int             _threshold;
        bool            _displayDebug;
        bool            _mirror;
    
};
