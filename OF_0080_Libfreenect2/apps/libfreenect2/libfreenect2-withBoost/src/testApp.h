#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Protonect.h"

#include "ofxUI.h"

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
    
    void setupGUI();
    
    void scaleVals(ofFloatPixels &r);
    void threshHold(ofFloatPixels &r);
    void meanFilter(ofFloatPixels &r);
    void medianFilter(ofFloatPixels &r);
    void stdDevFilter(ofFloatPixels &r);
		
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	threadedKinect tKinect;
    ofFloatImage depthFloat, threshFloat, lastDepthFloat;
    vector<ofFloatImage> pastDepthFloats;
    vector<ofFloatPixels> pastDepthPix;
    int numPastDepth;
    float stdDevThresh;
    
    ofFloatImage velFloat, lastVelFloat, stdDevFloat;
    ofFloatImage noiseReducedFloat, lastNoiseReducedFloat;
    
    float nearThreshold, farThreshold;
    
    bool bPrintImageVals;
    
    float xOffset, yOffset, xScale, yScale;
    
    ofxCvContourFinder contours;
    ofxCvFloatImage cvFloatImg;
    ofxCvGrayscaleImage cvGrayImg;
    
    bool bThreshBool;
    bool bIncludePixel;
    bool bMean, bMedian;

//    ofMesh depthMesh;
    ofEasyCam cam;
    
    bool bMeshSnapshot;
    
    ofPlanePrimitive plane;
    ofShader shader;
    
    bool bDrawMesh;
    
    int mode;
    
    bool bDropPix;
    bool bUseNoiseReduced;
    bool bUseStdDev;
    
    ofxUISuperCanvas * gui;
};
