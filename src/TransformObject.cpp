#pragma once
#include "TransformObject.h"

//  Base class for any object that needs a transform.
//
TransformObject::TransformObject() {
	position = ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0);
	scale = ofVec3f(10, 10, 10);
	rotation = 0;
}

void TransformObject::setPosition(const ofVec3f & pos) {
	position = pos;
}
