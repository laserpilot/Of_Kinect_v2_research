#include "testApp.h"
#include "ofxOpenCv.h"
#include "computeStats.h"



ofTexture tex;

//these are define in protonect.cpp
extern ofPixels pix;
const int SENSOR_W = 1920;
const int SENSOR_H = 1080;

const int DEPTH_W = 512;
const int DEPTH_H = 424;

//the color image is 1920 X 1080; aspect ratio 1.77777777
//the depth image is 512 X 424; aspect ratio 1.2

extern void updateKinect();
extern void closeKinect();
extern ofFloatImage pixDepth;

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
//    ofSetLogLevel(OF_LOG_SILENT);

	ofSetFrameRate(30);
    ofBackground(20, 20, 20);
    
    runKinect2(ofToDataPath(""));

    
    tex.allocate(SENSOR_W, SENSOR_H, GL_RGB);
    
    xOffset = yOffset = 0;
    
    bPrintImageVals = false;
    
    nearThreshold = 0.7;
    farThreshold = 0.5;
    
    bThreshBool = true;
    bIncludePixel = true;
    bBlur = false;
    
    cam.setDistance(500);
    
    bMeshSnapshot = false;
    

    
    plane.set(DEPTH_W, DEPTH_H, DEPTH_W/2, DEPTH_H/2);
    plane.mapTexCoords(0, 0, DEPTH_W, DEPTH_H);

    shader.load("shadersGL3/shader");
    
    bDrawMesh = false;
    
    mode = 0;
    
    bDropPix = false;
    bUseNoiseReduced = false;
    
    numPastDepth = 3;
//    pastDepthPix.assign(numPastDepth, ofFloatPixels());
//    lastDepthFloat.allocate(DEPTH_W, DEPTH_H, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void testApp::update() {
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
//    if ( ofGetFrameNum() % 60 == 0) shader.load("shadersGL3/shader");

    updateKinect();
    
    if(pix.getWidth()){
        tex.loadData(pix);
    }
    
    depthFloat.setFromPixels(pixDepth.getPixelsRef());
    
    // set pixels to 0.0 to 1.0 range
    ofFloatPixelsRef r = depthFloat.getPixelsRef();
    for(int i = 0; i < r.size(); i++){
        r[i] /= 10000.0;
        r[i] = ofClamp(1.0-r[i], 0, 1);
        if(r[i] == 1.0) r[i] =0.0;
    }
    
    // blur image slighty by averaging pixel with neighbours
    if (bBlur) {
        ofFloatPixelsRef temp = depthFloat.getPixelsRef();
        
        if (temp.size() > 0) {
            for(int x = 1; x < DEPTH_W-1; x++){
                for (int y = 1; y < DEPTH_H-1; y++) {
                    int i = y * DEPTH_W + x;
                    int top = (y-1) * DEPTH_W + x;
                    int bottom = (y+1) * DEPTH_W + x;
                    int left = y * DEPTH_W + (x-1);
                    int right = y * DEPTH_W + (x+1);
                    
                    r[i] = bIncludePixel ?
                    (temp[i] + temp[top] + temp[bottom] + temp[left] + temp[right]) * 0.20
                    : (temp[top] + temp[bottom] + temp[left] + temp[right]) * 0.25;
                }
            }
        }
    }
    
    depthFloat.setFromPixels(r);
    /*
    if (r.size() > 0 && lastDepthFloat.isAllocated()) {
        ofFloatPixelsRef lastR = lastDepthFloat.getPixelsRef();
        ofFloatPixels velR; velR.allocate(lastR.getWidth(), lastR.getHeight(), OF_PIXELS_MONO);
        ofFloatPixels noiseReduceR; noiseReduceR.allocate(lastR.getWidth(), lastR.getHeight(), OF_PIXELS_MONO);
        int zeroCount = 0;
        for(int i = 0; i < r.size(); i++){
            velR[i] = abs(r[i] - lastR[i]);
            if (velR[i] == 0) zeroCount++;
            noiseReduceR[i] = velR[i] < 0.003 ? r[i] : (bDropPix ? 0.0 : (r[i] + lastR[i])/2);
        }

        if (zeroCount != velR.size()) {
            velFloat.setFromPixels(velR);
            noiseReducedFloat.setFromPixels(noiseReduceR);
        }
        
        lastNoiseReducedFloat = noiseReducedFloat;
        lastVelFloat = velFloat;
    }
    
    
    lastDepthFloat = depthFloat;*/
    
    if (r.size() > 0) {
        if (pastDepthPix.size() > 0) {
            ofFloatPixels stdDevR;
            stdDevR.allocate(DEPTH_W, DEPTH_H, 1);
            ofFloatPixels noiseReduceR;
            noiseReduceR.allocate(DEPTH_W, DEPTH_H, 1);
            
            int zeroCount = 0;
            float max = 0.0;
            for(int i = 0; i < r.size(); i++){
                vector<float> pixPast;
                float sum = 0.0;
                for (int j = 0; j < pastDepthPix.size(); j++) {
                    pixPast.push_back(pastDepthPix[j][i]);
                    sum += pastDepthPix[j][i];
                }
                float mean = sum / pastDepthPix.size();
                stdDevR[i] = computeStdDev(pixPast.begin(), pixPast.end(), mean);
                
                if (stdDevR[i] == 0) zeroCount++;
                if (stdDevR[i] > max) max = stdDevR[i];
                
                noiseReduceR[i] = stdDevR[i] < 0.001 ? r[i] : (bDropPix ? 0.0 : mean);
            }
            
            cout << "MAX STDEV = " << max << endl;
            
            if (zeroCount != stdDevR.size()) {
                stdDevFloat.setFromPixels(stdDevR);
                noiseReducedFloat.setFromPixels(noiseReduceR);
            }
        }
        
        pastDepthPix.push_back(r);
        if (pastDepthPix.size() > numPastDepth) {
            pastDepthPix.erase(pastDepthPix.begin());
        }
        
//        pastDepthFloats.push_back(depthFloat);
//        if (pastDepthFloats.size() > numPastDepth) {
//            pastDepthFloats.erase(pastDepthFloats.begin());
//        }
    }

    /*
    //threshold image
    for(int i = 0; i < r.size(); i++){
        r[i] = (r[i] > farThreshold && r[i] < nearThreshold) ?
        (bThreshBool ? 1.0 : r[i])
        : 0.0;
    }
    
    threshFloat.setFromPixels(r);
   
    if (r.size() > 0) {
        cvFloatImg.setFromPixels(r);
        cvGrayImg = cvFloatImg;
        contours.findContours(cvGrayImg, 20, (DEPTH_W*DEPTH_H)/3, 10, true);
    }
     */
}

//--------------------------------------------------------------
void testApp::draw() {
    
    ofEnableDepthTest();
    
    ofTexture depthMap = bUseNoiseReduced ? noiseReducedFloat.getTextureReference() : depthFloat.getTextureReference();
    
    shader.begin();
    shader.setUniformTexture("tex0", depthMap, 0);
    {
        cam.begin();
        {
            ofPushMatrix();
            ofRotateZ(180);
            ofTranslate(0,  150, -1000);
            {
                switch (mode) {
                    case 0:
                        plane.draw();
                        break;
                        
                    case 1:
                        plane.drawWireframe();
                        break;
                        
                    case 2:
                        plane.drawVertices();
                        break;
                        
                    default:
                        break;
                }
                
            }
            ofPopMatrix();
        }
        cam.end();
    }
    shader.end();
    
    ofDisableDepthTest();
    
    depthFloat.draw(0, 0);
    stdDevFloat.draw(depthFloat.width+4, 0);
//    velFloat.draw(depthFloat.width+4, 0);
    noiseReducedFloat.draw((depthFloat.width+4)*2, 0);
//    threshFloat.draw(depthFloat.width+4, 0);
//
//    contours.draw((depthFloat.width+4)*2, 0);
    

//    tex.draw(0, 0, 1920.0/1080.0*DEPTH_H, DEPTH_H);
}


//--------------------------------------------------------------
void testApp::exit() {
    closeKinect(); 
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    if (key == OF_KEY_LEFT) {
        xOffset--;
    }
    if (key == OF_KEY_RIGHT) {
        xOffset++;
    }
    if (key == 'd') {
        bPrintImageVals = true;
    }
    if (key == 't') {
        bThreshBool ^= true;
    }
    if (key == 'i') {
        bIncludePixel ^= true;
    }
    if (key == 'b') {
        bBlur ^= true;
    }
    if (key == 's') {
        bMeshSnapshot = true;
    }
    if (key == 'm') {
        mode = (mode+1)%3;
    }
    if (key == 'p') {
        bDropPix ^= true;
    }
    if (key == 'n') {
        bUseNoiseReduced ^= true;
    }

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    nearThreshold = ofMap(x, 0, ofGetWidth(), 0.0, 1.0);
    farThreshold = ofMap(y, 0, ofGetHeight(), 0.0, 1.0);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}


//    depthMesh.enableColors();
//    depthMesh.setMode(OF_PRIMITIVE_POINTS);
//
//    primitiveMode = 0;
//    depthMesh.enableIndices();
//
//    bRainbow = false;

//    if (r.size() > 0) {
//        depthMesh.clear();
//        for(int x = 0; x < DEPTH_W; x++){
//            for (int y = 0; y < DEPTH_H; y++) {
//                int i = y * DEPTH_W + x;
//                if (r[i] > 0.0) {
//                    depthMesh.addVertex(ofVec3f(x,y, r[i]*1000));
//                    ofFloatColor vertexCol;
//                    vertexCol.setHsb((r[i]-0.5) * 2, 1.0, 0.7);
//                    depthMesh.addColor(bRainbow ? vertexCol : ofFloatColor(r[i]));
//                }
//            }
//        }
//
//        for (int y = 0; y<DEPTH_H-1; y++){
//            for (int x=0; x<DEPTH_W-1; x++){
//                depthMesh.addIndex(x+y*DEPTH_W);       // 0
//                depthMesh.addIndex((x+1)+y*DEPTH_W);     // 1
//                depthMesh.addIndex(x+(y+1)*DEPTH_W);     // 10
//
//                depthMesh.addIndex((x+1)+y*DEPTH_W);     // 1
//                depthMesh.addIndex((x+1)+(y+1)*DEPTH_W);   // 11
//                depthMesh.addIndex(x+(y+1)*DEPTH_W);     // 10
//            }
//        }
//        bMeshSnapshot = false;
//    }
