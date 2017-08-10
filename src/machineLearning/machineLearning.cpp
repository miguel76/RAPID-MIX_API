/*
 * machineLearning.cpp
 * Created by Michael Zbyszynski on 10 Jan 2016
 * Copyright © 2017 Goldsmiths. All rights reserved.
 */

#include "machineLearning.h"

RAPIDMIX_BEGIN_NAMESPACE

void trainingData2rapidLib (const trainingData &newTrainingData, std::vector<trainingExample> &trainingSet) {
    for (int h = 0; h < newTrainingData.trainingSet.size(); ++h) { //Go through every phrase
        for (int i = 0; i < newTrainingData.trainingSet[h].elements.size(); ++i) { //...and every element
            trainingExample tempExample;
            tempExample.input = newTrainingData.trainingSet[h].elements[i].input;
            if (newTrainingData.trainingSet[h].elements[i].output.size() > 0) {
                tempExample.output = newTrainingData.trainingSet[h].elements[i].output;
            } else {
                std::unordered_map<std::string, int>::const_iterator mappedLabel = newTrainingData.labels.find(newTrainingData.trainingSet[h].label);
                tempExample.output.push_back(double(mappedLabel->second));
            }
            trainingSet.push_back(tempExample);
        }
    }
};

template<>
bool machineLearning<classification>::train(const trainingData &newTrainingData) {
    std::vector<trainingExample> trainingSet;
    trainingData2rapidLib(newTrainingData, trainingSet);
    return classification::train(trainingSet);
}

template<>
bool machineLearning<regression>::train(const trainingData &newTrainingData) {
    std::vector<trainingExample> trainingSet;
    trainingData2rapidLib(newTrainingData, trainingSet);
    return regression::train(trainingSet);
}

template<>
bool machineLearning<seriesClassification>::train(const trainingData &newTrainingData) {
    std::vector<std::vector<trainingExample> > seriesSet;
    for (int i = 1; i < newTrainingData.trainingSet.size(); ++i) { //each phrase
        std::vector<trainingExample> tempSeries;
        for (int j = 0; j < newTrainingData.trainingSet[i].elements.size(); ++j) { //each element
            trainingExample tempExample;
            tempExample.input = newTrainingData.trainingSet[i].elements[j].input;
            tempSeries.push_back(tempExample);
        }
        seriesSet.push_back(tempSeries);
    }
    return seriesClassification::trainTrainingSet(seriesSet);
}

template<>
int machineLearning<seriesClassification>::run(const std::vector<std::vector<double> > &inputSeries) {
    return seriesClassification::run(inputSeries);
}


RAPIDMIX_END_NAMESPACE
