//
//  test_RapidLib.cpp
//  Unit tests for RapidLib
//
//  Created by Francisco Bernardo on 17/02/2017.
//  Copyright © 2017 Goldsmiths. All rights reserved.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "machineLearning.h"
#include <iostream>

SCENARIO("Test NN Regression", "[machineLearning]")
{
    GIVEN("NN Regression object and training dataset")
    {
        rapidmix::staticRegression myNN;
        
        rapidmix::trainingData myData;
        std::vector<double> input = { 0.2, 0.7 };
        std::vector<double> output = { 3.0 };
        REQUIRE(myData.addElement(input, output) == 1);
        
        input = { 2.0, 44.2 };
        output = { 20.14 };
        REQUIRE(myData.addElement(input, output) == 2);
        
        myData.writeJSON("/var/tmp/testTrainingData.json");
        
        REQUIRE(myNN.train(myData) == true);
        
        std::string filepath = "/var/tmp/modelSetDescription.json";
        myNN.writeJSON(filepath);
        
        std::vector<double> inputVec = { 1.618, 18.9 }; //arbitrary input        
        REQUIRE(myNN.run(inputVec)[0] == 12.596715279688551);
        
        WHEN("when NN model is deserialized from file")
        {
            rapidmix::staticRegression myNNfromFile;
            REQUIRE(myNNfromFile.readJSON(filepath) == true);
            
            THEN("run models and in-memory model")
            {
                REQUIRE(myNN.run(inputVec)[0] == myNNfromFile.run(inputVec)[0]);
            }
        }
        
        WHEN("when NN model is deserialized from JSON stream")
        {
            rapidmix::staticRegression myNNfromString;
            REQUIRE(myNNfromString.putJSON(myNN.getJSON()) == true);
            
            THEN("run models and in-memory model")
            {
                REQUIRE(myNN.run(inputVec)[0] == myNNfromString.run(inputVec)[0]);
            }
        }
    }
}

SCENARIO("Test kNN classification", "[machineLearning]")
{
    GIVEN("kNN object and training dataset")
    {
        rapidmix::staticClassification myKnn;
        
        rapidmix::trainingData myData;
        std::vector<double> input = { 0.2, 0.7 };
        std::vector<double> output = { 3.0 };
        REQUIRE(myData.addElement(input, output) == 1);
        
        input = { 2.0, 44.2 };
        output = { 20.14 };
        REQUIRE(myData.addElement(input, output) == 2);
        
        REQUIRE(myKnn.train(myData) == true);
        
        std::string filepath2 = "/var/tmp/modelSetDescription_knn.json";
        myKnn.writeJSON(filepath2);
        
        rapidmix::staticClassification myKnnFromString;
        myKnnFromString.putJSON(myKnn.getJSON());
        
        rapidmix::staticClassification myKnnFromFile;
        myKnnFromFile.readJSON(filepath2);
        
        
        std::vector<double> inputVec = { 2.0, 44.2 };
        
        REQUIRE(myKnn.run(inputVec)[0] == 20.0); //FIXME: kNN truncates to ints!
        
        WHEN("when kNN model is read from file")
        {
            THEN("run models and compare")
            {
                REQUIRE(myKnn.run(inputVec)[0] == myKnnFromFile.run(inputVec)[0]);
            }
        }
        
        WHEN("when NN model is read from JSON stream")
        {
            THEN("run models and compare")
            {
                REQUIRE(myKnn.run(inputVec)[0] == myKnnFromString.run(inputVec)[0]);
            }
        }
    }
}

SCENARIO("Test DTW classification", "[machineLearning]")
{
    GIVEN("DTW object and training dataset")
    {
        rapidmix::dtwTemporalClassification myDTW;
        
        rapidmix::trainingData myData;
        myData.startRecording("setOne");
        std::vector<double> input = { 0.1, 0.5 };
        std::vector<double> output = {};
        REQUIRE(myData.addElement(input, output) == 2); //TODO: Shouldn't this be 1?
        input = { 0.2, 0.4 };
        REQUIRE(myData.addElement(input, output) == 3);
        input = { 0.3, 0.3 };
        REQUIRE(myData.addElement(input, output) == 4);
        input = { 0.4, 0.2 };
        REQUIRE(myData.addElement(input, output) == 5);
        input = { 0.5, 0.1 };
        REQUIRE(myData.addElement(input, output) == 6);
        myData.stopRecording();
        
        myData.startRecording("setTwo");
        input = { 1., 4. };
        myData.addElement(input, output);
        input = { 2., -3. };
        myData.addElement(input, output);
        input = { 1., 5. };
        myData.addElement(input, output);
        input = { -2., 1. };
        myData.addElement(input, output);
        myData.stopRecording();
        
        REQUIRE(myDTW.train(myData) == true);

        std::vector<std::vector<double> > inputSet1;
        inputSet1.push_back( {1., 4.});
        inputSet1.push_back( {2., -3.});
        inputSet1.push_back( {1., 5.});
        inputSet1.push_back( {-2., 1.});
        
        REQUIRE(myDTW.run(inputSet1) == 1);
        
        std::vector<std::vector<double> > inputSet0;
        inputSet0.push_back( { 0.1, 0.5 });
        inputSet0.push_back( { 0.2, 0.4 });
        inputSet0.push_back( { 0.3, 0.3 });
        inputSet0.push_back( { 0.4, 0.2 });
        inputSet0.push_back( { 0.5, 0.1 });
        
        REQUIRE(myDTW.run(inputSet0) == 0);
    }
}

SCENARIO("Test both classes reject bad data", "[machineLearning]") {
    rapidmix::staticRegression badNN;
    rapidmix::staticClassification badKNN;
    rapidmix::trainingData myBadData;
    
    std::vector<double> input = { 0.1, 0.2, 0.3};
    std::vector<double> output = { 1.0 };
    myBadData.addElement(input,output);
    
    input = { 1.0, 2.0, 3.0, 4.0 };
    myBadData.addElement(input, output);
    REQUIRE(badNN.train(myBadData) == false);
    REQUIRE(badKNN.train(myBadData) == false);
    
    //TODO: These should return false with empty data set. I think it just crashes now. -mz
    
}


/*
 rapidmix::staticClassification labelKnn;
 labelKnn.train(myXmmData);
 std::vector<double> input = { 0.8, 0.1 };
 std::cout << "knn test: " << myXmmData.getLabel(labelKnn.run(input)[0]) << std::endl;
 
 return 0;
 
 */