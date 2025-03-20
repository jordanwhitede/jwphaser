// PluginJWPhaser.cpp
// Jordan White (jordanwhitede@gmail.com)

#include "SC_PlugIn.hpp"
#include "JWPhaser.hpp"

static InterfaceTable* ft;

namespace JWPhaser {

JWPhaser::JWPhaser() {
    mCalcFunc = make_calc_function<JWPhaser, &JWPhaser::next>();
    next(1);
}

void JWPhaser::next(int nSamples) {

    // Audio rate input
    const float* input = in(0);

    // Control rate parameter: gain.
    const float gain = in0(1);

    // Output buffer
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain;
    }
}

} // namespace JWPhaser

PluginLoad(JWPhaserUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<JWPhaser::JWPhaser>(ft, "JWPhaser", false);
}
