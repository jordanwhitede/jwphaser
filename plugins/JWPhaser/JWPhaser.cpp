// PluginJWPhaser.cpp
// Jordan White (jordanwhitede@gmail.com)
// going well. to do: 1. make slopes/check out how control/audio rate work.
// 2. add non-linearity or option of multiple nonlinearities

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
    const float freq = in0(2);
    const float q = in0(3);
    const float mix = in0(4);
    float nonlin = in0(5);
    float gain = in0(6);
    float feedback = in0(7);
    float numStages = in0(8);

    // Output buffer
    float* output = out(0);

    if (numStages > 8) {
        numStages = 8;
    }

    if (numStages < 2) {
        numStages = 2;
    }

    int delayInSamples = 0.01f * sampleRate();

    // phaser function
    for (int i = 0; i < nSamples; ++i) {

        // get feedback signal
        int readPhase = writePhase - delayInSamples;
        if (readPhase < 0) readPhase += bufSize;
        feedbackSignal = feedbackBuffer[readPhase & mask];
/*
        // lfo here to calculate coeff modulation
        float lfoValue = sinf(lfoPhase);
        lfoPhase += 2.0f * M_PI * rate / sampleRate(); // update phase
        //if (lfoPhase > 2.0f * M_PI) lfoPhase -= 2.0f * M_PI; // wrap phase at 2pi - this may be more efficient?
        lfoPhase = fmod(lfoPhase, 2.0f * M_PI); // wrap phase at 2pi - think this one is safer?

        // scale lfo to [0, 1]
        float scaledLFOValue = 0.5 * (1 + lfoValue);
        // exponential mapping
        float expLFOValue = minFreq * powf(maxFreq / minFreq, scaledLFOValue);
        expLFOValue = expLFOValue * depth; // adding depth control


        // convert freq to coeff
        float tanValue = tanf(M_PI * expLFOValue / sampleRate());
        float coeff = (tanValue - 1.0f) / (tanValue + 1.0f);

        */

        // compute coeffs

        // q = frequency/bandwidth, so bandwidth = frequency/q
        float bw = freq / q;

        // normalize freq and bandwidth
        float wc = 2 * M_PI * freq / sampleRate();
        float wBW = 2 * M_PI * bw / sampleRate();

        // compute pole radius and angle
        float radius = 1 - (wBW / 2);
        float theta = wc; // seems redundant, leaving in for now

        // calculate a1 and a2
        float a1 = -2 * radius * cosf(theta);
        float a2 = powf(radius, 2);

        // copy signal and mix with feedback
        const float sig = input[i] + feedbackSignal;

        // series allpasses
        float allpassed = sig; // this seems redundant
        for (int j = 0; j < numStages; ++j) {
            allpassed = apf[j].process(allpassed, a1, a2);
    }

        float sum = (input[i] * (1.0f - mix)) + (allpassed * mix);
        output[i] = zapgremlins(sum);

        // add nonlinearity to fb path


        // write signal to buffer, then update writePhase
        if (nonlin = 0) {feedbackBuffer[writePhase] = cubic.process(allpassed, gain, feedback);};
        if (nonlin = 1) {feedbackBuffer[writePhase] = tanh.process(allpassed, gain, feedback);};
        if (nonlin = 2) {feedbackBuffer[writePhase] = fold.process(allpassed, gain, feedback);};
        writePhase = (writePhase + 1) & mask;

    }
}

} // namespace JWPhaser

PluginLoad(JWPhaserUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<JWPhaser::JWPhaser>(ft, "JWPhaser", false);
}
