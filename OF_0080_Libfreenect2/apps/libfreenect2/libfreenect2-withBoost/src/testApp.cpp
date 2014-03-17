#include "testApp.h"
#include "ofxOpenCv.h"



ofTexture tex;

//these are define in protonect.cpp
extern ofPixels pix;
int SENSOR_W = 1920;
int SENSOR_H = 1080;


extern void updateKinect();
extern void closeKinect();
extern ofFloatImage pixDepth;

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
//    ofSetLogLevel(OF_LOG_SILENT);

	ofSetFrameRate(60);
    ofBackground(20, 20, 20);
    
    runKinect2(ofToDataPath(""));

    
    tex.allocate(SENSOR_W, SENSOR_H, GL_RGB);
    
    xOffset = yOffset = 0;
    
    bPrintImageVals = false;
    
    nearThreshold = 0.7;
    farThreshold = 0.5;
    
    bThreshBool = true;
    bIncludePixel = true;
    bBlur = true;
    
    cam.setDistance(500);
    
    bMeshSnapshot = false;
    
    depthMesh.enableColors();
    depthMesh.setMode(OF_PRIMITIVE_POINTS);
    
//    depthMesh.enableIndices();
}

//--------------------------------------------------------------
void testApp::update() {
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    updateKinect();
    
    if(pix.getWidth()){
        tex.loadData(pix);
    }
    
    //the color image is 1920 X 1080; aspect ratio 1.77777777
    //the depth image is 512 X 424; aspect ratio 1.2
    
    depthFloat.setFromPixels(pixDepth.getPixelsRef());
    
    // set pixels to 0.0 to 1.0 range
    ofFloatPixelsRef r = depthFloat.getPixelsRef();
    for(int i = 0; i < r.size(); i++){
        r[i] /= 10000.0;
        r[i] = ofClamp(1.0-r[i], 0, 1);
        if(r[i] == 1.0) r[i] =0.0;
    }
    
    if (r.size() > 0 && bMeshSnapshot) {
        depthMesh.clear();
        for(int x = 0; x < 512; x++){
            for (int y = 0; y < 424; y++) {
                int i = y * 512 + x;
                depthMesh.addVertex(ofVec3f(x,y, r[i]*1000));
                depthMesh.addColor(ofFloatColor(r[i]));
            }
        }
        
//        for (int y = 0; y<424-1; y++){
//            for (int x=0; x<512-1; x++){
//                depthMesh.addIndex(x+y*512);       // 0
//                depthMesh.addIndex((x+1)+y*512);     // 1
//                depthMesh.addIndex(x+(y+1)*512);     // 10
//                
//                depthMesh.addIndex((x+1)+y*512);     // 1
//                depthMesh.addIndex((x+1)+(y+1)*512);   // 11
//                depthMesh.addIndex(x+(y+1)*512);     // 10
//            }
//        }
//        bMeshSnapshot = false;
    }
    /*
    if (bPrintImageVals) cout << r.size() << endl; bPrintImageVals = false;
    
    // blur image slighty by averaging pixel with neighbours
    if (bBlur) {
        ofFloatPixelsRef temp = depthFloat.getPixelsRef();
    
        if (temp.size() > 0) {
            for(int x = 1; x < 511; x++){
                for (int y = 1; y < 423; y++) {
                    int i = y * 512 + x;
                    int top = (y-1) * 512 + x;
                    int bottom = (y+1) * 512 + x;
                    int left = y * 512 + (x-1);
                    int right = y * 512 + (x+1);

                    r[i] = bIncludePixel ?
                    (temp[i] + temp[top] + temp[bottom] + temp[left] + temp[right]) * 0.20
                    : (temp[top] + temp[bottom] + temp[left] + temp[right]) * 0.25;
                }
            }
        }
    }
    
    depthFloat.setFromPixels(r);
    
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
        contours.findContours(cvGrayImg, 20, (512*424)/3, 10, true);
    }
*/
}

//--------------------------------------------------------------
void testApp::draw() {

//    tex.draw(depthFloat.getWidth() + 4, 0, 192*4, 108*4);
//    ofSetColor(255);
//    tex.draw(0, 0, 1920.0/1080.0*424, 424);
    
    cam.begin();
    ofPushMatrix();
    ofTranslate(-512/2, -424/2);
    depthMesh.draw();
    ofPopMatrix();
    cam.end();
    
//    depthFloat.draw(0, 0);
//    threshFloat.draw(depthFloat.width+4, 0);
//
//    contours.draw((depthFloat.width+4)*2, 0);
    

//    tex.draw(0, 108*4 + 4, 192*4, 108*4);
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
