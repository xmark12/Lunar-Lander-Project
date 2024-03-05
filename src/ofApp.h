#pragma once

//--------------------------------------------------------------
//
//  Mark John Humilde
//
//  Final Project
// 
//  Project Date: 12/9/2022
// 
//--------------------------------------------------------------

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include <glm/gtx/intersect.hpp>
#include "vector3.h"

//--------------------------------------------------------------
//
//  ofApp.h
// 
//  Description: 
//  Primarily used to initialize variables and functions.
// 
//--------------------------------------------------------------
class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);

		void camSetup();
		void updateCameras();
		void setCameraTarget();

		void lightingSetup();

		void soundSetup();

		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		ofCamera* currentCam = &followCam;
		ofCamera followCam;
		ofCamera onboardCam;
		ofCamera trackingCam;
		ofEasyCam freeCam;

		ofxAssimpModelLoader land, lander;
		ofLight light;
		Box boundingBox, landerBounds, marsBounds;
		Box landArea;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;

		ofxIntSlider numLevels;
		ofxPanel gui;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		bool thrusterIsOn = false;
		bool fuelState = true;
		bool simulationToggle = false;
		bool died = false;
		bool restartGame = false;
		bool youWon = false;
		bool newGame = false;
		bool collide = false;
		bool altitudeTriggered = false;
		bool guiEnabled = true;

		int fuel = 12000;
		
		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;

		map<int, bool> keymap;
		glm::vec3 landerPos;
		int landerRot;

		glm::vec3 heading() {
			glm::vec3 rotate(cos(moveEmitter.sys->particles.at(0).rotation), 0, sin(moveEmitter.sys->particles.at(0).rotation));
			return (glm::normalize(rotate));
		}

		ofxVec3Slider minTurbulence;
		ofxVec3Slider maxTurbulence;

		ofxFloatSlider gravity;
		ofxFloatSlider radius;
		ofxVec3Slider velocitySlide;
		ofxFloatSlider lifespan;
		ofxFloatSlider rate;
		ofxFloatSlider radialForceVal;
		ofxFloatSlider radialHeightVal;

		ParticleEmitter emitter;
		ParticleEmitter emitter2;
		ParticleEmitter moveEmitter;
		TurbulenceForce *turbForce;
		GravityForce *gravityForce;
		DirectionalForce *dirForce;
		ImpulseRadialForce *radialForce;
		float upForce = 0.0;
		float frontForce = 0.0;
		float sideForce = 0.0;
		float angularForce = 0.0;

		ofLight keyLight, fillLight, rimLight;
		ofxFloatSlider keyLightSpecularRed;
		ofxFloatSlider keyLightSpecularGreen;
		ofxFloatSlider keyLightSpecularBlue;

		ofxFloatSlider fillLightSpecularRed;
		ofxFloatSlider fillLightSpecularGreen;
		ofxFloatSlider fillLightSpecularBlue;

		ofxFloatSlider rimLightSpecularRed;
		ofxFloatSlider rimLightSpecularGreen;
		ofxFloatSlider rimLightSpecularBlue;

		ofxFloatSlider planeMaterialSpecularRed;
		ofxFloatSlider planeMaterialSpecularGreen;
		ofxFloatSlider planeMaterialSpecularBlue;

		ofMaterial planeMaterial;
		ofImage background;

		ofSoundPlayer thrusterSound;
		ofSoundPlayer landerDead;
		ofSoundPlayer winSound;
		ofSoundPlayer backgroundSound;
};
