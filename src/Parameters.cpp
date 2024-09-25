//
//  Parameters.cpp
//  CircleTree
//
//  Created by Owen Mathews on 9/24/24.
//

#include "Parameters.hpp"
#include "ofJson.h"
#include "ofApp.h"


HSBFloats HSBFloats::fromJson(ofJson json) {
    HSBFloats result = HSBFloats();
    
    try {
        result.hue = json["hue"];
    } catch (const ofJson::type_error& e) { }
    try {
        result.saturation = json["saturation"];
    } catch (const ofJson::type_error& e) { }
    try {
        result.brightness = json["brightness"];
    } catch (const ofJson::type_error& e) { }
    try {
        result.alpha = json["alpha"];
    } catch (const ofJson::type_error& e) { }
    
    return result;
}

ofJson HSBFloats::jsonRepresentation() {
    ofJson json;
    
    json = {{"hue", hue}};
    json += {"saturation", saturation};
    json += {"brightness", brightness};
    json += {"alpha", alpha};
    
    return json;
}

TreeRenderParameters TreeRenderParameters::fromJson(ofJson json) {
    TreeRenderParameters params = TreeRenderParameters();
    
    try {
        params.drawChooserIndex = json["drawChooserIndex"];
    } catch (const ofJson::type_error& e) { }
    try {
        params.colorChooserIndex = json["colorChooserIndex"];
    } catch (const ofJson::type_error& e) { }
    try {
        params.blendMode = json["blendMode"];
    } catch (const ofJson::type_error& e) { }
    
    return params;
}

ofJson TreeRenderParameters::jsonRepresentation() {
    ofJson json;
    
    json = {{"drawChooserIndex", drawChooserIndex}};
    json += {"colorChooserIndex", colorChooserIndex};
    json += {"blendMode", blendMode};
    
    return json;
}


TreesParameters TreesParameters::fromFile(std::string filePath) {
    TreesParameters params = TreesParameters();
    
    ofJson json = ofLoadJson(filePath);
    
    try {
        params.timestamp = json["timestamp"];
    } catch (const ofJson::type_error& e) { }
    try {
        params.randomSeed = json["randomSeed"];
    } catch (const ofJson::type_error& e) { }
    try {
        params.treeDepth = json["treeDepth"];
    } catch (const ofJson::type_error& e) { }
    try {
        params.animatorChooserIndex = json["animatorChooserIndex"];
    } catch (const ofJson::type_error& e) { }
    try {
        //            ofJson rps = json["renderParameters"];
        //            params.renderParameters1 = TreeRenderParameters::fromJson(rps.at(0));
        params.renderParameters1 = TreeRenderParameters::fromJson(json["renderParameters1"]);
        params.renderParameters2 = TreeRenderParameters::fromJson(json["renderParameters2"]);
        //            params.renderParameters = json["renderParameters"];
        //            for (auto& rp : json["renderParameters"].items()) {
        //                params.renderParameters.push_back(rp.value().template get<TreeRenderParameters>());
        //            }
    } catch (const ofJson::type_error& e) { }
    try {
        params.backgroundColor = HSBFloats::fromJson(json["backgroundColor"]);
    } catch (const ofJson::type_error& e) { }
    
    return params;
}

ofJson TreesParameters::jsonRepresentation() {
    ofJson json;
    
    json.emplace("timestamp", timestamp);
    json += {"randomSeed", randomSeed};
    json += {"treeDepth", treeDepth};
    json += {"animatorChooserIndex", animatorChooserIndex};
    json += {"renderParameters1", renderParameters1.jsonRepresentation()};
    json += {"renderParameters2", renderParameters2.jsonRepresentation()};
    json += {"backgroundColor", backgroundColor.jsonRepresentation()};
    json += {"renderParameters", {renderParameters1.jsonRepresentation(), renderParameters2.jsonRepresentation()}};
    
    return json;
}
