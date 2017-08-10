/*
 * machineLearning.h
 * Created by Michael Zbyszynski on 10 Jan 2016
 * Copyright © 2017 Goldsmiths. All rights reserved.
 */

#ifndef machineLearning_h
#define machineLearning_h

#include "rapidMix.h"
#include "trainingData.h"

////////// Include all of the machine learning algorithms here
#include "classification.h"
#include "regression.h"
#include "seriesClassification.h"
#include "rapidXmmTools.h"
#include "rapidGVF.h"

// forward declaration
namespace rapidmix { class trainingData; }

RAPIDMIX_BEGIN_NAMESPACE

//* Host class for machine learning algorithms */
template <typename MachineLearningModule>
class machineLearning : public MachineLearningModule {
public:
    
    //* Constructors */
    machineLearning() : MachineLearningModule() {};
    
    template<class T>
    machineLearning(T type) : MachineLearningModule(type) {};
    
    //* this function becomes specialized in the implementation */
    bool train(const trainingData &newTrainingData);
    
    //* this function is not being specialized
    std::vector<double> run(const std::vector<double> &inputVector) {
        return MachineLearningModule::run(inputVector);
    }
    
    //* This is the one I'm using for DTW */
    int run(const std::vector<std::vector<double> > &inputSeries);
    
    bool reset() {
        return MachineLearningModule::reset();
    }
    
private:
    MachineLearningModule module;
};

////////// typedefs for calling different algorithms

typedef machineLearning<classification> staticClassification;
typedef machineLearning<regression> staticRegression;
typedef machineLearning<seriesClassification> dtwTemporalClassification;

typedef xmmToolConfig xmmConfig;
typedef machineLearning<xmmGmmTool> xmmStaticClassification;
typedef machineLearning<xmmGmrTool> xmmStaticRegression;
typedef machineLearning<xmmHmmTool> xmmTemporalClassification;
typedef machineLearning<xmmHmrTool> xmmTemporalRegression;

typedef machineLearning<rapidGVF> gvfTemporalVariation;


RAPIDMIX_END_NAMESPACE

#endif
