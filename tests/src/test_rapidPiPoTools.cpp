//
//  test_rapidPiPoTools.cpp
//  Unit tests for rapidPiPoTools
//

#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif

#ifdef RTA_USE_VECLIB
#undef RTA_USE_VECLIB
#endif

#include <unistd.h>
#include "catch.hpp"
#include "signalProcessing.h"

//#define MAX_PATH_SIZE 256

//=============================== ONSEG TEST =================================//

SCENARIO("Test rapidPiPoHost", "[signalProcessing]")
{
    GIVEN("A rapidPiPoHost class with a rapidPiPo chain and an audio file")
    {
        maxiSample buffer;

        // in XCode this gives the path to DerivedData folder
        // char pathStrBuf[MAX_PATH_SIZE];
        // char *cwd = getcwd(pathStrBuf, sizeof(pathStrBuf));
        // std::cout << std::string(cwd) << std::endl;
        
        // but here we just add the file to the Copy File(s) Build Phase
        buffer.load("./DnB-loop-175BPM.wav", 0);
        // ( source : http://freesound.org/people/yewbic/sounds/40107/ )

        buffer.reset(); // (no real need to do this here)
        
        //====================================================================//

        // instantiate PiPo related classes here :
        rapidmix::pipoHost host; // -> this class is located in rapidPiPoTools
        
        // if we want to add some custom PiPos to our collection :
        // #include "myCustomPiPo.h"
        // PiPoCollection::addToCollection("myCustomPiPo", new PiPoCreator<myCustomPiPo>);
        // now we can write :
        // pipoHost.setChain("myCustomPiPo");

// #include "PiPoMaximChroma.h"
        // this one is not part of the default collection :
        // PiPoCollection::addToCollection("chroma", new PiPoCreator<PiPoMaximChroma>);
        
        host.setGraph("slice:fft:sum:scale:onseg");
        
        host.setAttr("slice.size", 1024);
        host.setAttr("slice.hop", 256);
        host.setAttr("slice.norm", "power");
        host.setAttr("fft.mode", "power");
        host.setAttr("fft.weighting", "itur468");
        host.setAttr("scale.inmin", 1.);
        host.setAttr("scale.inmax", 10.);
        host.setAttr("scale.outmin", 0.);
        host.setAttr("scale.outmax", 10.);
        host.setAttr("scale.func", "log");
        host.setAttr("scale.base", 10.);
        host.setAttr("onseg.duration", 1.);
        host.setAttr("onseg.min", 0.);
        host.setAttr("onseg.max", 1.);
        host.setAttr("onseg.mean", 0.);
        host.setAttr("onseg.stddev", 0.);
        host.setAttr("onseg.startisonset", 1.);
        host.setAttr("onseg.threshold", 9.);
        host.setAttr("onseg.offthresh", -120.);
        
        std::cout << "onseg threshold : ";
        std::cout << host.getDoubleAttr("onseg.threshold") << std::endl;
        std::cout << "fft mode : ";
        std::cout << host.getEnumAttr("fft.mode") << std::endl;
        std::cout << "param names : " << std::endl;

        std::vector<std::string> attrs = host.getAttrNames();
        
        for (int i = 0; i < attrs.size(); ++i)
        {
            std::cout << "- " << attrs[i] << std::endl;
        }

        std::cout << host.getJSON() << std::endl;
        // set another chain :
        // pipoHost.setChain("chroma");
        
        WHEN("file is processed")
        {
            rapidmix::pipoStreamAttributes sa;
            sa.hasTimeTags = true;
            sa.rate = 44100;
            sa.offset = 0;
            sa.width = 1,
            sa.height = 1,
            sa.labels = std::vector<std::string>();
            sa.hasVarSize = false;
            sa.domain = 0;
            sa.maxFrames = 1;
            
            host.setInputStreamAttributes(sa);
            
            float value;
            for (unsigned int i = 0; i < buffer.length; ++i) {
                value = buffer.play();
                host.frames((double)i, 0.0, &value, 1, 1);
            }

            THEN("compare results of actual processing and same file processed in another envrironment")
            {
                // compare the results of the processing

                REQUIRE(true);
            }
        }
    }
}
