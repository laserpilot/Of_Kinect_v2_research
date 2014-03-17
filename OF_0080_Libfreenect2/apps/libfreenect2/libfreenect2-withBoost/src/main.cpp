#include "testApp.h"

int main() {
    ofSetupOpenGL((512+4)*3, 1024, OF_WINDOW);
	ofRunApp(new testApp());
}
