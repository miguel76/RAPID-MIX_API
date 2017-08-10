//
//  test_rapidXmmTools.cpp
//  Unit tests for rapidXmmTools
//

#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif

#include "catch.hpp"
#include "machineLearning.h"

//================================== GMM =====================================//

SCENARIO("Test GMM", "[machineLearning]")
{
    GIVEN("GMM static classification object and training dataset")
    {
        rapidmix::xmmConfig xcfg;
        xcfg.relativeRegularization = 0.1;
        
        rapidmix::trainingData myXmmData;
        std::vector<double> myXmmInput;
        std::vector<double> myXmmOutput;
        
        myXmmData.startRecording("lab1");
        myXmmInput = { 0.2, 0.7 };
        myXmmData.addElement(myXmmInput, myXmmOutput);
        myXmmData.stopRecording();
        
        myXmmData.startRecording("lab2");
        myXmmInput = { 0.8, 0.1 };
        myXmmData.addElement(myXmmInput, myXmmOutput);
        myXmmData.stopRecording();
        
        myXmmData.writeJSON("/var/tmp/testTrainingData.json");

        rapidmix::xmmStaticClassification myGmm(xcfg);
        
        myGmm.train(myXmmData);
        
        std::string filepath = "/var/tmp/modelSetDescription";
        myGmm.writeJSON(filepath);
        
        myXmmInput = { 0.2, 0.7 };
        
        WHEN("GMM model is deserialized from file")
        {
            rapidmix::xmmStaticClassification myGmmFromFile;
            myGmmFromFile.readJSON(filepath);
            
            THEN("compare results of original and deserialized models")
            {
                REQUIRE(myGmm.run(myXmmInput)[0] == myGmmFromFile.run(myXmmInput)[0]);
            }
        }

        WHEN("GMM model is deserialized from JSON stream")
        {
            rapidmix::xmmStaticClassification myGmmFromString;
            myGmmFromString.putJSON(myGmm.getJSON());
            
            THEN("compare results of original and deserialized models")
            {
                REQUIRE(myGmm.run(myXmmInput)[0] == myGmmFromString.run(myXmmInput)[0]);
            }
        }
    }
}



//================================== GMR =====================================//

SCENARIO("Test GMR", "[machineLearning]")
{
    GIVEN("GMR static regression object and training dataset")
    {
        rapidmix::xmmConfig xcfg;
        xcfg.relativeRegularization = 0.1;
        
        rapidmix::trainingData myXmmData;
        std::vector<double> myXmmInput;
        std::vector<double> myXmmOutput;
        
        myXmmData.startRecording("lab1");
        myXmmInput = { 0.2, 0.7 };
        myXmmOutput = { 1.0 };
        myXmmData.addElement(myXmmInput, myXmmOutput);
        myXmmData.stopRecording();
        
        myXmmData.startRecording("lab2");
        myXmmInput = { 0.8, 0.1 };
        myXmmOutput = { 2.0 };
        myXmmData.addElement(myXmmInput, myXmmOutput);
        myXmmData.stopRecording();
        
        myXmmData.writeJSON("/var/tmp/testTrainingData.json");
        
        rapidmix::xmmStaticRegression myGmr(xcfg);
        
        myGmr.train(myXmmData);
        
        std::string filepath = "/var/tmp/modelSetDescription";
        myGmr.writeJSON(filepath);
        
        myXmmInput = { 0.2, 0.7 };
        
        WHEN("GMM model is deserialized from file")
        {
            rapidmix::xmmStaticClassification myGmrFromFile;
            myGmrFromFile.readJSON(filepath);
            
            THEN("compare results of original and deserialized models")
            {
                double epsilon = 0.001;
                double origOut = myGmr.run(myXmmInput)[0];
                double fileOut = myGmrFromFile.run(myXmmInput)[0];
                REQUIRE(std::abs(origOut - fileOut) < epsilon);
            }
        }
        
        WHEN("GMM model is deserialized from JSON stream")
        {
            rapidmix::xmmStaticClassification myGmrFromString;
            myGmrFromString.putJSON(myGmr.getJSON());
            
            THEN("compare results of original and deserialized models")
            {
                double epsilon = 0.001;
                double origOut = myGmr.run(myXmmInput)[0];
                double stringOut = myGmrFromString.run(myXmmInput)[0];
                REQUIRE(std::abs(origOut - stringOut) < epsilon);
            }
        }
    }
}



//================================== HMM =====================================//

SCENARIO("Test HMM", "[machineLearning]")
{
    GIVEN("HMM temporal classification object and training dataset")
    {
        rapidmix::xmmConfig xcfg;
        xcfg.relativeRegularization = 0.1;
        xcfg.states = 6;
        xcfg.likelihoodWindow = 10;
        
        rapidmix::trainingData myXmmData;
        std::vector<double> myXmmOutput;
        
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
        
        rapidmix::xmmTemporalClassification myHmm(xcfg);

        myHmm.train(myXmmData);
        
        myXmmData.writeJSON("/var/tmp/testTrainingData.json");
        
        std::string filepath = "/var/tmp/modelSetDescription";
        myHmm.writeJSON(filepath);
        
        WHEN("HMM model processes the phrase it was trained with")
        {
            THEN("check its time progression output is constantly increasing")
            {
                std::vector<double> progress(myXmmPhrase.size());
                for (int i = 0; i < myXmmPhrase.size(); ++i) {
                    // we take the second value because run() returns
                    // [ likelihood_1, timeProgression_1, .... likelihood_n, timeProgression_n ]
                    progress[i] = myHmm.run(myXmmPhrase[i])[1];
                }
                std::vector<double> sortedProgress = progress;
                std::sort(sortedProgress.begin(), sortedProgress.end());
                
                REQUIRE(std::equal(progress.begin(), progress.end(), sortedProgress.begin()));
            }
        }
        
        WHEN("HMM model is deserialized from file")
        {
            rapidmix::xmmTemporalClassification myHmmFromFile;
            myHmmFromFile.readJSON(filepath);
            
            for (int i = 0; i < myXmmPhrase.size(); ++i) {

                THEN("compare results of original and deserialized models")
                {
                    REQUIRE(myHmm.run(myXmmPhrase[i]) == myHmmFromFile.run(myXmmPhrase[i]));
                }
            }
        }
        
        WHEN("HMM model is deserialized from JSON stream")
        {
            rapidmix::xmmTemporalClassification myHmmFromString;
            myHmmFromString.putJSON(myHmm.getJSON());
            
            for (int i = 0; i < myXmmPhrase.size(); ++i) {
                
                THEN("compare results of original and deserialized models")
                {
                    REQUIRE(myHmm.run(myXmmPhrase[i]) == myHmmFromString.run(myXmmPhrase[i]));
                }
            }
        }
    }
}



//================================== HMR =====================================//
////////// here we compute RMS error and validate if it stays under some epsilon

SCENARIO("Test HMR", "[machineLearning]")
{
    GIVEN("HMR temporal regression object and training dataset")
    {
        rapidmix::xmmConfig xcfg;
        xcfg.relativeRegularization = 0.001;
        xcfg.absoluteRegularization = 0.001;
        xcfg.states = 6;
        xcfg.likelihoodWindow = 10;
        
        rapidmix::trainingData myXmmData;
        
        myXmmData.startRecording("lab1");
        std::vector <std::pair <std::vector<double>, std::vector<double>>> myXmmPhrase = {
            { { 0.0, 0.0 }, { 1.0 } },
            { { 0.0, 0.0 }, { 2.0 } },
            { { 0.0, 0.0 }, { 3.0 } },
            { { 0.0, 0.0 }, { 4.0 } },
            { { 0.1, 1.1 }, { 5.0 } },
            { { 0.2, 1.2 }, { 6.0 } },
            { { 0.3, 1.3 }, { 7.0 } },
            { { 0.4, 1.4 }, { 8.0 } },
            { { 0.5, 1.5 }, { 9.0 } },
            { { 0.6, 1.6 }, { 10.0 } },
            { { 0.7, 1.7 }, { 11.0 } },
            { { 0.8, 1.8 }, { 12.0 } },
            { { 0.9, 1.9 }, { 13.0 } },
            { { 1.0, 2.0 }, { 14.0 } },
            { { 1.1, 2.1 }, { 15.0 } },
            { { 1.2, 2.2 }, { 16.0 } },
            { { 1.3, 2.3 }, { 17.0 } },
            { { 1.4, 2.4 }, { 18.0 } },
            { { 1.5, 2.5 }, { 19.0 } },
            { { 1.6, 2.6 }, { 20.0 } }
        };
        
        for (int i = 0; i < myXmmPhrase.size(); ++i) {
            myXmmData.addElement(myXmmPhrase[i].first, myXmmPhrase[i].second);
        }
        myXmmData.stopRecording();
        
        rapidmix::xmmTemporalRegression myHmr(xcfg);
        
        myHmr.train(myXmmData);
        
        myXmmData.writeJSON("/var/tmp/testTrainingData.json");
        
        std::string filepath = "/var/tmp/modelSetDescription";
        myHmr.writeJSON(filepath);
        
        WHEN("HMR model processes the phrase it was trained with")
        {
            THEN("check its regression output is the same as the output example")
            {
                int cnt = 0;
                double sum = 0;
                
                for (int i = 0; i < myXmmPhrase.size(); ++i) {
                    std::vector<double> regression;
                    regression = myHmr.run(myXmmPhrase[i].first);
                
                    for (int j = 0; j < regression.size(); ++j) {
                        double delta = regression[j] - myXmmPhrase[i].second[j];
                        sum += delta * delta;
                        cnt++;
                    }
                }
                
                sum = std::sqrt(sum / cnt);
                
                // totally arbitrary epsilon value :
                double epsilon = 1.0;
                REQUIRE(sum <= epsilon);
            }
        }
        
        WHEN("HMR model is deserialized from file")
        {
            rapidmix::xmmTemporalRegression myHmrFromFile;
            myHmrFromFile.readJSON(filepath);
            
            for (int i = 0; i < myXmmPhrase.size(); ++i) {
                
                THEN("compare results of original and deserialized models")
                {
                    int cnt = 0;
                    double sum = 0;

                    for (int i = 0; i < myXmmPhrase.size(); ++i) {
                        std::vector<double> r1, r2;
                        r1 = myHmr.run(myXmmPhrase[i].first);
                        r2 = myHmrFromFile.run(myXmmPhrase[i].first);
                    
                        for (int j = 0; j < r1.size(); ++j) {
                            double delta = r1[j] - r2[j];
                            sum += delta * delta;
                            cnt++;
                        }
                    }
                    
                    sum = std::sqrt(sum / cnt);

                    // totally arbitrary epsilon value :
                    double epsilon = 0.1;
                    REQUIRE(sum <= epsilon);
                }
            }
        }
        
        WHEN("HMR model is deserialized from JSON stream")
        {
            rapidmix::xmmTemporalRegression myHmrFromString;
            myHmrFromString.putJSON(myHmr.getJSON());
            
            for (int i = 0; i < myXmmPhrase.size(); ++i) {
                
                THEN("compare results of original and deserialized models")
                {
                    int cnt = 0;
                    double sum = 0;
                    
                    for (int i = 0; i < myXmmPhrase.size(); ++i) {
                        std::vector<double> r1, r2;
                        r1 = myHmr.run(myXmmPhrase[i].first);
                        r2 = myHmrFromString.run(myXmmPhrase[i].first);
                        
                        for (int j = 0; j < r1.size(); ++j) {
                            double delta = r1[j] - r2[j];
                            sum += delta * delta;
                            cnt++;
                        }
                    }
                    
                    sum = std::sqrt(sum / cnt);
                    
                    // totally arbitrary epsilon value :
                    double epsilon = 0.1;
                    REQUIRE(sum <= epsilon);
                }
            }
        }
    }
}
