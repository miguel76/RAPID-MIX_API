//
//  rapidGVF.h
//
//  Created by Francisco on 04/05/2017.
//  Copyright © 2017 Goldsmiths. All rights reserved.
//

#ifndef rapidGVF_h
#define rapidGVF_h

#include <vector>
#include <string>
#include "GVF.h"

namespace rapidmix { class trainingData; }

class rapidGVF {
    
public: 
    rapidGVF();

    ~rapidGVF();
    
    bool train(const rapidmix::trainingData &newTrainingData);
    
    std::vector<double> process(const std::vector<double> &inputVector);
    
protected:
    GVF * gvf;
    GVFGesture currentGesture;
    GVFOutcomes outcomes;
};

#endif
