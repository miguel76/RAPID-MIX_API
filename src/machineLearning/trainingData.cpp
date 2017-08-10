/*
 * trainingData.cpp
 * Created by Michael Zbyszynski on 2 Feb 2017
 * Copyright © 2017 Goldsmiths. All rights reserved.
 */

#include <fstream>
#include "trainingData.h"

RAPIDMIX_BEGIN_NAMESPACE

trainingData::trainingData () {
    phrase defaultPhrase = {0, "default"};
    trainingSet.push_back(defaultPhrase);
    currentId = 1;
    targetPhrase = 0;
};

uint32_t trainingData::assignCurrentId() {
    uint32_t returnVal = currentId;
    ++currentId;
    return returnVal;
}


uint32_t trainingData::startRecording() {
    phrase tempPhrase = { assignCurrentId(), std::to_string(tempPhrase.uniqueId) }; //TODO: Is this label helpful? -MZ
    trainingSet.push_back(tempPhrase);
    targetPhrase = int(trainingSet.size() - 1);
    return tempPhrase.uniqueId;
};

uint32_t trainingData::startRecording(std::string label) {
    phrase tempPhrase = { assignCurrentId(), label };
    labels.insert(std::make_pair(label, labels.size()));
    trainingSet.push_back(tempPhrase);
    targetPhrase = int(trainingSet.size() - 1);
    return tempPhrase.uniqueId;
    
};

uint32_t trainingData::addElement(std::vector<double>input, std::vector<double> output) {
    element newElement;
    newElement.uniqueId = assignCurrentId();
    newElement.input = input;
    newElement.output = output;
    newElement.timeStamp = NULL;
    trainingSet[targetPhrase].elements.push_back(newElement);
    return newElement.uniqueId;
}

uint32_t trainingData::addElement(std::vector<double>input) {
    element newElement;
    newElement.uniqueId = assignCurrentId();
    newElement.input = input;
    newElement.timeStamp = NULL;
    trainingSet[targetPhrase].elements.push_back(newElement);
    return newElement.uniqueId;
}


void trainingData::stopRecording() {
    targetPhrase = 0; //direct elements to default phrase
}

std::string trainingData::getLabel(int value) {
    std::string theLabel = "not found";
    for (const auto& element : labels) {
        if (element.second == value) {
            theLabel = element.first;
        }
    }
    return theLabel;
}

std::vector<std::string> trainingData::getColumnNames() {
    return trainingSet[targetPhrase].columnNames;
}

void trainingData::setColumnNames(std::vector<std::string> column_names) {
    trainingSet[targetPhrase].columnNames = column_names;
}

Json::Value trainingData::parse2json() {
    Json::Value root;
    Json::Value metadata;
    Json::Value trainingSetJSON;
    
    metadata["creator"] = "RAPID-MIX API C++";
    metadata["version"] = RAPIDMIX_VERSION;
    
    //Go through all the phrases
    for (int i = 0; i < trainingSet.size(); ++i) {
        Json::Value thisPhrase;
        thisPhrase.append(trainingSet[i].uniqueId);
        thisPhrase.append(trainingSet[i].label);
      
        Json::Value column_names;
        for (int j = 0; j < trainingSet[i].columnNames.size(); ++j) {
            column_names.append(trainingSet[i].columnNames[j]);
        }
        thisPhrase.append(column_names);
        
        Json::Value elements;
        for (int j = 0; j < trainingSet[i].elements.size(); ++j) {
            Json::Value singleElement;
            
            Json::Value elementInput;
            for (int k = 0; k < trainingSet[i].elements[j].input.size(); ++k) {
                elementInput.append(trainingSet[i].elements[j].input[k]);
            }
            singleElement.append(elementInput);
            
            Json::Value elementOutput;
            for (int k = 0; k < trainingSet[i].elements[j].output.size(); ++k) {
                elementOutput.append(trainingSet[i].elements[j].output[k]);
            }
            singleElement.append(elementOutput);
            
            singleElement.append(trainingSet[i].elements[j].timeStamp);
            elements.append(singleElement);
        }
        thisPhrase.append(elements);
        
        trainingSetJSON.append(thisPhrase);
    }
    
    root["metadata"] = metadata;
    root["trainingSet"] = trainingSetJSON;
    return root;
}

std::string trainingData::getJSON() {
    Json::Value root = parse2json();
    return root.toStyledString();
}

void trainingData::writeJSON(const std::string &filepath) {
    Json::Value root = parse2json();
    std::ofstream jsonOut;
    jsonOut.open (filepath);
    Json::StyledStreamWriter writer;
    writer.write(jsonOut, root);
    jsonOut.close();
    
}

void trainingData::json2trainingSet(const Json::Value &root) {
    trainingSet = {};
    for (const Json::Value& jsonPhrase : root["trainingSet"]) {
    
        phrase tempPhrase = { jsonPhrase[0].asUInt(), jsonPhrase[1].asString()    };
        
        for (int i = 0; i < jsonPhrase[2].size(); ++i) {
            tempPhrase.columnNames.push_back(jsonPhrase[2][i].asString());
        }
        
        for (int i = 0; i < jsonPhrase[3].size(); ++i) {
            element tempElement;
            for (int j = 0; j < jsonPhrase[3][i][0].size(); ++j) {
                tempElement.input.push_back(jsonPhrase[3][i][0][j].asDouble());
            }
            for (int j = 0; j < jsonPhrase[3][i][1].size(); ++j) {
                tempElement.output.push_back(jsonPhrase[3][i][1][j].asDouble());
            }
            tempElement.timeStamp = jsonPhrase[3][i][2].asDouble();
            
            tempPhrase.elements.push_back(tempElement);
        }
        trainingSet.push_back(tempPhrase);
    }
}

bool trainingData::putJSON(const std::string &jsonMessage) {
    Json::Value parsedFromString;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
    if (parsingSuccessful)
    {
        json2trainingSet(parsedFromString);
    }
    return parsingSuccessful;
}

bool trainingData::readJSON(const std::string &filepath) {
    Json::Value root;
    std::ifstream file(filepath);
    file >> root;
    json2trainingSet(root);
    return true; //TODO: check something first
}


RAPIDMIX_END_NAMESPACE
