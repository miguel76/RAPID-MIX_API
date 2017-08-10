

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include "json.h"

#include "rapidmix.h"

SCENARIO("Test facade for RapidLib classification", "[machineLearning]")
{
    GIVEN("a static classification object")
    {
        rapidmix::staticClassification myKnn;
        rapidmix::trainingData myData;
        
        WHEN("column names are set") {
            THEN("verify them") {
                std::vector<std::string> columnNames = { "X", "Y" };
                myData.setColumnNames(columnNames);
                REQUIRE(myData.getColumnNames() == columnNames);
            }
        }
        
        std::vector<double> input = { 0.2, 0.7 };
        std::vector<double> output = { 3.0 };
        int testVar = myData.addElement(input, output);
        
        input = { 2.0, 44.2 };
        output = { 20.14 };
        int testVar2 = myData.addElement(input, output);
        
        WHEN("data are added") {
            THEN("check id") {
                REQUIRE(testVar == 1);
                REQUIRE(testVar2 == 2);
            }
        }
        
        bool trainTest = myKnn.train(myData);
        WHEN ("training happens") {
            THEN("check") {
                REQUIRE(trainTest == true);
            }
        }
        
        REQUIRE(myKnn.getK()[0] == 1);
        WHEN ("k is set") {
            myKnn.setK(0, 2);
            THEN("check that it has changed") {
                REQUIRE(myKnn.getK()[0] == 2);
            }
        }
        
        std::vector<double> inputVec = { 2.0, 44.2 };
        
        WHEN("models are serialized and deserialized") {
            std::string filepath2 = "/var/tmp/modelSetDescription_knn.json";
            myKnn.writeJSON(filepath2);
            
            rapidmix::staticClassification myKnnFromString;
            myKnnFromString.putJSON(myKnn.getJSON());
            
            rapidmix::staticClassification myKnnFromFile;
            myKnnFromFile.readJSON(filepath2);
            
            //std::cout << "knn before: " << myKnn.run(inputVec)[0] << std::endl;
            //std::cout << "knn from string: " << myKnnFromString.run(inputVec)[0] << std::endl;
            //std::cout << "knn from file: " << myKnnFromFile.run(inputVec)[0] << std::endl;
            
            THEN("check that results are the same") {
                REQUIRE(myKnn.run(inputVec)[0] == myKnnFromString.run(inputVec)[0]);
                REQUIRE(myKnn.run(inputVec)[0] == myKnnFromFile.run(inputVec)[0]);
            }
        }
        
        std::string trainingPath = "/var/tmp/testTrainingData.json";
        myData.writeJSON(trainingPath);
        rapidmix::trainingData myStoredData;
        bool msdTest = myStoredData.readJSON(trainingPath);
        rapidmix::staticClassification myStoredKnn;
        bool msdTrain = myStoredKnn.train(myStoredData);
        
        WHEN("data are saved as a JSON") {
            THEN("check that the file reads and parses") {
                REQUIRE(msdTest == true);
            }
            
            THEN("check that the new model is the same as the old model") {
                REQUIRE(msdTrain == true);
                REQUIRE(myKnn.run(inputVec)[0] == myStoredKnn.run(inputVec)[0]);
            }
        }
    }
}

SCENARIO("Test facade for RapidLib svm classification", "[machineLearning]")
{
    GIVEN("an svm classification object")
    {
        rapidmix::staticClassification mySVM(classification::svm);
        rapidmix::trainingData myData;
        
        std::vector<double> input = { 0.2, 0.7 };
        std::vector<double> output = { 3.0 };
        int testVar = myData.addElement(input, output);
        
        input = { 2.0, 44.2 };
        output = { 20.14 };
        int testVar2 = myData.addElement(input, output);
        
        mySVM.train(myData);
    }
}

SCENARIO("Test facade for RapidLib regression", "[machineLearning]")
{
    GIVEN("an regression object")
    {
        rapidmix::staticRegression myNN;
        rapidmix::trainingData myData;
        std::vector<double> input = { 0.2, 0.7 };
        std::vector<double> output = { 3.0 };
        int testVar = myData.addElement(input, output);
        
        input = { 2.0, 44.2 };
        output = { 20.14 };
        int testVar2 = myData.addElement(input, output);
        
        WHEN("data are added") {
            THEN("check id") {
                REQUIRE(testVar == 1);
                REQUIRE(testVar2 == 2);
            }
        }
        
        bool trainTest = myNN.train(myData);
        WHEN ("training happens") {
            THEN("check") {
                REQUIRE(trainTest == true);
            }
        }
        
        std::vector<double> inputVec = { 2.0, 44.2 };
        
        
        WHEN("models are serialized and deserialized") {
            std::string filepath = "/var/tmp/modelSetDescription.json";
            myNN.writeJSON(filepath);
            
            rapidmix::staticRegression myNNfromString;
            myNNfromString.putJSON(myNN.getJSON());
            
            rapidmix::staticRegression myNNfromFile;
            myNNfromFile.readJSON(filepath);
            
            
            //std::cout << "before: " << myNN.run(inputVec)[0] << std::endl;
            //std::cout << "from string: " << myNNfromString.run(inputVec)[0] << std::endl;
            //std::cout << myNNfromString.getJSON() << std::endl;
            //std::cout << "from file: " << myNNfromFile.run(inputVec)[0] << std::endl;
            
            
            THEN("check that results are the same") {
                REQUIRE(myNN.run(inputVec)[0] == myNNfromString.run(inputVec)[0]);
                REQUIRE(myNN.run(inputVec)[0] == myNNfromFile.run(inputVec)[0]);
            }
        }
    }
}

SCENARIO("Test RapidLib regression with a bigger vector", "[machineLearning]")
{
    GIVEN("a static regeression object")
    {
        rapidmix::staticRegression bigVector;
        rapidmix::trainingData trainingSet2;
        
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-0.5,0.5);
        int vecLength = 64;
        for (int j = 0; j < vecLength; ++j) {
            std::vector<double> input;
            std::vector<double> output;
            for (int i = 0; i < vecLength; ++i) {
                input.push_back(distribution(generator));
            }
            output = { distribution(generator) };
            trainingSet2.addElement(input, output);
        }
        bigVector.train(trainingSet2);
        std::vector<double> inputVec2;
        for (int i=0; i < vecLength; ++i) {
            inputVec2.push_back(distribution(generator));
        }
        REQUIRE(isfinite(bigVector.process(inputVec2)[0]));
    }
}

SCENARIO("Test facade around XMM", "[machineLearning]")
{
    GIVEN("some XMM")
    {
        //====================== rapidXmmTools test ==============================//
        
        rapidmix::trainingData myXmmData;
        std::vector<double> myXmmInput;
        std::vector<double> myXmmOutput;
        xmmToolConfig xcfg;
        
        //////////////////// testing GMM :
        
        myXmmData.startRecording("lab1");
        myXmmInput = { 0.2, 0.7 };
        myXmmData.addElement(myXmmInput, myXmmOutput);
        myXmmData.stopRecording();
        
        myXmmData.startRecording("lab2");
        myXmmInput = { 0.8, 0.1 };
        myXmmData.addElement(myXmmInput, myXmmOutput);
        myXmmData.stopRecording();
        
        xcfg.relativeRegularization = 0.1;
        rapidmix::xmmStaticClassification myGmm(xcfg);
        
        if (myGmm.train(myXmmData)) {
            std::cout << "GMM training successful !" << std::endl;
            std::vector<double> res;
            std::vector<double> input = { 0.2, 0.7 };
            res = myGmm.run(input);
            for (auto &elt : res) std::cout << elt << " ";
            std::cout << std::endl;
            
            input = { 0.8, 0.1 };
            res = myGmm.run(input);
            for (auto &elt : res) std::cout << elt << " ";
            std::cout << std::endl;     } else {
                std::cout << "GMM training impossible !" << std::endl;
            }
        
        //mz label test
        
        WHEN("rapidLib classifier trains on the same data") {
            rapidmix::staticClassification labelKnn;
            labelKnn.train(myXmmData);
            THEN("verify it returns the same label") {
                REQUIRE(myXmmData.getLabel(labelKnn.run({ 0.2, 0.7 })[0]) == "lab1");
            }
        }
        //////////////////// testing HMM :
        
        // phrases seem to need a minimum length related to the number of states
        // otherwise sometimes we get stuck into NaN values
        xcfg.states = 6;
        // we don't really need this here as we only have 1 label ...
        xcfg.likelihoodWindow = 10;
        rapidmix::xmmTemporalClassification myHmm(xcfg);
        
        myXmmData = rapidmix::trainingData(); // reset the whole set
        myXmmData.startRecording("lab1");
        std::vector<std::vector<double>> myXmmPhrase = {
            { 0.0, 0.0 },
            { 0.0, 0.0 },
            { 0.0, 0.0 },
            { 0.0, 0.0 },
            { 0.1, 1.1 },
            { 0.2, 1.2 },
            { 0.3, 1.3 },
            { 0.4, 1.4 },
            { 0.5, 1.5 },
            { 0.6, 1.6 },
            { 0.7, 1.7 },
            { 0.8, 1.8 },
            { 0.9, 1.9 },
            { 1.0, 2.0 },
            { 1.1, 2.1 },
            { 1.2, 2.2 },
            { 1.3, 2.3 },
            { 1.4, 2.4 },
            { 1.5, 2.5 },
            { 1.6, 2.6 }
        };
        for (int i = 0; i < myXmmPhrase.size(); ++i) {
            myXmmData.addElement(myXmmPhrase[i], myXmmOutput);
        }
        myXmmData.stopRecording();
        
        if (myHmm.train(myXmmData)) {
            std::cout << "HMM training successful !" << std::endl;
            std::vector<double> res;
            for (int i = 0; i < myXmmPhrase.size(); ++i) {
                res = myHmm.run(myXmmPhrase[i]);
                for (auto &elt : res) std::cout << elt << " ";
                std::cout << std::endl;
            }
        } else {
            std::cout << "HMM training impossible !" << std::endl;
        }
        
    }
}


