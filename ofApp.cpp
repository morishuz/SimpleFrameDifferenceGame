#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

    // this is the resolution of the camera we would like
    // currently this assumes the camera supports this
    
    _camWidth  = 640;
    _camHeight = 480;
    
    //we can now get back a list of devices.
    vector<ofVideoDevice> devices = _vidGrabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    _vidGrabber.setDeviceID(0);
    _vidGrabber.setDesiredFrameRate(60);
    _vidGrabber.initGrabber(_camWidth, _camHeight);
    
    // these are textures and we need this only to draw  the images
    _videoTexture.allocate(_camWidth, _camHeight, OF_PIXELS_BGR);           // for video camera image (colour)
    _cameraTexture.allocate(_camWidth, _camHeight, OF_PIXELS_BGR);          // for mirrored camera image (colour)
    _greyTexture.allocate(_camWidth, _camHeight, OF_PIXELS_GRAY_ALPHA);     // grey scale version of mirrored camera image
    _prevTexture.allocate(_camWidth, _camHeight, OF_PIXELS_GRAY_ALPHA);     // previous grey frame
    _diffTexture.allocate(_camWidth, _camHeight, OF_PIXELS_GRAY_ALPHA);     // difference image between current and previous
    _thTexture.allocate(_camWidth, _camHeight, OF_PIXELS_GRAY_ALPHA);       // thresholded difference
    
    // these are ofPixel images that store the pixels in normal CPU memory
    _cameraImage.allocate(_camWidth, _camHeight,3);
    _greyImage.allocate(_camWidth, _camHeight,1);
    _prevGreyImage.allocate(_camWidth, _camHeight,1);
    _difImage.allocate(_camWidth, _camHeight,1);
    _thImage.allocate(_camWidth, _camHeight,1);
    
    // initialise our ball image
    // (something to try - modify the software to draw lots of these balls - this is fairly simple!)
    _ball.init(_camWidth,_camHeight);
    
    
    // flags:
    _displayDebug   = false;  // do we display the images showing steps in the processing (toggle with any key
    _mirror         = true;   // do we mirror the input image? (change with key 'M')
    _threshold      = 30;     // the grey scale value that we use to threshold the difference image (change with up / down arrow)
}

//--------------------------------------------------------------
void ofApp::update()
{
    ofBackground(100, 100, 100);
    _vidGrabber.update();
    
    if(_vidGrabber.isFrameNew())
    {
        // these are just making references so we can get shorter variable names
        // pointing to the same image - not strictly necessary
        ofPixels & vid    = _vidGrabber.getPixels();
        ofPixels & cam    = _cameraImage;
        ofPixels & grey   = _greyImage;
        ofPixels & prev   = _prevGreyImage;
        ofPixels & diff   = _difImage;
        ofPixels & th     = _thImage;

        // make some intermediate variables with shorter names for width, height and number of colours
        int ch =  vid.getNumChannels(); // how many colours does this image have?
        int w  =  vid.getWidth();
        int h  =  vid.getHeight();
        
        
        // copy camera image
        // mirror
        
        if(_mirror)
        {
            for(int r = 0; r < h; r++) //cycle through all rows (y direction)
            {
                for(int c = 0; c < w; c++) // cycle throught all columns (x direction)
                {
                    // for colour image the index for pixel at row r and column c is:
                    // (r*w + w-c-1)*ch for Blue
                    // (r*w + w-c-1)*ch for Green
                    // (r*w + w-c-1)*ch for Red
                    // (assuming BGR ordering, not RGB)
                    
                    // here we swap pixels at column c with those at w-c-1
                    // effectively creating a mirror image
                    cam[(r*w + c)*ch]    = vid[(r*w + w-c-1)*ch];
                    cam[(r*w + c)*ch+1]  = vid[(r*w + w-c-1)*ch+1];
                    cam[(r*w + c)*ch+2]  = vid[(r*w + w-c-1)*ch+2];
                }
            }
        }
        else
        {
            // if mirroring is switched off just copy the pixels
            for(int i = 0; i < vid.size(); i++)
            {
                cam[i]  = vid[i];
            }
        }
        
        
        // average colours to make once channel
        // grey scale image
        
        for(int i = 0; i < grey.size(); i++) //cam.size is ch times larger than grey.size()!
        {
            int sum  = cam[i*ch] + cam[i*ch+1]  + cam[i*ch+2]; //add RBG values
            grey[i]  = sum/3; //then take the average and assign to output
        }
        
        
        // subtract
        // from previous frame
        for(int i = 0; i < grey.size(); i++)
        {
            diff[i]  = abs(grey[i] - prev[i]);
        }
        
        // threshold the difference
        for(int i = 0; i < grey.size(); i++)
        {
            if(diff[i] > _threshold)
            {
                th[i] = 255;
            }
            else th[i]  =0;
        }
        
        //copy current to previous
        for(int i = 0; i < grey.size(); i++)
        {
            prev[i]  = grey[i];
        }
    
        // set textures to the accutal pixels values we have just updated
        _videoTexture.loadData(vid);
        _cameraTexture.loadData(cam);
        _greyTexture.loadData(grey);
        _prevTexture.loadData(prev);
        _diffTexture.loadData(diff);
        _thTexture.loadData(th);

    }

}


//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetHexColor(0xffffff);
    
    // the size we want the images to
    // be displayed at (not the size of the images in pixels!)
    int w = 320;
    int h = 240;
    
    if(_displayDebug)
    {
        
        _cameraTexture.draw(20 + w, 20, w, h);
        _videoTexture.draw(20 , 20, w, h);
        _greyTexture.draw(20 , 20+h, w,h);
        _prevTexture.draw(20 + w, 20+h, w, h);
        _diffTexture.draw(20, 20+2*h, w, h);
        _thTexture.draw(20 + w, 20+2*h, w, h);
    }
    else
    {
        _cameraTexture.draw(20 , 20, 3 * w, 3 * h);
        
        _ball.motion(_thImage); // update ball velocity using our motion image
        _ball.update();         // update position
        
        // actually draw position of the ball
        // with offset
        // and scale (we scaled the image display so we need to scale our display too
        _ball.draw(20,20,3*320 / (float)_camWidth,3*240 / (float)_camHeight);

    }
    
  }

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    // handle keyboard input
    if(key == 'm' || key == 'M')
    {
        _mirror = !_mirror;
    }
    else if(key == OF_KEY_UP)
    {
        _threshold++;
        if(_threshold >= 255) _threshold = 255;
    }
    else if(key == OF_KEY_DOWN)
    {
        _threshold--;
        if(_threshold <= 0) _threshold = 0;
    }
    else
    {
        _displayDebug = !_displayDebug;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
