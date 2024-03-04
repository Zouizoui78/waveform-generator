#include "kissfft_wrapper.hpp"

#include "kiss_fftr.h"

namespace kissfft {

std::vector<double> fft(const std::vector<double>& samples) {
    kiss_fftr_cfg cfg = kiss_fftr_alloc(samples.size(), 0, nullptr, nullptr);

    size_t size = samples.size() / 2 + 1;
    kiss_fft_cpx* fft_output = new kiss_fft_cpx[size];

    kiss_fftr(cfg, samples.data(), fft_output);

    std::vector<double> ret;
    ret.reserve(size);
    for (int i = 0; i < size; i++) {
        ret.push_back(sqrt(pow(fft_output[i].r, 2) + pow(fft_output[i].i, 2)));
    }

    free(cfg);
    delete[] fft_output;
    return ret;
}

}