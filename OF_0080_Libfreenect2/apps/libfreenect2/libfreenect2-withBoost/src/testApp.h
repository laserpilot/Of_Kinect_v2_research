#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxUI.h"

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
    
    //gui
    void setupGUI();
    
    
    //image video proccessing
    void scaleVals(ofFloatPixels &r);
    void threshHold(ofFloatPixels &r);
    void meanFilter(ofFloatPixels &r);
    void medianFilterS(ofFloatPixels &r);
    void weinerFilter(ofFloatPixels &r);
    void stdDevFilter(ofFloatPixels &r);
    void medianFilterT(ofFloatPixels &r);
    
    //mesh functions
    void drawMesh();
    void exportMesh(ofFloatPixels &r);
	
	//events
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    
	//kinect2!!!!!
	threadedKinect tKinect;
    ofFloatImage depthFloat;
    
    //thresholds
    float nearThreshold, farThreshold;
    bool bThreshBool;
    
    //spatial filtering
    bool bIncludePixel;
    bool bMean, bMedianS, bMedianT;
    
    //weiner filter
    ofFloatImage wiener2float;
    ofFloatImage sampleForSpectrum;
    int stddev_noise;
    ofImage spectrumDraw;
    
    //stdDev time filter
    vector<ofFloatPixels> pastDepthPix;
    int numPastDepth;
    float stdDevThresh;
    bool bDropPix, bUseStdDev;
    ofFloatImage stdDevFloat, noiseReducedFloat;
    
    //median time filter
    ofFloatImage mediaTFloat;
    
    //potentialy for rgb depth calibration
    float xOffset, yOffset;
    
    //cv contour finder
    ofxCvContourFinder contours;
    ofxCvFloatImage cvFloatImg;
    ofxCvGrayscaleImage cvGrayImg;
    

    //mesh
    ofEasyCam cam;
    ofPlanePrimitive plane;
    ofShader shader;
    ofMesh depthMesh;
    int mode;
    float zScale;
    bool bMeshSnapshot;
    bool bDrawMesh;

    //gui
    ofxUISuperCanvas * gui;
    
    
    //debug
    bool bPrintImageVals;
    
};
