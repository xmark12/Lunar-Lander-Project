#pragma once

#include "ofMain.h"

class ParticleForceField;

class Particle {
public:
	Particle();

	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f forces;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;
	float   rotation;
	float   angularForce;
	float   angularVelocity;
	float   angularAccleration;
	void    integrate();
	void    draw();
	float   age();        // sec
	ofColor color;
};


