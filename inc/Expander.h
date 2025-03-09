/// Expander.h

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

#ifndef EXPANDER_H
#define EXPANDER_H

#include <optional>

/**
 * @brief Expander configuration
 * @tparam T Type of the expander
 */
template<typename T = double>
struct ExpanderConfiguration {
    /** Sample rate */
    int sampleRate = 0.0;

    /** Threshold in decibels */
    T threshold = static_cast<T>(0);

    /** Attack time in milliseconds */
    std::chrono::milliseconds attack = std::chrono::milliseconds(0);

    /** Release time in milliseconds */
    std::chrono::milliseconds release = std::chrono::milliseconds(0);

    /** The ratio of the expander */
    T ratio = static_cast<T>(0);

    /** Makeup gain in decibels */
    std::optional<T> makeupGain = std::nullopt;

    /** Knee width in decibels */
    std::optional<T> kneeWidth = std::nullopt;
};

/**
 * @brief Expander class. Implements an expander with optional makeup gain
 * and knee width.
 * @details An expander is a device that increases the dynamic range of an audio
 * signal. It does so by amplifying the signal when it falls below a certain
 * threshold. The amount of amplification is determined by the ratio parameter.
 * The attack and release parameters determine how quickly the expander responds
 * to changes in the input signal. The makeup gain parameter is used to decrease
 * the overall gain of the signal after expansion. The knee width parameter
 * determines the width of the knee of the expander's characteristic curve. A
 * larger knee width results in a smoother transition between the expanded and
 * unexpanded regions of the characteristic curve.
 * @tparam T Type of the expander
 */
template<typename T = double>
class Expander {
public:
    /**
     * @brief Public constructor
     * @param configuration Expander configuration
     * @return An Expander object
     */
    auto static create(ExpanderConfiguration<T> configuration)
            -> std::optional<Expander> {
        if (configuration.sampleRate <= 0) {
            return std::nullopt;
        }
        if (configuration.attack.count() <= 0 ||
            configuration.release.count() <= 0) {
            return std::nullopt;
        }
        return Expander(configuration);
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
     * @brief Resets the expander
     */
    auto reset() -> void { m_gainSmoothing = 0.0; }

    /**
     * @brief Set the expander configuration after creation
     * @param configuration Expander configuration
     */
    auto set_configuration(ExpanderConfiguration<T> configuration) -> void {
        m_config = configuration;
        calculate_intermediate_values();
    }

private:
    /**
     * @brief Private constructor
     * @param configuration Expander configuration
     */
    explicit Expander(ExpanderConfiguration<T> configuration) :
        m_config(configuration) {
        /// If makeupGain is not specified, calculate it:
        if (!m_config.makeupGain.has_value()) {
            m_config.makeupGain = 0.0 - calculate_static_characteristic(0.0);
        }
        calculate_intermediate_values();
    }

    /**
     * @brief Calculate intermediate values
     */
    auto calculate_intermediate_values() -> void {
        m_attackValue =
                std::exp(-log10(9.0) /
                         ((static_cast<T>(m_config.attack.count()) / 1000.0) *
                          m_config.sampleRate));
        m_releaseValue =
                std::exp(-log10(9.0) /
                         ((static_cast<T>(m_config.release.count()) / 1000.0) *
                          m_config.sampleRate));
    }

    /**
     * @brief Calculate the static characteristic of the expander
     * @param inputDecibels Input in decibels
     * @return The static characteristic
     */
    T calculate_static_characteristic(T inputDecibels) {
        T expanderThreshold =
                m_config.threshold +
                ((inputDecibels - m_config.threshold) / m_config.ratio);
        if (m_config.kneeWidth.has_value()) {
            /// Soft knee
            if (inputDecibels >
                (expanderThreshold + (m_config.kneeWidth.value() / 2.0))) {
                return inputDecibels;
            }
            if (inputDecibels <
                (expanderThreshold - (m_config.kneeWidth.value() / 2.0))) {
                return expanderThreshold;
            }
            T numerator = std::pow(inputDecibels - expanderThreshold -
                                           (m_config.kneeWidth.value() / 2.0),
                                   2.0);
            return inputDecibels +
                   (numerator / (2.0 * m_config.kneeWidth.value()));
        }
        /// Hard knee
        if (inputDecibels > expanderThreshold) {
            return inputDecibels;
        }
        return expanderThreshold;
    }

    /**
     * @brief Update the gain smoothing
     * @param xSc Static characteristic
     * @param inputDecibels Input in decibels
     */
    void update_gain_smoothing(T xSc, T inputDecibels) {
        T gC = xSc - inputDecibels;
        T alpha = [this, &gC]() -> T {
            if (gC >= m_gainSmoothing) {
                return m_attackValue;
            }
            return m_releaseValue;
        }();
        m_gainSmoothing = alpha * m_gainSmoothing + (1.0 - alpha) * gC;
    }

    /** Expander configuration */
    ExpanderConfiguration<T> m_config;

    /** Gain smoothing */
    T m_gainSmoothing = 0;

    T m_attackValue = 0;
    T m_releaseValue = 0;
};

#endif // EXPANDER_H
