/*
 * trainingData.h
 * Created by Michael Zbyszynski on 2 Feb 2017
 * Copyright © 2017 Goldsmiths. All rights reserved.
 */

#ifndef trainingData_h
#define trainingData_h

#include <vector>
#include <string>
#include <unordered_map>

#include "rapidMix.h"
#include "json.h"

RAPIDMIX_BEGIN_NAMESPACE

/** This is used by both NN and KNN models for training */
class trainingData {
  
public:
    
    trainingData();
    
    struct element{
        uint32_t uniqueId; //MZ: Does this scope of this id need to extend beyond this instantiation?
        std::vector<double> input;
        std::vector<double> output;
        double timeStamp;
    };
    
    struct phrase {
        uint32_t uniqueId;
        std::string label; //TODO: Need to work this with templates
        std::vector<std::string> columnNames;
        std::vector<element> elements;
    };
    
    std::vector<phrase> trainingSet;
    
    //* Create a new phrase that can be recorded into. Returns phrase id */
    uint32_t startRecording();
    
    //* Create new phrase, with a label, that can be recorded into. Returns phrase id */
    uint32_t startRecording(std::string label);
    
    //* Add an element with input and output to the phrase that is recording, or to the default phrase if recording is stopped. Returns phrase id. */
    uint32_t addElement(std::vector<double>input, std::vector<double> output);
    
    //* Add an element with just input to the phrase that is recording, or to the default phrase if recording is stopped. Returns phrase id. */
    uint32_t addElement(std::vector<double>input);
    
    void stopRecording();
    
    std::vector<std::string> getColumnNames();
    void setColumnNames(std::vector<std::string> columnNames);
  
    
    /** Get a JSON representation of the data set in the form of a styled string */
    std::string getJSON();
    /** Write a JSON version of the training set to specified file path */
    void writeJSON(const std::string &filepath);
    /** populate a data set with string. See getJSON() */
    bool putJSON(const std::string &jsonMessage);
    /** read a JSON file at file path and build a training set from it */
    bool readJSON(const std::string &filepath);

    
    //this holds string labels
    std::unordered_map<std::string, int> labels;
    std::string getLabel(int value);
    
private:
    int targetPhrase;
    uint32_t currentId;
    
    //* Returns and increments current id */
    uint32_t assignCurrentId();
    
    Json::Value parse2json();
    void json2trainingSet(const Json::Value &newTrainingData);
    
};

RAPIDMIX_END_NAMESPACE

#endif
