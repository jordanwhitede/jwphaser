// PluginJWPhaser.hpp
// Jordan White (jordanwhitede@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"
#include <cmath>

namespace JWPhaser {

    class Cubic {
    public:
        Cubic() {}
            float process(float input, float gain, float fbAmp) {
                float sig = input * gain;
                sig = sig - (powf(sig, 3) / 3);
                sig = sig * fbAmp;
                return sig;

            }
    };

    class Tanh {
    public:
        Tanh() {}
        float process(float input, float gain, float fbAmp) {
                float sig = input * gain;
                sig = tanhf(sig) * fbAmp;
                return sig;
        }
    };

    class Wavefolder {
    public:
        Wavefolder() {}
        float process(float input, float gain, float fbAmp) {
            // wavefolding function
            float sig = input * gain;
            if (sig > 1.0) { sig = 2.0 - sig; };
            if (sig < 1.0) { sig = -2.0 - sig; };
            return sig * fbAmp;
        }
    };

    class AllpassFilter {
    public:
       // AllpassFilter() : prevInput(0.0f), prevOutput(0.0f) {}
       AllpassFilter() : x_1(0.0f), x_2(0.0f), y_1(0.0f), y_2(0.0f) {}

        float process(float input, float a1, float a2) {

            // 1st order Allpass formula
            // float output = (coeff * input) + prevInput - (coeff * prevOutput);

            // 2nd order Allpass formula
            float output = a2 * input + a1 * x_1 + x_2 - a1 * y_1 - a2 * y_2;


            // update previous input & output;
            x_2 = x_1;
            y_2 = y_1;
            x_1 = input;
            x_2 = output;
            //prevInput = input;
            //prevOutput = output;

            return output;
        }

    private:
        //float prevInput;
        //float prevOutput;
        float x_1;
        float x_2;
        float y_1;
        float y_2;
    };

class JWPhaser : public SCUnit {
public:
    JWPhaser();

    // Destructor
     ~JWPhaser();

private:
    // Calc function
    void next(int nSamples);
    void clear(int nSamples);
    // Member variables
    float lfoPhase;
    float feedbackSignal;
    float bufSize;
    float* feedbackBuffer;
    int writePhase;
    int mask;

    AllpassFilter apf[8];
    Wavefolder fold;
    Tanh tanh;
    Cubic cubic;
};

} // namespace JWPhaser
