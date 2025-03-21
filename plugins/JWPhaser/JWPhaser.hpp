// PluginJWPhaser.hpp
// Jordan White (jordanwhitede@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace JWPhaser {

    class AllpassFilter {
    public:
        AllpassFilter() : prevInput(0.0f), prevOutput(0.0f) {}

        float process(float input, float coeff) {

            // all pass formula, check again
            float output = (coeff * input) + prevInput - (coeff * prevOutput);

            // update previous input & output;
            prevInput = input;
            prevOutput = output;

            return output;
        }

    private:
        float prevInput;
        float prevOutput;
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

    AllpassFilter apf[4];
};

} // namespace JWPhaser
