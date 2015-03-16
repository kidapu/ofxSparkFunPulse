//
//  ofxSparkFunPulse.h
//
//  Created by Tatsuya Kida on 3/1/15.
//

#pragma once

#include "ofMain.h"

class ofxSparkFunPulse : protected ofThread
{
public:
    
    // vector of current voltage reads (y-axis)
    int ibl;
    int bpm;
    vector<int> signals;
    
    // constructor
    ~ofxSparkFunPulse();
    
    // if empty, tries to find usbserial
    bool setup( string name = "" );
    
    // update datas
    void update();
    
    // draw graph + peak
    void drawRawDatas( int x, int y, ofColor color = ofColor(0) );
    
private:
    //------------------------------------------
    // y-axis adjustment
    float adjustYAxis(float signal, float graph_scale, float signal_ave)
    {
        return ofGetHeight()*0.5 - (signal - signal_ave) * graph_scale + 200;
    }

protected:
    // deletegate method of ofThread
    void threadedFunction();
    
    //
    ofSerial _serial;
    
    // about signal
    int _pre_signal = 0;
    float _signal_average = 0;
    float _graph_scale = 0;
        
    // about bpm
    long long _preZeroCrossTime = 0;
    float _my_bpm;
    
    
    //-------//
    // literals
    //-------//
    // The size of the buffer array
    static const int _packet_size = 8;

    // Highest sensor read (for graphing)
    static const int _max_read = 340; //800; //50
    
    // arduino baud rate
    static const int _baundrate = 115200;
    
    // static const int _peak_x_min = 30;
    static const int _min_signal = 400;
    
};




