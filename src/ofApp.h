#pragma once

#include "ofMain.h"
#include "ofxGui.h"

struct AudioClip {
public:
    ofSoundPlayer sound;
    std::string path;
    ofPoint point;
    float t;
    std::string label;
    int color;
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void eLoad();
    void load(std::string file);
    std::map<std::string, int> getColorMap(ofJson js);
    int getColor(std::string label, std::map<std::string, int> colorMap);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    std::vector<AudioClip> sounds;
    
    ofParameter<float> maxDuration;
    ofParameter<float> mouseRadius;
    ofParameter<float> pauseLength;
    ofxButton bLoad;
    ofxPanel gui;
    
    std::string tsnePath;
    bool parsingSuccessful;
};
