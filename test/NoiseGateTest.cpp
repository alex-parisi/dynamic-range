/// NoiseGateTest.cpp

#include <gtest/gtest.h>
#include "NoiseGate.h"

TEST(NoiseGateTest, CreateNoiseGateSuccess) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_TRUE(noiseGate.has_value());
}

TEST(NoiseGateTest, CreateNoiseGateFailureInvalidSampleRate) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 0,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_FALSE(noiseGate.has_value());
}

TEST(NoiseGateTest, CreateNoiseGateFailureInvalidAttack) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(0),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_FALSE(noiseGate.has_value());
}

TEST(NoiseGateTest, CreateNoiseGateFailureInvalidRelease) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(0),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_FALSE(noiseGate.has_value());
}

TEST(NoiseGateTest, ProcessSampleBelowThreshold) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_TRUE(noiseGate.has_value());
    float sample = 0.1f; // -20 dB
    noiseGate->process(sample);
    ASSERT_FLOAT_EQ(sample, 0.1f);
}

TEST(NoiseGateTest, ProcessSampleAboveThreshold) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_TRUE(noiseGate.has_value());
    float sample = 1.0f; // 0 dB
    noiseGate->process(sample);
    ASSERT_EQ(sample, 1.0f);
}

TEST(NoiseGateTest, ResetNoiseGate) {
    constexpr auto config = NoiseGateConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<NoiseGate<float>> noiseGate =
            NoiseGate<float>::create(config);
    ASSERT_TRUE(noiseGate.has_value());
    noiseGate->reset();
    float sample = 1.0f;
    noiseGate->process(sample);
    ASSERT_EQ(sample, 1.0f);
}
