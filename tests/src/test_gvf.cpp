//
//  test_GVF.cpp
//  Unit tests for GFV
//
//  Created by Francisco Bernardo on 17/02/2017.
//  Copyright © 2017 Goldsmiths. All rights reserved.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "machineLearning.h"
#include "trainingData.h"


TEST_CASE( "Tests default GVF ctor.", "[GVF]" ) {

    GVF * gvf = new GVF();
    gvf->translate(false);
    gvf->segmentation(false);
    
    REQUIRE(gvf != NULL);

}

TEST_CASE( "Tests default GVFGesture ctor.", "[GVF]" ) {
    
    GVF * gvf = new GVF();
    GVFGesture gesture;
    GVFOutcomes outcomes;
    
    gvf->translate(false);
    gvf->segmentation(false);
    
    gvf->setNumberOfParticles(1000);
    gvf->setTolerance(0.2f);
    gvf->setScalingsVariance(0.00001f); //0.00001f);   //0.000002f);   //0.00004f);
    gvf->setDynamicsVariance(0.0001f);
   
    vector<vector<float>> trainingData;
    
    if(gvf->getState() == GVF::STATE_LEARNING)
    {
        gvf->startGesture();
        
        for (vector<vector<float>>::iterator frame = trainingData.begin(); frame != trainingData.end(); frame++)
        {
            // Fill the template
            gesture.addObservation(*frame);
        }
    }
    
    gvf->setState(GVF::STATE_FOLLOWING);
    
    gesture.clear();
    trainingData.clear();
    
    vector<vector<float>> testData;
    
    if (gvf->getState()==GVF::STATE_FOLLOWING)
    {
        for (vector<vector<float>>::iterator frame = testData.begin(); frame != testData.end(); frame++)
        {
            // Fill the template
            gesture.addObservation(*frame);
        }
        gvf->update(gesture.getLastObservation());
    }
    
//    float phase = gvf->getOutcomes().estimations[0].alignment;
//    float speed = gvf->getOutcomes().estimations[0].dynamics[0];
//    
//    getDynamicsVariance();
//    getScalingsVariance();
//    getRotationsVariance();
    
    REQUIRE( gvf != NULL);
    
}



SCENARIO("Test GVF Regression", "[machineLearning]")
{
    GIVEN("gvf object and training dataset")
    {
        rapidmix::gvfTemporalVariation gvf;
        rapidmix::trainingData myData;
        
        // Record first gesture
        myData.startRecording();
        std::vector<double> inputs = { 4.0, 0.7 };
        myData.addElement(inputs);
        
        inputs = { 3.0, 0.8 };
        myData.addElement(inputs);
        
        inputs = { 2.0, 0.9 };
        myData.addElement(inputs);
        
        inputs = { 1.0, 1.0 };
        myData.addElement(inputs);
        
        inputs = { 0.4, 1.2 };
        myData.addElement(inputs);
        
        inputs = { 0.2, 1.4 };
        myData.addElement(inputs);
        myData.stopRecording();
        
        // Record reverse of first gesture
        myData.startRecording();
        inputs = { 0.2, 1.4 };
        myData.addElement(inputs);
        
        inputs = { 0.4, 1.2 };
        myData.addElement(inputs);
        
        inputs = { 1.0, 1.0 };
        myData.addElement(inputs);
        
        inputs = { 2.0, 0.9 };
        myData.addElement(inputs);
        
        inputs = { 3.0, 0.8 };
        myData.addElement(inputs);
        
        inputs = { 4.0, 0.7 };
        myData.addElement(inputs);
        myData.stopRecording();
        
        // Train
        gvf.train(myData);
        
        // Set first gesture (or a fragment of it)
        std::vector<double> outcomes;
        
        WHEN("when gvf is trained with a gesture and the reverse gesture")
        {
            THEN("follows the the gesture (first) and confirm it is the likeliestGesture and likelihoods reasonable")
            {
                outcomes = gvf.process(inputs = { 3.0, 0.8 });
                outcomes = gvf.process({ 2.0, 0.9 });
                outcomes = gvf.process({ 1.0, 1.0 });
                outcomes = gvf.process({ 0.4, 1.2 });
                outcomes = gvf.process({ 0.2, 1.4 });
                
                // The assumtion for the test is that the outcome of the last segment of the test gesture must converge
                REQUIRE(outcomes[0] == 0); // outcomes[0] - likeliestGesture must be equal to first gesture '0'
                REQUIRE(outcomes[1] > 0.5); // outcomes[1] - likelihood gesture '0' must be greater than 50%
                REQUIRE(outcomes[2] < 0.5); // outcomes[2] - likelihood gesture '1' must be lesser than 50%
//                REQUIRE(outcomes[3] < 0.5); // outcomes[3] - alignment gesture '0' must be lesser than 50%
//                REQUIRE(outcomes[4] < 0.5); // outcomes[4] - alignment gesture '1' must be lesser than 50%
            }
        }
        
        WHEN("when gvf is trained with two gestures")
        {
            THEN("gvf follows the test gesture (first gesture scaled) and confirm it is the likeliestGesture and likelihoods anc  reasonable ")
            {

            }
        }
        
        WHEN("when gvf is trained with two gestures")
        {
            THEN("gvf follows the test gesture (first gesture scaled) and confirm it is the likeliestGesture and likelihoods anc  reasonable ")
            {
                
            }
        }
    }
}
