/// NoiseGate.h

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

#ifndef NOISE_GATE_H
#define NOISE_GATE_H

#include <optional>

/**
 * @brief Noise gate configuration
 * @tparam T Type of the noise gate
 */
template<typename T = double>
struct NoiseGateConfiguration {
    /** Sample rate */
    int sampleRate = 0.0;

    /** Threshold in decibels */
    T threshold = static_cast<T>(0);

    /** Attack time in milliseconds */
    std::chrono::milliseconds attack = std::chrono::milliseconds(0);

    /** Release time in milliseconds */
    std::chrono::milliseconds release = std::chrono::milliseconds(0);

    /** Makeup gain in decibels */
    std::optional<T> makeupGain = std::nullopt;

    /** Knee width in decibels */
    std::optional<T> kneeWidth = std::nullopt;
};

/**
 * @brief Noise gate class. Implements a noise gate that attenuates the signal
 * when it falls below a certain threshold.
 * @details A noise gate is a type of dynamic range compressor that attenuates
 * the signal when it falls below a certain threshold. The attenuation is
 * controlled by the attack and release times.
 * @tparam T Type of the limiter
 */
template<typename T = double>
class NoiseGate {
public:
    /**
     * @brief Public constructor
     * @param configuration Noise gate configuration
     * @return A NoiseGate object
     */
    auto static create(NoiseGateConfiguration<T> configuration)
            -> std::optional<NoiseGate> {
        if (configuration.sampleRate <= 0) {
            return std::nullopt;
        }
        if (configuration.attack.count() <= 0 ||
            configuration.release.count() <= 0) {
            return std::nullopt;
        }
        return NoiseGate(configuration);
    }

    /**
     * @brief Process a sample in-place
     * @param sample Sample to process
     */
    auto process(T &sample) -> void {
        T inputLevel = std::fabs(sample);
        if (inputLevel > m_thresholdValue) {
            m_envelope = m_attackValue * (m_envelope - inputLevel) + inputLevel;
        } else {
            m_envelope = m_releaseValue * m_envelope;
        }
        if (m_envelope < m_thresholdValue) {
            sample = static_cast<T>(0);
        }
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
     * @brief Resets the noise gate
     */
    auto reset() -> void { m_envelope = 0.0; }

private:
    /**
     * @brief Private constructor
     * @param configuration Noise gate configuration
     */
    explicit NoiseGate(NoiseGateConfiguration<T> configuration) :
        m_config(configuration) {
        calculate_intermediate_values();
    }

    /**
     * @brief Calculate intermediate values
     */
    auto calculate_intermediate_values() -> void {
        m_attackValue = std::exp(
                -1.0 / ((static_cast<T>(m_config.attack.count()) / 1000.0) *
                        m_config.sampleRate));
        m_releaseValue = std::exp(
                -1.0 / ((static_cast<T>(m_config.release.count()) / 1000.0) *
                        m_config.sampleRate));
    }

    /** Limiter configuration */
    NoiseGateConfiguration<T> m_config;

    T m_attackValue = 0.0;
    T m_releaseValue = 0.0;
    T m_thresholdValue = 0.0;
    T m_envelope = 0.0;
};

#endif // NOISE_GATE_H
