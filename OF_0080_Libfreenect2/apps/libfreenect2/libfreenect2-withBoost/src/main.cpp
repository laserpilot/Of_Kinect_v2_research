#include "testApp.h"
#include "ofGLProgrammableRenderer.h"

int main() {
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL((512+4)*3, 1024, OF_WINDOW);
	ofRunApp(new testApp());
}
