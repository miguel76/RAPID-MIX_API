/*
 * signalProcessing.h
 * Created by Michael Zbyszynski on 6 Feb 2017
 * Copyright © 2017 Goldsmiths. All rights reserved.
 */

#ifndef signalProcessing_h
#define signalProcessing_h

#include "rapidMix.h"
#include "maximilian.h"
#include "maxim.h"
#include "rapidStream.h"
#include "rapidPiPoTools.h"

RAPIDMIX_BEGIN_NAMESPACE

/*
 * Wrapper for signalProcessing modules, currently a collection of typedefs
 */

typedef maxiFFT FFT;
typedef maxiMFCC MFCC;
typedef rapidStream rapidStream; //MZ: Best... typedef... EVER!

typedef pipoHost pipoHost;
typedef pipoStreamAttributes pipoStreamAttributes;


RAPIDMIX_END_NAMESPACE

#endif
