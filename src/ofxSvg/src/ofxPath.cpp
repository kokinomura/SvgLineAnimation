//
//  ofxPath.cpp
//  lineMotion_RoundJoint
//
//  Created by Koki Nomura on 2014/07/22.
//
//

#include "ofMain.h"
#include "ofxPath.h"

//------------------------------------------------------------
void ofxPolyline::draw() {
    draw(0, size()-1);
}

//------------------------------------------------------------
void ofxPolyline::draw(int begin, int end) {
    
    if (begin >= end) {
        return;
    }
    
    // Create a mesh of polyline
    ofMesh mesh;
    createMesh(mesh, begin, end);
    
    // Draw
    for (int i=0; i<2; i++) {
        // For statement for ofNoFill() and ofFill()
        if (i == 0) {
            // No fill for antialias
            ofNoFill();
            mesh.drawWireframe();
        } else if (i == 1) {
            // Fill
            ofFill();
            mesh.draw();
        }
        
        // Draw ends of line
        if (strokeLinecap == OFXSVG_STROKE_LINECAP_BUTT) {
            ;
        } else if (strokeLinecap == OFXSVG_STROKE_LINECAP_ROUND) {
            ofCircle((*this)[begin], width/2);
            ofCircle((*this)[end], width/2);
        } else if (strokeLinecap == OFXSVG_STROKE_LINECAP_SQUARE) {
            ofPushStyle();
            ofSetRectMode(OF_RECTMODE_CENTER);
            for (int j=0; j<2; j++) {
                int index;
                float capXOffset;
                if (j == 0) {
                    index = begin;
                    capXOffset = -width/4.0;
                } else if (j == 1) {
                    index = end;
                    capXOffset = width/4.0;
                }
                ofVec3f rotation3 = getNormalAtIndex(index);
                ofVec2f rotation = ofVec2f(rotation3);
                rotation.rotate(90);  // Normal to line direction
                float angle = ofVec2f(1.0, 0.0).angle(rotation);
                ofPushMatrix();
                ofTranslate((*this)[index].x, (*this)[index].y);
                ofRotate(angle);
                ofRect(capXOffset, 0, width/2, width);
                ofPopMatrix();
            }
            ofPopStyle();
        }
    }
    ofNoFill();
}


//------------------------------------------------------------
ofxPolyline ofxPolyline::getResampledAndVerticesBySpacing(float spacing) {
    
    // Get resampled points while keeping vertices
    ofxPolyline polyline;
    polyline.setStrokeLinecap(getStrokeLinecap());
    polyline.setStrokeLinejoin(getStrokeLinejoin());
    float totalLength = getPerimeter();
    int currentIndex = 1;
    ofVec2f prevPoint = ofVec2f(FLT_MAX, FLT_MAX);
    for(float f=0; f<totalLength; f += spacing) {
        float index = getIndexAtLength(f);
        while (index > currentIndex) {
            ofVec2f newPoint = (*this)[currentIndex];
            // Don't add if this point is very close to the prev point
            if (!newPoint.match(prevPoint)) {
                polyline.lineTo(newPoint);
                prevPoint = newPoint;
            }
            currentIndex++;
        }
        ofVec2f newPoint = getPointAtLength(f);
        // Don't add if this point is very close to the prev point
        if (!newPoint.match(prevPoint)) {
            polyline.lineTo(newPoint);
            prevPoint = newPoint;
        }
    }
    
    if(!isClosed()) {
        if(polyline.size() > 0) {
            polyline[polyline.size()-1] = (*this)[size()-1];
        }
        polyline.setClosed(false);
    } else {
        polyline.setClosed(true);
    }
    return polyline;
}

//---------------------------------------------------------
#pragma mark - Private Methods

void ofxPolyline::createMesh(ofMesh &mesh, int begin, int end) {
    
    if (end - begin == 1) {
        // If only two points, just make a line.
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        pair<ofVec2f, ofVec2f> joint = jointBegin((*this)[begin], (*this)[end], width);
        mesh.addVertex(joint.first);
        mesh.addVertex(joint.second);
        
        joint = jointEnd((*this)[begin], (*this)[end], width);
        mesh.addVertex(joint.first);
        mesh.addVertex(joint.second);
        return;
    } else if (end - begin == 0) {
        // Return no mesh
        return;
    }
    
    // More than two points
    if (strokeLinejoin == OFXSVG_STROKE_LINEJOIN_MITER) {
        createMeshMiterJoint(mesh, begin, end);
    } else if (strokeLinejoin == OFXSVG_STROKE_LINEJOIN_BEVEL) {
        createMeshBevelJoint(mesh, begin, end);
    } else if (strokeLinejoin == OFXSVG_STROKE_LINEJOIN_ROUND) {
        createMeshRoundJoint(mesh, begin, end);
    }
}

//---------------------------------------------------------
void ofxPolyline::createMeshMiterJoint(ofMesh &mesh, int begin, int end) {
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    // Starting point
    pair<ofVec2f, ofVec2f> joint = jointBegin((*this)[begin], (*this)[begin+1], width);
    mesh.addVertex(joint.first);
    mesh.addVertex(joint.second);    
    
    // Between both ends
    for (int i=begin+1; i<end-1; i++) {
        joint = miterJoint((*this)[i-1], (*this)[i], (*this)[i+1], width);
        mesh.addVertex(joint.first);
        mesh.addVertex(joint.second);
    }
    
    // End point
    joint = jointEnd((*this)[end-1], (*this)[end], width);
    mesh.addVertex(joint.first);
    mesh.addVertex(joint.second);
}

//---------------------------------------------------------
void ofxPolyline::createMeshBevelJoint(ofMesh &mesh, int begin, int end) {
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    // Starting point
    pair<ofVec2f, ofVec2f> jb1 = jointBegin((*this)[begin], (*this)[begin+1], width);
    pair<vector<ofVec2f>, vector<ofVec2f> > jb2 = bevelJoint((*this)[begin], (*this)[begin+1], (*this)[begin+2], width);
    
    mesh.addVertex(jb1.first);
    mesh.addVertex(jb1.second);
    mesh.addVertex(jb2.first.front());
    
    mesh.addVertex(jb1.second);
    mesh.addVertex(jb2.first.front());
    mesh.addVertex(jb2.second.front());
    
    // Between both ends
    for (int i=begin+1; i<end-1; i++) {
        pair<vector<ofVec2f>, vector<ofVec2f> > j1 = bevelJoint((*this)[i-1], (*this)[i], (*this)[i+1], width);
        pair<vector<ofVec2f>, vector<ofVec2f> > j2 = bevelJoint((*this)[i], (*this)[i+1], (*this)[i+2], width);
        
        // triangle of bevel joint
        for (int i=0; i<j1.first.size(); i++) {
            mesh.addVertex(j1.first[i]);
        }
        for (int i=0; i<j1.second.size(); i++) {
            mesh.addVertex(j1.second[i]);
        }
        
        // the two points of the bevel triangle + next left point
        mesh.addVertex(j1.first.back());
        mesh.addVertex(j1.second.back());
        mesh.addVertex(j2.first.front());
        
        // right point of the bevel triangle + next left point
        mesh.addVertex(j1.second.back());
        mesh.addVertex(j2.first.front());
        mesh.addVertex(j2.second.front());
        
    }
    
    // End point
    pair<vector<ofVec2f>, vector<ofVec2f> > je1 = bevelJoint((*this)[end-2], (*this)[end-1], (*this)[end], width);
    pair<ofVec2f, ofVec2f> je2 = jointEnd((*this)[end-1], (*this)[end], width);
    
    for (int i=0; i<je1.first.size(); i++) {
        mesh.addVertex(je1.first[i]);
    }
    for (int i=0; i<je1.second.size(); i++) {
        mesh.addVertex(je1.second[i]);
    }
    
    mesh.addVertex(je1.first.back());
    mesh.addVertex(je1.second.back());
    mesh.addVertex(je2.first);
    
    mesh.addVertex(je1.second.back());
    mesh.addVertex(je2.first);
    mesh.addVertex(je2.second);
}

//---------------------------------------------------------
void ofxPolyline::createMeshRoundJoint(ofMesh &mesh, int begin, int end) {
    // Starting point
    pair<ofVec2f, ofVec2f> jb1 = jointBegin((*this)[begin], (*this)[begin+1], width);
    pair<vector<ofVec2f>, vector<ofVec2f> > jb2 = roundJoint((*this)[begin], (*this)[begin+1], (*this)[begin+2], width);
    
    mesh.addVertex(jb1.first);
    mesh.addVertex(jb1.second);
    mesh.addVertex(jb2.first.front());
    
    mesh.addVertex(jb1.second);
    mesh.addVertex(jb2.first.front());
    mesh.addVertex(jb2.second.front());
    
    // Between both ends
    for (int i=begin+1; i<end-1; i++) {
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        
        pair<vector<ofVec2f>, vector<ofVec2f> > j1, j2;
        j1 = roundJoint((*this)[i-1], (*this)[i], (*this)[i+1], width);
        j2 = roundJoint((*this)[i], (*this)[i+1], (*this)[i+2], width);
        
        // Triangles representing a circular sector of the round joint
        if (j1.first.size() == 1) {
            for (int i=0; i<j1.second.size()-1; i++) {
                mesh.addVertex(j1.first.front());
                mesh.addVertex(j1.second[i]);
                mesh.addVertex(j1.second[i+1]);
            }
        } else {
            for (int i=0; i<j1.first.size()-1; i++) {
                mesh.addVertex(j1.second.front());
                mesh.addVertex(j1.first[i]);
                mesh.addVertex(j1.first[i+1]);
            }
        }
        
        // the last two points of the round joint
        // + the next left point of the next round joint
        mesh.addVertex(j1.first.back());
        mesh.addVertex(j1.second.back());
        mesh.addVertex(j2.first.front());

        // the last right point + two point of the next round joint
        mesh.addVertex(j1.second.back());
        mesh.addVertex(j2.first.front());
        mesh.addVertex(j2.second.front());
    }
    
    // End point
    pair<vector<ofVec2f>, vector<ofVec2f> > je1 = roundJoint((*this)[end-2], (*this)[end-1], (*this)[end], width);
    pair<ofVec2f, ofVec2f> je2 = jointEnd((*this)[end-1], (*this)[end], width);
    
    if (je1.first.size() == 1) {
        for (int i=0; i<je1.second.size()-1; i++) {
            mesh.addVertex(je1.first.front());
            mesh.addVertex(je1.second[i]);
            mesh.addVertex(je1.second[i+1]);
        }
    } else {
        for (int i=0; i<je1.first.size()-1; i++) {
            mesh.addVertex(je1.second.front());
            mesh.addVertex(je1.first[i]);
            mesh.addVertex(je1.first[i+1]);
        }
    }
    
    mesh.addVertex(je1.first.back());
    mesh.addVertex(je1.second.back());
    mesh.addVertex(je2.first);
    
    mesh.addVertex(je1.second.back());
    mesh.addVertex(je2.first);
    mesh.addVertex(je2.second);
}


//--------------------------------------------------------------
pair<ofVec2f, ofVec2f> ofxPolyline::jointBegin(ofVec2f p1, ofVec2f p2, float width) {
    ofVec2f v1 = (p2 - p1).normalize();
    ofVec2f p1l = p1 + v1.getRotated(90)*width/2;
    ofVec2f p1r = p1 + v1.getRotated(-90)*width/2;
    return pair<ofVec2f, ofVec2f>(p1l, p1r);
}

pair<ofVec2f, ofVec2f> ofxPolyline::jointEnd(ofVec2f p1, ofVec2f p2, float width) {
    ofVec2f v1 = (p2 - p1).normalize();
    ofVec2f p2l = p2 + v1.getRotated(90)*width/2;
    ofVec2f p2r = p2 + v1.getRotated(-90)*width/2;
    return pair<ofVec2f, ofVec2f>(p2l, p2r);
}

//--------------------------------------------------------------
pair<ofVec2f, ofVec2f> ofxPolyline::miterJoint(ofVec2f p1, ofVec2f p2, ofVec2f p3, float width) {

    ofVec2f v1 = (p2 - p1).normalize();
    ofVec2f v2 = (p3 - p2).normalize();
    
    // Intersection of the left 
    ofVec2f p1l = p1 + v1.getRotated(90)*width/2;
    ofVec2f p3l = p3 + v2.getRotated(90)*width/2;
    ofVec2f pCross1 = intersect(p1l, p3l, v1, v2);
    
    // Intersection on the right
    ofVec2f p1r = p1 + v1.getRotated(-90)*width/2;
    ofVec2f p3r = p3 + v2.getRotated(-90)*width/2;
    ofVec2f pCross2 = intersect(p1r, p3r, v1, v2);
    
    return pair<ofVec2f, ofVec2f>(pCross1, pCross2);
}

//--------------------------------------------------------------
pair<vector<ofVec2f>, vector<ofVec2f> > ofxPolyline::bevelJoint(ofVec2f p1, ofVec2f p2, ofVec2f p3, float width) {

    ofVec2f v1 = (p2 - p1).normalize();
    ofVec2f v2 = (p3 - p2).normalize();
    
    vector<ofVec2f> left;  // intersections on the left
    vector<ofVec2f> right;
    
    // どちらに曲がっているか
    if (0 < v1.angle(v2)) {
        // When the line is turning left,
        // find an intersection on the left of the two lines.
        ofVec2f p1l = p1 + v1.getRotated(90)*width/2;
        ofVec2f p3l = p3 + v2.getRotated(90)*width/2;
        ofVec2f pCross = intersect(p1l, p3l, v1, v2);
        left.push_back(pCross);
        
        // two points on the right of p2
        ofVec2f p2r1 = p2 + v1.getRotated(-90)*width/2;
        ofVec2f p2r2 = p2 + v2.getRotated(-90)*width/2;
        right.push_back(p2r1);
        right.push_back(p2r2);
    } else {
        // When the line is turning right,
        // find an intersection on the right of the two lines.
        ofVec2f p1r = p1 + v1.getRotated(-90)*width/2;
        ofVec2f p3r = p3 + v2.getRotated(-90)*width/2;
        ofVec2f pCross = intersect(p1r, p3r, v1, v2);
        right.push_back(pCross);
        
        // two points on the left of p2
        ofVec2f p2l1 = p2 + v1.getRotated(90)*width/2;
        ofVec2f p2l2 = p2 + v2.getRotated(90)*width/2;
        left.push_back(p2l1);
        left.push_back(p2l2);
    }
    return pair<vector<ofVec2f>, vector<ofVec2f> >(left, right);
}

//--------------------------------------------------------------
pair<vector<ofVec2f>, vector<ofVec2f> > ofxPolyline::roundJoint(ofVec2f p1, ofVec2f p2, ofVec2f p3, float width) {

    ofVec2f v1 = (p2 - p1).normalize();
    ofVec2f v2 = (p3 - p2).normalize();
    
    vector<ofVec2f> acute;  // Intersections on the acute angle side
    vector<ofVec2f> obtuse;
    
    bool isTurningRight;
    float acuteSideAngle;
    bool arcClockwise;
    
    if (0 < v1.angle(v2)) {
        // When the line is turning right
        isTurningRight = true;
        acuteSideAngle = 90;
        arcClockwise = true;
        
    } else {
        // When the line is turning left
        isTurningRight = false;
        acuteSideAngle = -90;
        arcClockwise = false;
    }
    
    // Find an intersection on the acute angle side of the two vectors
    ofVec2f p1a = p1 + v1.getRotated(acuteSideAngle)*width/2;
    ofVec2f p3a = p3 + v2.getRotated(acuteSideAngle)*width/2;
    ofVec2f pCross = intersect(p1a, p3a, v1, v2);
    acute.push_back(pCross);

    // The arc on the obtuse angle side of p2
    ofVec2f p2o1 = p2 + v1.getRotated(-acuteSideAngle)*width/2;
    ofVec2f p2o2 = p2 + v2.getRotated(-acuteSideAngle)*width/2;
    
    float angle1 = ofVec2f(1, 0).angle(p2o1-p2);
    float angle2 = ofVec2f(1, 0).angle(p2o2-p2);
    
    ofPath arc;
    arc.arc(p2, width/2, width/2, angle1, angle2, arcClockwise);
    ofPolyline arcPolyline = arc.getOutline()[0];        
    // Delete the center of the arc.
    vector<ofPoint> points = arcPolyline.getVertices();
    points.erase(points.begin());
    arcPolyline.clear();
    arcPolyline.addVertices(points);
    
    ofPolyline arcResampled = arcPolyline.getResampledBySpacing(1);
    vector<ofPoint> vertices = arcResampled.getVertices();
    
    for (int i=0; i<vertices.size(); i++) {
        // Convert from ofPoint (ofVec3f) to ofVec2f
        obtuse.push_back(vertices[i]);
    }
    
    // Occasionaly the size of obtuse is zero (I don't know why).
    if (obtuse.size() == 0) {
        obtuse.push_back((p1+p2)/2);
    }
    
    // The return value is pair(left, right)
    if (isTurningRight) {
        return pair<vector<ofVec2f>, vector<ofVec2f> >(obtuse, acute);
    } else {
        return pair<vector<ofVec2f>, vector<ofVec2f> >(acute, obtuse);
    }
}

//--------------------------------------------------------------
ofVec2f ofxPolyline::intersect(ofVec2f p1, ofVec2f p2, ofVec2f v1, ofVec2f v2) {
    // Find a intersection of two lines represented by
    // a starting point and a vector for each
    float jx, jy;
    
    float cross = v1.x*v2.y-v2.x*v1.y;
    if (abs(cross) < 0.001) {
        // When two vectors are linear dependent.
        // In this program, three points are on a line.
        // Just return the middle point.
        jx = (p1.x + p2.x) / 2;
        jy = (p1.y + p2.y) / 2;
    } else {
        // Linear independent
        if (v1.x == 0) {
            jx = p1.x;
        } else if (v2.x == 0) {
            jx = p2.x;
        } else {
            jx = (v1.x*(v2.x*p1.y+p2.x*v2.y)-v2.x*(p1.x*v1.y+v1.x*p2.y))/cross;
        }
        if (v1.y == 0) {
            jy = p1.y;
        } else if (v2.y == 0) {
            jy = p2.y;
        } else {
            jy = (v1.y*(v2.y*p1.x+p2.y*v2.x)-v2.y*(p1.y*v1.x+v1.y*p2.x))/(-cross);
        }
    }
    return ofVec2f(jx, jy);
}
