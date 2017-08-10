//
//  rapidGVF.cpp
//
//  Created by Francisco on 04/05/2017.
//  Copyright © 2017 Goldsmiths. All rights reserved.
//

#include "rapidGVF.h"
#include "trainingData.h"

rapidGVF::rapidGVF()
{
    // Initialised with default configuration
    this->gvf = new GVF();
    this->currentGesture = GVFGesture();
}

rapidGVF::~rapidGVF()
{
    delete this->gvf;
    this->currentGesture = NULL;
}

template<>
bool rapidmix::machineLearning<rapidGVF>::train(const trainingData &newTrainingData) {
    
    return rapidGVF::train(newTrainingData);
}

bool rapidGVF::train(const rapidmix::trainingData &newTrainingData) {
    
    if (newTrainingData.trainingSet.size() < 1) {
        // no recorded phrase
        return false;
    }
    
    if (newTrainingData.trainingSet.size() == 1 && newTrainingData.trainingSet[0].elements.size() == 0) {
        // empty recorded phrase
        return false;
    }
    
    if(gvf->getState() != GVF::STATE_LEARNING)
    {
        gvf->setState(GVF::STATE_LEARNING);
    }
    
    //Go through every phrase

    for (int h = 1; h < newTrainingData.trainingSet.size(); ++h) {
        this->gvf->startGesture();
        for (int i = 0; i < newTrainingData.trainingSet[h].elements.size(); ++i) {
            
            std::vector<double> vd = newTrainingData.trainingSet[h].elements[i].input;
            
            // Using template <class InputIterator> vector to change for vec<double> to vec<float>
            std::vector<float> vf(vd.begin(), vd.end());
            this->currentGesture.addObservation(vf);
        }
        this->gvf->addGestureTemplate(this->currentGesture);
    }
    return true;
}

template<>
std::vector<double> rapidmix::machineLearning<rapidGVF>::run(const std::vector<double> &inputVector) {
    
    return rapidGVF::process(inputVector);
}

std::vector<double> rapidGVF::process(const std::vector<double> &inputVector){
    
    
    if (inputVector.size() == 0) {
        return std::vector<double>();
    }
    
    gvf->restart();
    
    if(gvf->getState() != GVF::STATE_FOLLOWING)
    {
        gvf->setState(GVF::STATE_FOLLOWING);
    }
    
    // Using template <class InputIterator> vector to change for vec<double> to vec<float>
    std::vector<float> vf(inputVector.begin(),inputVector.end());
    
    this->currentGesture.addObservation(vf);
    this->outcomes = this->gvf->update(this->currentGesture.getLastObservation());
    
    std::vector<double> output;
    output.push_back(this->outcomes.likeliestGesture);
    output.insert(output.end(), this->outcomes.likelihoods.begin(), this->outcomes.likelihoods.end());
    output.insert(output.end(), this->outcomes.alignments.begin(), this->outcomes.alignments.end());
//    output.insert(output.end(), this->outcomes.dynamics.begin(), this->outcomes.dynamics.end());
//    output.insert(output.end(), this->outcomes.scalings.begin(), this->outcomes.scalings.end());
//    output.insert(output.end(), this->outcomes.rotations.begin(), this->outcomes.rotations.end());
//    
    return output;
}










