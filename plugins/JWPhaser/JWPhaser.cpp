// PluginJWPhaser.cpp
// Jordan White (jordanwhitede@gmail.com)

#include "SC_PlugIn.hpp"
#include "JWPhaser.hpp"
#include <cmath>

static InterfaceTable* ft;

namespace JWPhaser {



JWPhaser::JWPhaser() {
    mCalcFunc = make_calc_function<JWPhaser, &JWPhaser::next>();
    feedbackSignal = 0.0f;
    writePhase = 0;
    lfoPhase = 0.0f;
    bufSize = NEXTPOWEROFTWO((float) sampleRate() * 0.01f); // 10 ms delay

    // declare Buffer
    feedbackBuffer = (float *) RTAlloc(mWorld, bufSize * sizeof(float));
    mask = bufSize - 1;

    if (feedbackBuffer == nullptr) {
        mCalcFunc = make_calc_function<JWPhaser, &JWPhaser::clear>();

        clear(1);

        if (mWorld -> mVerbosity > -2) {
                Print("Failed to allocate memory for JWPhaser.\n");
        }
        return;
    }

    memset(feedbackBuffer, 0, bufSize * sizeof(float));
    next(1);
}

void JWPhaser::clear(int inNumSamples) {
        ClearUnitOutputs(this, inNumSamples);

}

JWPhaser::~JWPhaser(){
        RTFree(mWorld, feedbackBuffer);
}

void JWPhaser::next(int nSamples) {

    // Audio rate input
    const float* input = in(0);

    // control rate params
    const float rate = in0(1);

    float depth = in0(2);

    const float mix = in0(3);

    const float feedback = in0(4);

    // Output buffer
    float* output = out(0);

    if (depth < 0.01) {
        depth = 0.01;
    }

    if (depth > 0.99) {
        depth = 0.99;
    }

    float baseCoeff = 0.01; // i have not found a good value for this
    float modRange = depth; // or this - originally depth was multiplied by some number to keep it under control, but that's not an issue atm

    int delayInSamples = 0.01f * sampleRate();

    // phaser function
    // input + feedback. 4 Allpasses in series, feedback from here.
    for (int i = 0; i < nSamples; ++i) {

        // get feedback signal
        int readPhase = writePhase - delayInSamples;
        if (readPhase < 0) readPhase += bufSize;
        feedbackSignal = feedbackBuffer[readPhase & mask];

        // lfo here to calculate coeff modulation
        float lfoValue = sinf(lfoPhase);
        lfoPhase += 2.0f * M_PI * rate / sampleRate(); // update phase
        //if (lfoPhase > 2.0f * M_PI) lfoPhase -= 2.0f * M_PI; // wrap phase at 2pi
        lfoPhase = fmod(lfoPhase, 2.0f * M_PI); // wrap phase at 2pi

        // calculate modulated coeff
        float modulatedCoeff = baseCoeff + (lfoValue * modRange);
        modulatedCoeff = std::clamp(modulatedCoeff, 0.01f, 0.99f);

        // copy signal and mix with feedback
        const float sig = input[i] + feedbackSignal;

        // experimental - learning how to iterate, hopefully it works
        //float allpassed[4];
        float allpassed = sig;
       // AllpassFilter apf[4];
        for (int j = 0; j < 4; ++j) {
        allpassed = apf[j].process(allpassed, modulatedCoeff);
        //allpassed[1] = apf1.process(allpassed[0], modulatedCoeff);
        //allpassed[2] = apf2.process(allpassed[1], modulatedCoeff);
        //allpassed[3] = apf3.process(allpassed[2], modulatedCoeff);
    }

        float sum = (input[i] * (1.0f - mix)) + (allpassed * mix);
        output[i] = zapgremlins(sum);

        // write signal to buffer, then update writePhase
        feedbackBuffer[writePhase] = allpassed * feedback;
        writePhase = (writePhase + 1) & mask;

    }
}

} // namespace JWPhaser

PluginLoad(JWPhaserUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<JWPhaser::JWPhaser>(ft, "JWPhaser", false);
}
