//
//  ofxSparkFunPulse.cpp
//
//  Created by Tatsuya Kida on 3/1/15.
//

#include "ofxSparkFunPulse.h"

using Poco::RegularExpression;

//------------------------------------------
ofxSparkFunPulse::~ofxSparkFunPulse()
{
    stopThread();
    _serial.close();
}

//------------------------------------------
bool ofxSparkFunPulse::setup( string name )
{
    // search for available arduino serial port
    vector <ofSerialDeviceInfo> info = _serial.getDeviceList();
    bool serialSetupFlag = false;
    
    
    // search serial port.
    if ( name == "" )
    {
        for ( int i = 0; i < info.size(); i++)
        {
            if ( info[i].getDeviceName().find("tty.usbmodem") == string::npos )
            {
                continue;
            }
            
            serialSetupFlag = _serial.setup( info[i].getDevicePath(), _baundrate );
            cout << " setting up " << info[i].getDevicePath() << " " << serialSetupFlag << endl;
            break;
        }
    }
    // open port with argument name.
    else
    {
        serialSetupFlag = _serial.setup( name, _baundrate );
    }
    
    
    // fail to connect serial.
    if ( !serialSetupFlag )
    {
        return false;
    }
    
    
    // start new thread for recieving serial values.
    cout << "start thread" << endl ;
    startThread(); //
    
    return serialSetupFlag;
}


//------------------------------------------
void ofxSparkFunPulse::update()
{
    if ( signals.size() == 0 )
    {
        return;
    }
    
    
    // vars
    unsigned long long elapsedTime = ofGetElapsedTimeMillis();
    
    
    //----------//
    // calcurating average, max, min values from heartbeat waveform.
    //----------//
    int len = signals.size();
    float signal_sum = 0;
    float min = signals[0];
    float max = signals[0];
    for ( int i = 0; i < len; i+=1)
    {
        // sum
        signal_sum += signals[i];
        
        // max
        if( max < signals[i] ){ max = signals[i]; }
        
        // min
        if( signals[i] < min ){ min = signals[i]; }
    }
    _signal_average = signal_sum / len;
    
    
    // calcurate scale of graph
//    cout << _signal_average << endl;
    float max_abs_val = ( max - _signal_average < _signal_average - min ) ? _signal_average - min : max - _signal_average;
    _graph_scale = (ofGetHeight() * 0.5) * 0.7 / max_abs_val  ;
    
    
    
    //----------//
    // TODO : calcurating bpm with zero-cross method.
    //----------//
//    if( 1 < len
//       && signals[len-2] < _signal_average + 10
//       && _signal_average + 10 <= signals[len - 1])
//    {
//        _my_bpm  = 60 / ( (elapsedTime - _preZeroCrossTime) / 1000 );
//        _preZeroCrossTime = elapsedTime;
//        
//        cout << "my bpm:" << _my_bpm << endl;
//    }
    
    
}


void ofxSparkFunPulse::drawRawDatas( int x, int y, ofColor color )
{
    
    if ( signals.size() == 0)
    {
        return;
    }
    
    
    //----------//
    // draw
    //----------//
    ofPushMatrix();
    ofTranslate(x, y);
    ofNoFill();
    
    
    // draw average line
//    ofSetColor(255,255,255);
//    ofSetLineWidth(5);
//    ofBeginShape();
//    float line_y = adjustYAxis(_signal_average , _graph_scale, _signal_average) - 30;
//    ofLine(0, line_y, ofGetWidth(), line_y);
//    ofEndShape();
    
    
    // グラフを描画
    ofSetColor(color);
    ofSetLineWidth(5);
    ofBeginShape();
    int len = signals.size();
    for ( int i = 0; i < len; i++)
    {
        ofCurveVertex( i*4.0 - 400, adjustYAxis(signals[i], _graph_scale, _signal_average) );
    }
    ofEndShape();
    
    
    
    
    //----------//
    // display parameters
    //----------//
    stringstream ss;
    ss << "bpm : " << bpm << endl;
//    ss << "my bpm : " << _my_bpm << endl;
    ss << "ibl : " << ibl << endl;
    ofSetColor(255);
    ofDrawBitmapString(ss.str(), 10, 140);
    
    
    ofPopMatrix();
}





//------------------------------------------
// delegata method of ofThread
void ofxSparkFunPulse::threadedFunction()
{
    string numStr = "";
    
    while( isThreadRunning() )
    {
        // A count of how many bytes received
        int serialCount = 0;
        
        
        while ( 0 < _serial.available() )
        {
            //
            unsigned char readBytes[1];
            memset(readBytes, 0, 1);
            _serial.readBytes(readBytes, 1);
            string readString = string((char*)readBytes);
            
            //
            RegularExpression reg1("[A-Z]+");
            RegularExpression reg2("[0-9]+");
            
            // command
            if( reg1.match(readString) )
            {
//                cout <<  readString << ":" << numStr << endl;
                
                
                switch(readBytes[0])
                {
                    case 'S':
                    {
                        int signal;
                        signal = ofToInt(numStr);
                        
                        //
                        if( signal < _min_signal )
                        {
                            signal = _pre_signal;
                        }
                        else
                        {
                            // easy low-pass filter
                            signal = _pre_signal * 0.5 + signal * 0.5;
                        }
                        
                        //
                        lock();
                        signals.push_back( signal );
                        _pre_signal = signal;
                        
                        if( _max_read < signals.size() )
                        {
                            signals.erase( signals.begin() );
                        }
                        unlock();
                        break;
                    }
                    case 'B':
                        lock();
                        bpm = ofToInt(numStr);
                        unlock();
                        break;
                        
                    case 'Q':
                        lock();
                        ibl = ofToInt(numStr);
                        unlock();
                        break;
                }
                
                
                // clear
                numStr = "";
            }
            
            // number
            if( reg2.match(readString) )
            {
                numStr = numStr + readString;
            }
        }
        
    }
}


