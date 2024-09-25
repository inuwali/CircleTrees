//
//  Parameters.hpp
//  CircleTree
//
//  Created by Owen Mathews on 9/24/24.
//

#ifndef Parameters_hpp
#define Parameters_hpp

#include <stdio.h>

#include "ofJson.h"
#include "ofApp.h"

struct HSBFloats {
    float hue;
    float saturation;
    float brightness;
    float alpha;
    
    HSBFloats(): hue(255), saturation(255), brightness(255), alpha(255) {}
    
    static HSBFloats fromJson(ofJson json);
    
    ofJson jsonRepresentation();
};

struct TreeRenderParameters {
    int drawChooserIndex;
    int colorChooserIndex;
    ofBlendMode blendMode;
    
    TreeRenderParameters():
    drawChooserIndex(0),
    colorChooserIndex(0),
    blendMode(OF_BLENDMODE_DISABLED) {}
        
    static TreeRenderParameters fromJson(ofJson json);
    
    ofJson jsonRepresentation();
};


struct TreesParameters {
    uint64_t timestamp;
    uint64_t randomSeed;
    int treeDepth;
    int animatorChooserIndex;
    TreeRenderParameters renderParameters1;
    TreeRenderParameters renderParameters2;
    HSBFloats backgroundColor;
    
    TreesParameters():
    timestamp(ofGetUnixTimeMillis()),
    randomSeed(0),
    treeDepth(3),
    animatorChooserIndex(0),
    renderParameters1(TreeRenderParameters()),
    renderParameters2(TreeRenderParameters()),
    backgroundColor(HSBFloats()) {}
    
    static TreesParameters fromFile(std::string filePath);
    
    ofJson jsonRepresentation();
};


#endif /* Parameters_hpp */
