#pragma once

#include "ofMain.h"
#include "ofxSparkFunPulse.h"

class ofApp : public ofBaseApp
{

public:
    
    // vars
    ofxSparkFunPulse _pulse;
    
    void setup()
    {
        _pulse.setup();
    }
    
    void update()
    {
        _pulse.update();
    }
    
    void draw()
    {
        ofBackground(20);
        
        
        //------//
        // fps
        //------//
        stringstream ss;
        ss << "Display FPS: " << ofToString(ofGetFrameRate(), 0);
        ofDrawBitmapString(ss.str(), 10, 20);
        
        
        //------//
        // draw
        //------//
        _pulse.drawRawDatas(0, -100, ofColor(255, 0, 0));
    }
    

    void keyPressed(int key)
    {
        cout << key << endl;
        if(key == 'f')
        {
            ofToggleFullscreen();
        }
    }
//		void keyReleased(int key);
//		void mouseMoved(int x, int y );
//		void mouseDragged(int x, int y, int button);
//		void mousePressed(int x, int y, int button);
//		void mouseReleased(int x, int y, int button);
//		void windowResized(int w, int h);
//		void dragEvent(ofDragInfo dragInfo);
//		void gotMessage(ofMessage msg);
		
};
