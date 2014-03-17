#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Protonect.h"

extern int runKinect2(std::string binpath);

class threadedKinect : public ofThread{

    void threadedFunction(){
        runKinect2(ofToDataPath(""));
    }
};

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
		
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	threadedKinect tKinect;
    ofFloatImage depthFloat, threshFloat;
    
    float nearThreshold, farThreshold;
    
    
    bool bPrintImageVals;
    
    float xOffset, yOffset, xScale, yScale;
    
    ofxCvContourFinder contours;
    ofxCvFloatImage cvFloatImg;
    ofxCvGrayscaleImage cvGrayImg;
    
    bool bThreshBool;
    bool bIncludePixel;
    bool bBlur;

//    ofMesh depthMesh;
    
};
