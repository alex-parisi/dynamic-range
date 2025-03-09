/// Compressor.h

/**
Copyright © 2025 Alex Parisi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <optional>

/**
 * @brief Compressor configuration
 * @tparam T Type of the limiter
 */
template<typename T = double>
struct CompressorConfiguration {
    int sampleRate = 0.0;
    T threshold, attack, release, ratio;
    std::optional<T> makeupGain = std::nullopt;
    std::optional<T> kneeWidth = std::nullopt;
};

/**
 * @brief Limiter class. Implements a limiter with optional makeup gain and
 * knee width.
 * @details The limiter is implemented as a class with a process method that
 * takes a sample and processes it. The process method calculates the static
 * characteristic of the limiter, updates the gain smoothing, and applies the
 * makeup gain. The class also has a process method that takes an array of
 * samples and processes them all.
 * @tparam T Type of the limiter
 */
template<typename T = double>
class Compressor {
public:
    /**
     * @brief Public constructor
     * @param configuration Limiter configuration
     * @return A Limiter object
     */
    auto static create(CompressorConfiguration<T> configuration)
            -> std::optional<Compressor> {
        if (configuration.sampleRate <= 0) {
            return std::nullopt;
        }
        if (configuration.attack <= 0 || configuration.release <= 0) {
            return std::nullopt;
        }
        return Compressor(configuration);
    }

    /**
     * @brief Process a sample in-place
     * @param sample Sample to process
     */
    auto process(T &sample) -> void {
        T inputDecibels = 20.0 * std::log10(sample);
        T xSc = calculate_static_characteristic(inputDecibels);
        update_gain_smoothing(xSc, inputDecibels);
        T gM = m_gainSmoothing * m_config.makeupGain.value();
        T gLin = std::pow(10.0, gM / 20.0);

        sample *= gLin;
    }

    /**
     * @brief Processes an array of samples in-place
     * @param samples Pointer to the samples
     * @param count Number of samples
     */
    auto process(T *samples, const size_t count) -> void {
        for (int i = 0; i < count; i++) {
            process(samples[i]);
        }
    }

    /**
     * @brief Resets the limiter
     */
    auto reset() -> void { m_gainSmoothing = 0.0; }

private:
    /**
     * @brief Private constructor
     * @param configuration Limiter configuration
     */
    explicit Compressor(CompressorConfiguration<T> configuration) :
        m_config(configuration) {
        /// If makeupGain is not specified, calculate it:
        if (!m_config.makeupGain.has_value()) {
            m_config.makeupGain = 0.0 - calculate_static_characteristic(0.0);
        }
    }

    /**
     * @brief Calculate the static characteristic of the limiter
     * @param inputDecibels Input in decibels
     * @return The static characteristic
     */
    T calculate_static_characteristic(T inputDecibels) {
        T compressorThreshold =
                m_config.threshold -
                ((m_config.threshold - inputDecibels) / m_config.ratio);
        if (m_config.kneeWidth.has_value()) {
            /// Soft knee
            if (inputDecibels <
                (compressorThreshold - (m_config.kneeWidth.value() / 2.0))) {
                return inputDecibels;
            }
            if (inputDecibels >
                (compressorThreshold + (m_config.kneeWidth.value() / 2.0))) {
                return compressorThreshold;
            }
            T numerator = std::pow(inputDecibels - compressorThreshold +
                                           (m_config.kneeWidth.value() / 2.0),
                                   2.0);
            return inputDecibels -
                   (numerator / (2.0 * m_config.kneeWidth.value()));
        }
        /// Hard knee
        if (inputDecibels < compressorThreshold) {
            return inputDecibels;
        }
        return compressorThreshold;
    }

    /**
     * @brief Update the gain smoothing
     * @param xSc Static characteristic
     * @param inputDecibels Input in decibels
     */
    void update_gain_smoothing(T xSc, T inputDecibels) {
        T gC = xSc - inputDecibels;
        T alpha = [this, &gC]() -> T {
            if (gC <= m_gainSmoothing) {
                return std::exp(-log10(9.0) /
                                (m_config.attack * m_config.sampleRate));
            }
            return std::exp(-log10(9.0) /
                            (m_config.release * m_config.sampleRate));
        }();
        m_gainSmoothing = alpha * m_gainSmoothing + (1.0 - alpha) * gC;
    }

    /** Compressor configuration */
    CompressorConfiguration<T> m_config;

    /** Gain smoothing */
    T m_gainSmoothing = 0;
};

#endif // COMPRESSOR_H
