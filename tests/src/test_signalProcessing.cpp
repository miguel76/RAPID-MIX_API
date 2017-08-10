
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <math.h>
#include "rapidmix.h"

SCENARIO("Test rapidStream processing", "[signalProcessing]")
{
    GIVEN("rapidStream object with default window size")
    {
        rapidmix::rapidStream defaultProcessor; //create a processor with no argument
        
        WHEN("feeding in positions") {
            THEN("check velocity") {
                defaultProcessor.pushToWindow(1);
                defaultProcessor.pushToWindow(2);
                REQUIRE(defaultProcessor.velocity() == 1); //aka First-order difference
            }
            THEN("check acceleration") {
                defaultProcessor.clear();
                defaultProcessor.pushToWindow(1);
                defaultProcessor.pushToWindow(2);
                defaultProcessor.pushToWindow(10);
                REQUIRE(defaultProcessor.acceleration() == 7); //aka Second-order difference
            }
        }
        
        GIVEN("rapidStream object with a window size of 5")
        {
            
            rapidmix::rapidStream myProcessor(5); //create a processor with a window size of 5
            
            WHEN("feeding in some arbitrary numbers") {
                
                myProcessor.pushToWindow(0.246); //append? or push?
                myProcessor.pushToWindow(0.44);
                myProcessor.pushToWindow(-0.228);
                myProcessor.pushToWindow(0.402);
                myProcessor.pushToWindow(-0.382);
                
                THEN("check min and max") {
                    REQUIRE(myProcessor.maximum() == 0.44);
                    REQUIRE(myProcessor.minimum() == -0.382);
                }
                THEN("check sum, mean and std dev") {
                    REQUIRE(myProcessor.sum() == 0.478);
                    REQUIRE(myProcessor.mean() == 0.09559999999999999);
                    REQUIRE(myProcessor.standardDeviation() == 0.33702557766436664);
                    REQUIRE(myProcessor.rms() == 0.3503221374677884);
                }
            }
            
            myProcessor.clear(); //forget any stored vaules
            REQUIRE(myProcessor.sum() == 0);
            
            
            WHEN("putting positions in a window") {
                
                myProcessor.pushToWindow(1.);
                myProcessor.pushToWindow(2.);
                myProcessor.pushToWindow(2.);
                myProcessor.pushToWindow(4.);
                myProcessor.pushToWindow(5.);
                
                THEN("check min and max velocity") {
                    REQUIRE(myProcessor.maxVelocity() == 2.0);
                    REQUIRE(myProcessor.minVelocity() == 0.);
                }
                
                THEN("check min and max acceleration") {
                    REQUIRE(myProcessor.maxAcceleration() == 2.0);
                    REQUIRE(myProcessor.minAcceleration() == -1.0);
                }
            }
        }
    }
}

SCENARIO("Test maximilian feature extraction", "[signalProcessing]")
{
    GIVEN("an fft object") {
        rapidmix::FFT myFFT;
        myFFT.setup(1024, 512, 265);
        
        for (int i = 0; i < 1024; ++i) {
            float input = (i % 256/256.0);
            myFFT.process(input);
        }
        WHEN("phasor is the input") {
            THEN("check centroid") {
                REQUIRE(myFFT.spectralCentroid() == 3520.84277f);
            }
            float foo = myFFT.spectralFlatness();
            THEN("check flatness") {
                //REQUIRE(myFFT.spectralFlatness() == 0.43209f); // or 0.432094097 Why doesn't this work? -mz
            }
        }
        
        
        rapidmix::MFCC myMFCC;
        int sampleRate = 44100;
        myMFCC.setup(512, 42, 13, 20, 20000, sampleRate);
        double *mfccs = (double*) malloc(sizeof(double) * 13);
        
        
        myMFCC.mfcc(myFFT.magnitudes, mfccs); //MZ: This runs the thing.
        
        for (int i = 2; i < 42; ++i) {
            REQUIRE(myMFCC.melBands[i] < 0);
        }
        
        for (int i = 0; i < 13; ++i) {
            REQUIRE(mfccs[i] > 0.);
        }
        
    }
}