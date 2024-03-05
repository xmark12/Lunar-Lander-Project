
//--------------------------------------------------------------
//
//  Mark John Humilde
//
//  Final Project
// 
//  Project Date: 12/9/2022
// 
//--------------------------------------------------------------

#include "ofApp.h"
#include "Util.h"

//--------------------------------------------------------------
//
//  Setup
// 
//  Description: 
//  Loads cameras, lighting, land, lander, forces, and
//  emitters.
// 
//--------------------------------------------------------------
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = true;
	bTerrainSelected = true;

	// Loads cameras and lighting.
	//
	camSetup();
	lightingSetup();
	soundSetup();

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// Load land, lander, and background image.
	//
	land.loadModel("geo/moon-houdini.obj");
	land.setScaleNormalization(false);

	lander.loadModel("geo/lander.obj");
	lander.setScaleNormalization(false);
	landerPos = lander.getPosition();
	landerRot = lander.getRotationAngle(0);

	background.loadImage("geo/stars.jpg");

	// Slider values. Doesn't actually display; it's just to easily organize all the values.
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(velocitySlide.setup("Initial Velocity", ofVec3f(0, -10, 0), ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100)));
	gui.add(lifespan.setup("Lifespan", 3.0, .1, 10.0));
	gui.add(rate.setup("Rate", 100.0, .5, 100.0));
	gui.add(gravity.setup("Gravity", 4, -1, -20));
	gui.add(minTurbulence.setup("Min Turbulence", ofVec3f(-10, -10, -10), ofVec3f(-50, -50, -50), ofVec3f(0, 0, 0)));
	gui.add(maxTurbulence.setup("Max Turbulence", ofVec3f(10, 10, 10), ofVec3f(0, 0, 0), ofVec3f(50, 50, 50)));
	gui.add(radius.setup("Radius", 1, .1, 10));
	gui.add(radialForceVal.setup("Radial Force", 1000, 100, 5000));
	gui.add(radialHeightVal.setup("Radial Height", 0, 0, 20));
	gui.add(keyLightSpecularRed.setup("Key Light Red Specular Color", 1, 0.00, 10));
	gui.add(keyLightSpecularGreen.setup("Key Light Green Specular Color", 1, 0.00, 10));
	gui.add(keyLightSpecularBlue.setup("Key Light Blue Specular Color", 1, 0.00, 10));
	gui.add(fillLightSpecularRed.setup("Fill Light Red Specular Color", 1, 0.00, 10));
	gui.add(fillLightSpecularGreen.setup("Fill Light Green Specular Color", 1, 0.00, 10));
	gui.add(fillLightSpecularBlue.setup("Fill Light Blue Specular Color", 1, 0.00, 10));
	gui.add(planeMaterialSpecularRed.setup("Plane Red Specular Color", 1, 0.00, 10));
	gui.add(planeMaterialSpecularGreen.setup("Plane Green Specular Color", 1, 0.00, 10));
	gui.add(planeMaterialSpecularBlue.setup("Plane Blue Specular Color", 1, 0.00, 10));
	bHide = true;

	// Create Octree for testing.
	//
	octree.create(land.getMesh(0), 10);

	// Load the landing area.
	//
	landArea = Box(Vector3(-130, 20, -35), Vector3(-120, 30, -25));

	// Load emitters.
	//
	turbForce = new TurbulenceForce(ofVec3f(minTurbulence->x, minTurbulence->y, minTurbulence->z),
		ofVec3f(maxTurbulence->x, maxTurbulence->y, maxTurbulence->z));
	gravityForce = new GravityForce(ofVec3f(0, -gravity, 0));
	dirForce = new DirectionalForce(ofVec3f(sideForce, upForce, frontForce), angularForce);

	emitter.sys->addForce(turbForce);
	emitter.sys->addForce(gravityForce);

	radialForce = new ImpulseRadialForce(1000.0);
	emitter2.sys->addForce(turbForce);
	emitter2.sys->addForce(gravityForce);
	emitter2.sys->addForce(radialForce);

	emitter2.setVelocity(ofVec3f(0, 0, 0));
	emitter2.setOneShot(true);
	emitter2.setEmitterType(RadialEmitter);
	emitter2.setGroupSize(500);

	moveEmitter.sys->addForce(turbForce);
	moveEmitter.sys->addForce(gravityForce);
	moveEmitter.sys->addForce(dirForce);
	moveEmitter.radius = 0;
	moveEmitter.setPosition(ofVec3f(0, 100, 100));
	moveEmitter.setVelocity(ofVec3f(0, -10, 0));
	moveEmitter.setLifespan(-1);
	moveEmitter.setRate(0);
	moveEmitter.setParticleRadius(0);
	moveEmitter.spawn(0);
	moveEmitter.start();

	emitter.start();
}

//--------------------------------------------------------------
void ofApp::camSetup() {
	ofEnableLighting();
	
	followCam.setFov(67.5f);
	followCam.setNearClip(0.1f);

	onboardCam.setFov(90.0f);
	onboardCam.setNearClip(0.1f);

	trackingCam.setFov(22.5f);
	trackingCam.setNearClip(0.1f);
	auto trackAboveLandingArea = glm::vec3(-120, 20, -25);
	trackAboveLandingArea.y += 100.0f;
	trackingCam.setPosition(trackAboveLandingArea);

	freeCam.setFov(90.0f);
	freeCam.setDistance(50.0f);
	freeCam.setNearClip(0.1f);
}

//--------------------------------------------------------------
void ofApp::lightingSetup() {
	// Contains 3 separate lighting types.
	//
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(-115, 25, -20);

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(.05);
	fillLight.setSpotlightCutOff(15);
	fillLight.setAttenuation(2, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(-125, 25, -20);

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(.05);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(-120, 25, -32);

	planeMaterial.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1, 1.0));
	planeMaterial.setDiffuseColor(ofFloatColor(0.3, 0.3, 0.3, 1.0));
	planeMaterial.setShininess(5);
}
//--------------------------------------------------------------
void ofApp::soundSetup() {
	thrusterSound.load("sounds/thruster.wav"); //can be mp3 or wav
	thrusterSound.setLoop(false); //sets loop, set to false for stuff like gun shots
	thrusterSound.setVolume(0.3f); //sets volume to 50% of maximum, 0.0 is silent and 1.0 is full

	landerDead.load("sounds/dead.wav"); //can be mp3 or wav
	landerDead.setLoop(false); //sets loop, set to false for stuff like gun shots
	landerDead.setVolume(0.5f); //sets volume to 50% of maximum, 0.0 is silent and 1.0 is full

	winSound.load("sounds/win.wav"); //can be mp3 or wav
	winSound.setLoop(false); //sets loop, set to false for stuff like gun shots
	winSound.setVolume(0.5f); //sets volume to 50% of maximum, 0.0 is silent and 1.0 is full

	backgroundSound.load("sounds/wind.mp3"); //can be mp3 or wav
	backgroundSound.setLoop(true); //sets loop, set to false for stuff like gun shots
	backgroundSound.setVolume(0.3f); //sets volume to 50% of maximum, 0.0 is silent and 1.0 is full
}

//--------------------------------------------------------------
//
//  Update
// 
//  Description: 
//  Updates cameras, lighting, toggles on and off physics
//  simulation. Physics simulation contains updating position,
//  rotation, all values from setup, and collision.
// 
//--------------------------------------------------------------
void ofApp::update() {
	// Update cameras.
	//
	updateCameras();

	// Update lights.
	//
	keyLight.setSpecularColor(ofFloatColor(keyLightSpecularRed, keyLightSpecularGreen, keyLightSpecularBlue));
	fillLight.setSpecularColor(ofFloatColor(fillLightSpecularRed, fillLightSpecularGreen, fillLightSpecularBlue));
	planeMaterial.setSpecularColor(ofFloatColor(planeMaterialSpecularRed, planeMaterialSpecularGreen, planeMaterialSpecularBlue, 1.0));

	if (!backgroundSound.isPlaying())
		backgroundSound.play();

	// If simulation, then start physics.
	//
	if (simulationToggle) {
		// Upon a new game, reset all values.
		//
		if (newGame) {
			moveEmitter.sys->particles.at(0).position = glm::vec3(0, 100, 100);
			moveEmitter.sys->particles.at(0).rotation = 0;
			fuel = 12000;
			newGame = false;
		}

		// Update position, rotation, and emitters.
		//
		dirForce->set(ofVec3f(sideForce, upForce, frontForce) + heading(), angularForce);

		lander.setPosition(moveEmitter.sys->particles.at(0).position.x,
			moveEmitter.sys->particles.at(0).position.y,
			moveEmitter.sys->particles.at(0).position.z);
		lander.setRotation(1, moveEmitter.sys->particles.at(0).rotation, 0, 1, 0);

		emitter.setPosition(ofVec3f(moveEmitter.sys->particles.at(0).position.x,
			moveEmitter.sys->particles.at(0).position.y,
			moveEmitter.sys->particles.at(0).position.z));
		emitter.setVelocity(ofVec3f(velocitySlide));

		moveEmitter.setLifespan(-1);
		moveEmitter.setRate(0);
		moveEmitter.setParticleRadius(0);
		moveEmitter.spawn(0);
		moveEmitter.update();

		if (thrusterIsOn) {
			emitter.setLifespan(float(lifespan));
			emitter.setRate(float(rate));
			emitter.setParticleRadius(float(radius));
		}
		else if (!thrusterIsOn) {
			emitter.setLifespan(0);
			emitter.setRate(0);
			emitter.setParticleRadius(0);
		}
		emitter.update();

		if (!collide || !died || !youWon)
			turbForce = new TurbulenceForce(ofVec3f(minTurbulence->x, minTurbulence->y, minTurbulence->z),
				ofVec3f(maxTurbulence->x, maxTurbulence->y, maxTurbulence->z));
		else if (collide || died || youWon)
			turbForce = new TurbulenceForce(ofVec3f(0, 0, 0), ofVec3f(0, 0, 0));

		if (!collide || !died || !youWon)
			gravityForce = new GravityForce(ofVec3f(0, -gravity, 0));
		else if (collide || died || youWon)
			gravityForce = new GravityForce(ofVec3f(0, 0, 0));

		radialForce->set(radialForceVal, radialHeightVal);
		emitter2.setPosition(ofVec3f(moveEmitter.sys->particles.at(0).position.x,
			moveEmitter.sys->particles.at(0).position.y,
			moveEmitter.sys->particles.at(0).position.z));
		emitter2.setLifespan(lifespan);
		emitter2.setVelocity(ofVec3f(100, 100, 100));
		emitter2.setRate(rate);
		emitter2.setParticleRadius(radius);

		emitter2.update();

		bLanderSelected = true;

		// If collide, then set a boolean to true. Otherwise false.
		//
		if (colBoxList.size() > 0) {
			collide = true;
		}
		else if (colBoxList.size() == 0) {
			collide = false;
		}

		// Sound effect for thruster and win.
		//
		if (thrusterIsOn && !thrusterSound.isPlaying())
			thrusterSound.play();
		else if (!thrusterIsOn && thrusterSound.isPlaying())
			thrusterSound.stop();

		if (youWon && !winSound.isPlaying())
			winSound.play();
		else if (!youWon && winSound.isPlaying())
			winSound.stop();

		// Movement based on keymaps. Uses forces.
		//
		if (keymap['W'] || keymap['w'] || keymap[OF_KEY_UP]) {
			if (fuel > 0 && colBoxList.size() == 0) {
				frontForce -= 0.1;
				fuel -= 1;
				thrusterIsOn = true;
			}
			else if (fuel <= 0) {
				thrusterIsOn = false;
			}
			if (collide) {
				thrusterIsOn = false;
			}
		}
		if (keymap['A'] || keymap['a'] || keymap[OF_KEY_LEFT]) {
			if (fuel > 0 && colBoxList.size() == 0) {
				sideForce -= 0.1;
				fuel -= 1;
				thrusterIsOn = true;
			}
			else if (fuel <= 0) {
				thrusterIsOn = false;
			}
			if (collide) {
				thrusterIsOn = false;
			}
		}
		if (keymap['S'] || keymap['s'] || keymap[OF_KEY_DOWN]) {
			if (fuel > 0 && colBoxList.size() == 0) {
				frontForce += 0.1;
				fuel -= 1;
				thrusterIsOn = true;
			}
			else if (fuel <= 0) {
				thrusterIsOn = false;
			}
			if (collide) {
				thrusterIsOn = false;
			}
		}
		if (keymap['D'] || keymap['d'] || keymap[OF_KEY_RIGHT]) {
			if (fuel > 0 && colBoxList.size() == 0) {
				sideForce += 0.1;
				fuel -= 1;
				thrusterIsOn = true;
			}
			else if (fuel <= 0) {
				thrusterIsOn = false;
			}
			if (collide) {
				thrusterIsOn = false;
			}
		}
		if (keymap[' '] && !youWon && !died) {
			if (fuel > 0) {
				upForce += 0.3;
				fuel -= 1;
				thrusterIsOn = true;
			}
			else if (fuel <= 0) {
				thrusterIsOn = false;
			}

			if (collide)
				thrusterIsOn = false;
		}
		if ((keymap['P'] && (youWon || died)) || (keymap['p'] && (youWon || died))) {
			youWon = false;
			died = false;
			newGame = true;
		}
		if (keymap['Q'] || keymap['q']) {
			if (fuel > 0) {
				angularForce += 1;
			}
			else if (fuel <= 0) {

			}
			if (collide)
				moveEmitter.sys->particles.at(0).angularVelocity = 0;
		}
		if (keymap['E'] || keymap['e']) {
			if (fuel > 0) {
				angularForce -= 1;
			}
			else if (fuel <= 0) {

			}
			if (collide)
				moveEmitter.sys->particles.at(0).angularVelocity = 0;
		}

		//------------------------------------------------------------------------------------
		// Checks for intersection so that collide can work.
		//
		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
		//------------------------------------------------------------------------------------

		// Everything below is for collision.
		//
		if ((collide && !bounds.overlap(landArea) && ((moveEmitter.sys->particles.at(0).velocity.y > 1) || (moveEmitter.sys->particles.at(0).velocity.y < -1))) ||
			(moveEmitter.sys->particles.at(0).position.y < -10 && ((moveEmitter.sys->particles.at(0).velocity.y > 1) || (moveEmitter.sys->particles.at(0).velocity.y < -1)))) {
			died = true;
			youWon = false;
			thrusterIsOn = false;

			// Sound effect for crash landing.
			//
			if (!landerDead.isPlaying())
				landerDead.play();
			else if (landerDead.isPlaying())
				landerDead.stop();
		}

		if ((collide && !bounds.overlap(landArea) && ((moveEmitter.sys->particles.at(0).velocity.y > 1) || (moveEmitter.sys->particles.at(0).velocity.y < -1))) ||
			(moveEmitter.sys->particles.at(0).position.y < -10 && ((moveEmitter.sys->particles.at(0).velocity.y > 1) || (moveEmitter.sys->particles.at(0).velocity.y < -1)))) {
			emitter2.sys->reset();
			emitter2.start();
			moveEmitter.sys->particles.at(0).velocity = glm::vec3(0, 0, 0);
			moveEmitter.sys->particles.at(0).angularVelocity = 0;
		}

		if (!bounds.overlap(landArea) && collide && !keymap[' ']) {
			thrusterIsOn = false;
			moveEmitter.sys->particles.at(0).velocity = glm::vec3(0, 0, 0);
			moveEmitter.sys->particles.at(0).angularVelocity = 0;
		}
		else if (collide && bounds.overlap(landArea) && ((moveEmitter.sys->particles.at(0).velocity.y < 5) || (moveEmitter.sys->particles.at(0).velocity.y > -5))) {
			moveEmitter.sys->particles.at(0).velocity = glm::vec3(0, 0, 0);
			youWon = true;
			moveEmitter.sys->particles.at(0).angularVelocity = 0;
		}
		else if (collide && bounds.overlap(landArea) && ((moveEmitter.sys->particles.at(0).velocity.y > 5) || (moveEmitter.sys->particles.at(0).velocity.y < -5))) {
			died = true;
			youWon = false;
			thrusterIsOn = false;
			emitter2.sys->reset();
			emitter2.start();
			moveEmitter.sys->particles.at(0).velocity = glm::vec3(0, 0, 0);
			moveEmitter.sys->particles.at(0).angularVelocity = 0;

			if (!landerDead.isPlaying())
				landerDead.play();
			else if (landerDead.isPlaying())
				landerDead.stop();
		}
	}
}

//--------------------------------------------------------------
void ofApp::updateCameras() {
	const auto landerPosition = lander.getPosition();

	followCam.orbitDeg(moveEmitter.sys->particles.at(0).rotation+0.0f, -45.0f, 25.0f,
		landerPosition);
	onboardCam.orbitDeg(moveEmitter.sys->particles.at(0).rotation+0.0f, 270.0f, 0.7f,
		landerPosition);
	trackingCam.lookAt(landerPosition);
}

//--------------------------------------------------------------
//
//  Draw
// 
//  Description: 
//  Draws the background, begin camera, begin lights, loads
//  the land and lander, draw emitters, and draws GUI on-
//  screen.
// 
//--------------------------------------------------------------
void ofApp::draw() {
	glDepthMask(false);
	ofSetColor(ofColor::white);
	// Draws background.
	//
	background.draw(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
	if (!bHide) gui.draw();
	glDepthMask(true);

	currentCam->begin();

	ofPushMatrix();

	ofEnableLighting();
	planeMaterial.begin();
	land.drawFaces();
	planeMaterial.end();
	ofNoFill();

	// Draws landing area.
	//
	ofDrawCylinder(ofVec3f(-120, 20, -25), 5, 1);

	ofFill();
	ofMesh mesh;
	if (bLanderLoaded) {
		// If the lander hasn't exploded, then draw the lander and thruster emitter.
		//
		if (!died) {
			lander.drawFaces();
			emitter.draw();
			moveEmitter.draw();
		}
		// Else, draw explode emitter.
		else {
			emitter2.draw();
		}
		if (!bTerrainSelected) drawAxis(lander.getPosition());
		if (bDisplayBBoxes) {
			ofNoFill();
			ofSetColor(ofColor::white);
			for (int i = 0; i < lander.getNumMeshes(); i++) {
				ofPushMatrix();
				ofMultMatrix(lander.getModelMatrix());
				ofRotate(-90, 1, 0, 0);
				Octree::drawBox(bboxList[i]);
				ofPopMatrix();
			}
		}

		if (bLanderSelected) {

			ofVec3f min = lander.getSceneMin() + lander.getPosition();
			ofVec3f max = lander.getSceneMax() + lander.getPosition();

			Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}

	ofDisableLighting();

	ofPopMatrix();
	currentCam->end();

	// GUI Strings.
	//
	if (guiEnabled) {
		ofSetColor(ofColor::white);
		string velocityString;
		velocityString += "Velocity: " + std::to_string(int(abs(moveEmitter.sys->particles.at(0).velocity.y))) + " m/s";
		ofDrawBitmapString(velocityString, ofPoint(ofGetWindowWidth() / 2.2, 20));
		string fuelString;
		fuelString += "Fuel: " + std::to_string(int(fuel / 100)) + " second(s) remaining";
		ofDrawBitmapString(fuelString, ofPoint(ofGetWindowWidth() / 2.2, 40));
		string altitudeString;
		if (altitudeTriggered)
			altitudeString += "Altitude: " + std::to_string(int(moveEmitter.sys->particles.at(0).position.y)) + " meters";
		ofDrawBitmapString(altitudeString, ofPoint(ofGetWindowWidth() / 2.2, 60));
		string simulationString;
		if (simulationToggle) {
			simulationString = "Simulation: On";
		}
		else if (!simulationToggle) {
			simulationString = "Simulation: Off";
			ofDrawBitmapString("Simulation is Off. Press P to start simulation.", ofPoint(ofGetWindowWidth() / 2.4, ofGetWindowHeight() / 2));
		}
		ofDrawBitmapString(simulationString, ofPoint(ofGetWindowWidth() / 2.2, 80));
		ofDrawBitmapString("Can mouse drag in freecam while simulation is off", ofPoint(ofGetWindowWidth() / 2.4, 100));
		ofDrawBitmapString("and if the free cam is locked.", ofPoint(ofGetWindowWidth() / 2.2, 120));

		ofDrawBitmapString("Tips:", ofPoint(ofGetWindowWidth() - 300, 20));
		ofDrawBitmapString("WASD to move", ofPoint(ofGetWindowWidth() - 300, 40));
		ofDrawBitmapString("Spacebar to move up", ofPoint(ofGetWindowWidth() - 300, 60));
		ofDrawBitmapString("Rotate lander with Q or E", ofPoint(ofGetWindowWidth() - 300, 80));
		ofDrawBitmapString("1 for follow cam", ofPoint(ofGetWindowWidth() - 300, 100));
		ofDrawBitmapString("2 for onboard cam", ofPoint(ofGetWindowWidth() - 300, 120));
		ofDrawBitmapString("3 for tracking cam", ofPoint(ofGetWindowWidth() - 300, 140));
		ofDrawBitmapString("4 for free cam", ofPoint(ofGetWindowWidth() - 300, 160));
		ofDrawBitmapString("C to lock cam during free cam", ofPoint(ofGetWindowWidth() - 300, 180));
		ofDrawBitmapString("X to see altitude", ofPoint(ofGetWindowWidth() - 300, 200));
		ofDrawBitmapString("Land on the landing zone to win", ofPoint(ofGetWindowWidth() - 300, 220));

		string ifDied;
		string ifWin;
		if (youWon) {
			ifWin += "You Won! Press P to play again.";
			ofDrawBitmapString(ifWin, ofPoint(ofGetWindowWidth() / 2.4, ofGetWindowHeight() / 2));
		}
		if (died) {
			ifDied += "You Died. Press P to play again.";
			ofDrawBitmapString(ifDied, ofPoint(ofGetWindowWidth() / 2.4, ofGetWindowHeight() / 2));
		}
	}
}

//--------------------------------------------------------------
//
//  Draw Axis
// 
//  Description: 
//  Draw an XYZ axis in RGB at world (0,0,0) for reference.
//  Provided by Kevin Smith.
// 
//--------------------------------------------------------------
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}

//--------------------------------------------------------------
//
//  Key Pressed
// 
//  Description: 
//  Keybindings for non-movement options, such as the
//  camera type and toggling simulation.
// 
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	keymap[key] = true;
	switch (key) {
	case 'B':
	case 'b':
		break;
	case 'C':
	case 'c':
		if (currentCam == &freeCam) {
			if (freeCam.getMouseInputEnabled()) {
				freeCam.disableMouseInput();
			}
			else {
				freeCam.enableMouseInput();
			}
		}
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'I':
	case 'i':
		break;
	case 'H':
	case 'h':
		guiEnabled = !guiEnabled;
		break;
	case 'L':
	case 'l':
		break;
	case 'O':
	case 'o':
		break;
	case 'r':
		break;
	case 't':
		break;
	case 'u':
		break;
	case 'v':
		break;
	case 'V':
		break;
	case 'P':
	case 'p':
		simulationToggle = !simulationToggle;
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case 'X':
	case 'x':
		altitudeTriggered = !altitudeTriggered;
		break;
	case '1':
		currentCam = &followCam;
		break;
	case '2':
		currentCam = &onboardCam;
		break;
	case '3':
		currentCam = &trackingCam;
		break;
	case '4':
		currentCam = &freeCam;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
//
//  Key Released
// 
//  Description: 
//  Releases keybinds.
// 
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
	thrusterIsOn = false;
	upForce = 0.0;
	frontForce = 0.0;
	sideForce = 0.0;
	angularForce = 0.0;
	switch (key) {
	
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case 'E':
	case 'e':
	case 'Q':
	case 'q':
		angularForce = 0.0;
		break;
	default:
		break;

	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {


}

//--------------------------------------------------------------
//
//  Mouse Pressed
// 
//  Description: 
//  Used primarily to click on the lander
//  via a ray intersect.
// 
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (freeCam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = freeCam.getPosition();
		glm::vec3 mouseWorld = freeCam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), freeCam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
}

//--------------------------------------------------------------
//
//  Mouse Dragged
// 
//  Description: 
//  Used primarily to drag around the lander.
// 
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (freeCam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, freeCam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;

		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}

//--------------------------------------------------------------
void ofApp::setCameraTarget() {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
//
//  Get Mouse Point On Plane
// 
//  Description: 
//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.
//  Provided by Kevin Smith.
// 
//--------------------------------------------------------------
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = freeCam.getPosition();
	glm::vec3 camAxis = freeCam.getZAxis();
	glm::vec3 mouseWorld = freeCam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
