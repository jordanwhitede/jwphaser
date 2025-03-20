// PluginJWPhaser.hpp
// Jordan White (jordanwhitede@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace JWPhaser {

class JWPhaser : public SCUnit {
public:
    JWPhaser();

    // Destructor
    // ~JWPhaser();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace JWPhaser
