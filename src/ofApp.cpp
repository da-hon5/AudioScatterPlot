#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
    
    // this should point to the json file containing your audio files and tsne coordinates
    //TODO: use relative path!
    tsnePath = ofToDataPath("/home/hons/Desktop/instrument-recognition/src/t-sne/points.json");
    
    bLoad.addListener(this, &ofApp::eLoad);
    gui.setup();
    gui.setName("Audio t-SNE");
    gui.add(maxDuration.set("maxDuration", 1.0, 0.1, 2.0));
    gui.add(mouseRadius.set("mouseRadius", 20, 5, 300));
    gui.add(pauseLength.set("pauseLength", 2.0, 0.2, 5.0));
    gui.add(bLoad.setup("Load json-file"));

    load(tsnePath);
}

//--------------------------------------------------------------
void ofApp::eLoad() 
{
    ofFileDialogResult result = ofSystemLoadDialog("Select a json-file!",
    						 false, 
    						 "/home/hons/Desktop/instrument-recognition/src/t-sne");
    if (result.bSuccess) 
        load(result.filePath);
}

//--------------------------------------------------------------
void ofApp::load(string filename) 
{
    tsnePath = filename;

    ofJson js;
    ofFile file(tsnePath);
    parsingSuccessful = file.exists();
    
    if (!parsingSuccessful) 
    {
        ofLog(OF_LOG_ERROR) << "parsing not successful";
        return;
    }

    sounds.clear();

    file >> js;
    
    std::map<std::string, int> colorMap = getColorMap(js);

    ofVec2f minPoint(1e8, 1e8);
    ofVec2f maxPoint(-1e8, -1e8);
    for (auto & entry: js) 
    {
        if(!entry.empty()) 
        {
            std::string path = entry["path"];
            std::string label = entry["label"];
            float x = entry["point"][0];
            float y = entry["point"][1];
            minPoint.x = min(minPoint.x, x);
            minPoint.y = min(minPoint.y, y);
            maxPoint.x = max(maxPoint.x, x);
            maxPoint.y = max(maxPoint.y, y);
            AudioClip newSound;
            newSound.path = path;
            //newSound.sound.load(path); //TODO: only 256 sounds can be loaded !!!
            newSound.point.set(x, y);
            newSound.t = 0;
            newSound.label = label;
            newSound.color = getColor(label, colorMap);
            sounds.push_back(newSound);
        }
    }
    
    // normalize the points
    for (int i=0; i<sounds.size(); i++) 
    {
        sounds[i].point.set(ofMap(sounds[i].point.x, minPoint.x, maxPoint.x, 0.02, 0.98), // change back to range 0 - 1
                            ofMap(sounds[i].point.y, minPoint.y, maxPoint.y, 0.02, 0.98));
        printf("%.6f, %.6f\n", sounds[i].point.x, sounds[i].point.y); // just for debugging
    }
}

//--------------------------------------------------------------
std::map<std::string, int> ofApp::getColorMap(ofJson js)
{
    // remove duplicate labels
    std::vector<std::string> labels;
    for (auto & entry: js) 
    {
        if(!entry.empty()) 
            labels.push_back(entry["label"]);
    }

    std::sort(labels.begin(), labels.end());
    auto last = std::unique(labels.begin(), labels.end());
    labels.erase(last, labels.end());

    // create colorMap (https://www.color-hex.com/)
    std::vector<int> colors{0x0001f6, //blue
                            0xff0020, //red
                            0x40ff40, //green
                            0xff6000, //orange
                            0xff66c0, //pink
                            0xfffe00, //yellow
                            0xffeead}; //skin color

    std::map<std::string, int> colorMap;
    for (int i=0; i<labels.size(); i++)
    {
        colorMap[labels[i]] = colors[i];
    }

    return colorMap;
}

//--------------------------------------------------------------
int ofApp::getColor(std::string label, std::map<std::string, int> colorMap)
{
    int color = colorMap[label];
    return color;
}

//--------------------------------------------------------------
// This function gets called repeatedly. It gets called just before draw.
void ofApp::update()
{
    if (!parsingSuccessful) 
        return;
    
    for (int i=0; i<sounds.size(); i++) 
    {
        if (sounds[i].sound.isPlaying() && sounds[i].sound.getPositionMS() > maxDuration*1000) 
        {
            //sounds[i].sound.stop();
            sounds[i].sound.unload();
        }
    }
    ofSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackgroundGradient(ofColor(100), ofColor(20));
    if (!parsingSuccessful) 
    {
        ofDrawBitmapString("Could not find file "+tsnePath+"\nSee the instructions for how to create one.", 50, 50);
        return;
    }

    for (int i=0; i<sounds.size(); i++) 
    {
        if (sounds[i].sound.isPlaying()) 
            ofSetColor(0, 0, 0);
        else 
            ofSetHexColor(sounds[i].color);
            
        ofDrawCircle(ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y, 4);
    }
    //TODO: draw a legend here!
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
    for (int i=0; i<sounds.size(); i++) 
    {
        float distanceToMouse = ofDistSquared(x, y, ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y);
        if (distanceToMouse < mouseRadius && !sounds[i].sound.isPlaying() && (ofGetElapsedTimef() - sounds[i].t > pauseLength)) 
        {
            sounds[i].t = ofGetElapsedTimef();
            sounds[i].sound.load(sounds[i].path);  //loads sound on the fly
            sounds[i].sound.play();
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
