#include "testApp.h"

#include "computeStats.h"
#include "weiner2.h"

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
    ofBackground(25, 25, 25);
    
    runKinect2(ofToDataPath(""));

    tex.allocate(SENSOR_W, SENSOR_H, GL_RGB);

    //thresholding
    nearThreshold = 1.0;
    farThreshold = 0.9;
    bThreshBool = false;
    
    //spatial filtering
    bIncludePixel = true;
    bMean = bMedianSCPU = bMedianSGPU = bMedianT = false;
    
    medianFilter.load("shadersGL3/median filter");
    
    medianFilterRender.set(DEPTH_W, DEPTH_H, 10, 10);
    medianFilterRender.mapTexCoords(0, 0, DEPTH_W, DEPTH_H);
    
    medianFilteredDepth.allocate(DEPTH_W, DEPTH_H);
    medianFilteredDepth.begin();
    ofClear(0);
    medianFilteredDepth.end();
    
    //stdev time filter
    numPastDepth = 4;
    stdDevThresh = 0.003;
    bDropPix = false;
    
    //weiner filter
    wiener2float.allocate(512, 432, OF_IMAGE_GRAYSCALE);
    stddev_noise = 50;
    ofImage sample;
    sample.loadImage("sample.bmp");
    sampleForSpectrum = sample;
    
    //calibration
//    colorCalibration.load("kinect2color.yml");
    depthCalibration.setFillFrame(true);
    depthCalibration.load("calibration/kinect2depth.yml");
    
//    depthUndistorted.allocate(DEPTH_W, DEPTH_H, OF_IMAGE_GRAYSCALE);
//    imitate(depthUndistorted, pixDepth);
    
    //mesh
    cam.setDistance(500);
    RGBD.load("shadersGL3/rgbd");

    plane.set(DEPTH_W, DEPTH_H, DEPTH_W/2, DEPTH_H/2);
    plane.mapTexCoords(0, 0, DEPTH_W, DEPTH_H);

    bMeshSnapshot = false;
    bDrawMesh = false;
    
    mode = 0;
    zScale = 1000;
    
    //gui
    setupGUI();

    //debug
    bPrintImageVals = false;

    depthUndistorted.allocate(DEPTH_W, DEPTH_H, OF_IMAGE_COLOR);
    depthGrayscale.allocate(DEPTH_W, DEPTH_H, OF_IMAGE_COLOR);
    
    xOffset = 250;
    xScale = 1520;
}

//--------------------------------------------------------------
void testApp::update() {
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    if ( ofGetFrameNum() % 60 == 0) medianFilter.load("shadersGL3/median filter");

//    updateKinect();
    
    if(pix.getWidth()){
        tex.loadData(pix);
        colorImage.setFromPixels(pix);
    }
    
    
    depthFloat.setFromPixels(pixDepth.getPixelsRef());
    ofFloatPixelsRef r = depthFloat.getPixelsRef();
    

//    if (r.size() > 0) {
//        
//        unsigned char grayscalePix[DEPTH_W * DEPTH_H * 3];
//        for (int i = 0; i < DEPTH_W * DEPTH_H; i++) {
//            grayscalePix[i*3] = (unsigned char)(r[i]*255);
//            grayscalePix[i*3+1] = (unsigned char)(r[i]*255);
//            grayscalePix[i*3+2] = (unsigned char)(r[i]*255);
//        }
//    //
//        depthUndistorted.setFromPixels(grayscalePix, DEPTH_W, DEPTH_H, OF_IMAGE_COLOR);
//        depthUndistorted.update();
////        depthCalibration.undistort(ofxCv::toCv(depthGrayscale), ofxCv::toCv(depthUndistorted));
////        depthUndistorted.update();
////        depthUndistorted = depthGrayscale;
//    }
    
    scaleVals(r);
    threshHold(r);
    
    //adaptive filtering
//    weinerFilter(r);
    
    if (bMean) {
        meanFilter(r);
    }
    
    if (bMedianSCPU) {
        medianFilterCPU(r);
    }
    
    depthFloat.setFromPixels(r);
    
    if (bUseStdDev ) {
        stdDevFilter(r);
    }
    
    pastDepthPix.push_back(r);
    if (pastDepthPix.size() > numPastDepth) {
        pastDepthPix.erase(pastDepthPix.begin());
    }
    
    if (bMedianT) {
        medianFilterT(r);
    }
    
    if (bMeshSnapshot) {
        exportMesh(r);
    }

//    if (r.size() > 0) {
//        cvFloatImg.setFromPixels(r);
//        cvGrayImg = cvFloatImg;
//        contours.findContours(cvGrayImg, 20, (DEPTH_W*DEPTH_H)/3, 10, true);
//    }
}

//--------------------------------------------------------------
void testApp::draw() {
    ofSetColor(255);
    if (bMedianSGPU) medianFilterGPU();
    if (bDrawMesh) drawMesh();

//    if (bMedianS) {
//    }
//    depthUndistorted.draw(200+depthFloat.width+4, 0);
//    spectrumDraw.draw(depthFloat.width+4, 0);
//    wiener2float.draw((depthFloat.width+4)*2, 0);
    
    depthFloat.draw(200, 0);
    if (bMedianSGPU) medianFilteredDepth.draw(200+ depthFloat.width+4, 0);
    
    if (bUseStdDev) {
        stdDevFloat.draw(depthFloat.width+4, 0);
        noiseReducedFloat.draw((depthFloat.width+4)*2, 0);
    }
    
    if (bMedianT) mediaTFloat.draw(depthFloat.width+4, 0);
    
//    threshFloat.draw(depthFloat.width+4, 0);
//
//    contours.draw((depthFloat.width+4)*2, 0);
//    tex.draw(0, 0, 1920.0/1080.0*DEPTH_H, DEPTH_H);
}

//--------------------------------------------------------------
void testApp::setupGUI(){
    gui = new  ofxUISuperCanvas("BOOM.");
    gui->addSpacer();
    gui->addLabel("THRESHOLD");
    gui->addRangeSlider("range", 0.0, 1.0, &farThreshold, &nearThreshold);
    gui->addToggle("Boolean", &bThreshBool);
    gui->addSpacer();
    gui->addLabel("SPATIAL");
    gui->addToggle("Linear Filter", &bMean);
    gui->addToggle("Median Filter CPU", &bMedianSCPU);
    gui->addToggle("Median Filter GPU", &bMedianSGPU);
    gui->addIntSlider("noise stddev", 1, 100, &stddev_noise);
    gui->addSpacer();
    gui->addLabel("TIME");
    gui->addIntSlider("n past values", 2, 10, &numPastDepth);
    gui->addToggle("StdDev Filter", &bUseStdDev);
    gui->addToggle("drop/avg", &bDropPix);
    gui->addSlider("stdDev thresh", 0.0, 0.5, &stdDevThresh);
    gui->addToggle("Median Filter", &bMedianT);
    gui->addSpacer();
    gui->addLabel("MESH");
    gui->addToggle("draw mesh", &bDrawMesh);
    gui->addSlider("Z scale", 1, 2000, &zScale);
    gui->addIntSlider("render mode", 0, 2, &mode);
    gui->addSlider("xOffset", 0, 900, &xOffset);
    gui->addSlider("xScale", 1, 1920, &xScale);
    
    gui->autoSizeToFitWidgets();
}



//--------------------------------------------------------------
void testApp::scaleVals(ofFloatPixels &r) {
    for(int i = 0; i < r.size(); i++){
        r[i] /= 10000.0;
        r[i] = ofClamp(1.0-r[i], 0, 1);
        if(r[i] == 1.0) r[i] =0.0;
    }
}

//--------------------------------------------------------------
void testApp::threshHold(ofFloatPixels &r){
    for(int i = 0; i < r.size(); i++){
        r[i] = (r[i] > farThreshold && r[i] < nearThreshold) ?
        (bThreshBool ? 1.0 : r[i])
        : 0.0;
    }
}

//--------------------------------------------------------------
void testApp::meanFilter(ofFloatPixels &r){
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

//--------------------------------------------------------------
void testApp::medianFilterCPU(ofFloatPixels &r){
    ofFloatPixelsRef temp = depthFloat.getPixelsRef();
    
    if (temp.size() > 0) {
        for(int x = 1; x < DEPTH_W-1; x++){
            for (int y = 1; y < DEPTH_H-1; y++) {
                vector<float> neighbors;
                neighbors.push_back(r[y * DEPTH_W + x]);
                neighbors.push_back(r[(y-1) * DEPTH_W + x]);
                neighbors.push_back(r[(y+1) * DEPTH_W + x]);
                neighbors.push_back(r[y * DEPTH_W + (x-1)]);
                neighbors.push_back(r[y * DEPTH_W + (x+1)]);
                
                ofSort(neighbors);
                
                r[y * DEPTH_W + x] = neighbors[2];
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::medianFilterGPU(){
    medianFilteredDepth.begin();
    {
        ofClear(0);
        medianFilter.begin();
        medianFilter.setUniformTexture("tex0", depthFloat.getTextureReference(), 0);
        {
            ofPushMatrix();
            ofTranslate(DEPTH_W/2, DEPTH_H/2);
            medianFilterRender.draw();
            ofPopMatrix();
        }
        medianFilter.end();
    }
    medianFilteredDepth.end();
}



//--------------------------------------------------------------
void testApp::weinerFilter(ofFloatPixels &r){
    if(r.size() > 0){
        Mat I;
        I = ofxCv::toCv(depthFloat);
        
        Mat raw_sample;
        raw_sample = ofxCv::toCv(sampleForSpectrum);
        // Mat padded = padd_image(raw_sample);
        Mat sample(raw_sample.rows, raw_sample.cols, CV_8U);
        Mat spectrum = get_spectrum(sample);
        
        //        Mat paddedI = padd_image(I);
        
        Mat enhanced = wiener2(I, spectrum, 5);
        
        
        if (bPrintImageVals) {
            for (int i = 0; i < 512 * 432; i++) {
                cout << spectrum.data[i] << " ";
            }
            bPrintImageVals = false;
        }
        
        Mat floatEnhanced(enhanced.rows, enhanced.cols, CV_32F);
        //         Mat spectrumFla(spectrum.rows, spectrum.cols, CV_32F);
        
        ofxCv::toOf(floatEnhanced, wiener2float);
        
        ofxCv::toOf(raw_sample, spectrumDraw);
        wiener2float.update();
    }
}

//--------------------------------------------------------------
void testApp::stdDevFilter(ofFloatPixels &r){
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
                    
                noiseReduceR[i] = stdDevR[i] < stdDevThresh ? r[i] : (bDropPix ? 0.0 : mean);
            }
            
            cout << "MAX STDEV = " << max << endl;
            
            if (zeroCount != stdDevR.size()) {
                stdDevFloat.setFromPixels(stdDevR);
                noiseReducedFloat.setFromPixels(noiseReduceR);
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::medianFilterT(ofFloatPixels &r){
    
    if (r.size() > 0) {
        if (pastDepthPix.size() == numPastDepth) {
            
            ofFloatPixels medianTPix;
            medianTPix.allocate(DEPTH_W, DEPTH_H, 1);
            
            for (int i = pastDepthPix.size() % 2; i < r.size(); i++) {
                
                vector<float> pastPixVals;
                for (int j = 0; j < pastDepthPix.size(); j++) {
                    pastPixVals.push_back(pastDepthPix[j][i]);
                }
                
                pastPixVals.push_back(r[i]);
                ofSort(pastPixVals);
                
                medianTPix[i] = pastPixVals[pastDepthPix.size()/2+1];
            }
            
            mediaTFloat.setFromPixels(medianTPix);
        }
    }
    
}

//--------------------------------------------------------------
void testApp::drawMesh(){
    ofEnableDepthTest();
    
//    ofTexture depthMap = bUseStdDev ? noiseReducedFloat.getTextureReference() : depthFloat.getTextureReference();
    
    ofTexture depthMap = bMedianSGPU ? medianFilteredDepth.getTextureReference() : depthFloat.getTextureReference();
    
    RGBD.begin();
    RGBD.setUniformTexture("tex0", depthMap, 0);
    RGBD.setUniformTexture("tex1", tex, 1);
    RGBD.setUniform1f("scale", zScale);
    RGBD.setUniform1f("xOffset", xOffset);
    RGBD.setUniform1f("xScale", xScale);
    {
        cam.setDistance(zScale + 500);
        cam.begin();
        {
            ofPushMatrix();
            ofRotateZ(180);
            ofTranslate(0,  150, 0);
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
    RGBD.end();
    
    ofDisableDepthTest();
}

//--------------------------------------------------------------
void testApp::exportMesh(ofFloatPixels &r) {
    depthMesh.clear();
    depthMesh.enableColors();
    depthMesh.enableIndices();
    
    if (r.size() > 0) {
        depthMesh.clear();
        for (int y = 0; y < DEPTH_H; y++) {
            for(int x = 0; x < DEPTH_W; x++){
                int i = y * DEPTH_W + x;
                //                    if (r[i] > 0.0) {
                depthMesh.addVertex(ofVec3f(x,y, r[i]*500));
                //                    ofFloatColor vertexCol;
                //                    vertexCol.setHsb((r[i]-0.5) * 2, 1.0, 0.7);
                depthMesh.addColor(ofFloatColor(r[i]));
                //                    }
            }
        }
        //
        for (int y = 0; y<DEPTH_H-1; y++){
            for (int x=0; x<DEPTH_W-1; x++){
                depthMesh.addIndex(x+y*DEPTH_W);
                depthMesh.addIndex((x+1)+y*DEPTH_W);
                depthMesh.addIndex(x+(y+1)*DEPTH_W);
                
                depthMesh.addIndex((x+1)+y*DEPTH_W);
                depthMesh.addIndex((x+1)+(y+1)*DEPTH_W);
                depthMesh.addIndex(x+(y+1)*DEPTH_W);
            }
        }
    }
    
    depthMesh.save(ofGetTimestampString() + ".ply", true);
    bMeshSnapshot = false;

}

//--------------------------------------------------------------
void testApp::exit() {
    closeKinect();
}


//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    if (key == 't') {
        bThreshBool ^= true;
    }
    if (key == 'i') {
        bIncludePixel ^= true;
    }
    if (key == 'b') {
        bMean ^= true;
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
    if (key == 'd') {
        bUseStdDev ^= true;
    }
    if (key == ' ') {
        bPrintImageVals = true;
    }

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}



