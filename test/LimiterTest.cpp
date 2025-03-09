/// LimiterTest.cpp

#include <gtest/gtest.h>
#include "Limiter.h"

TEST(LimiterTest, CreateLimiterSuccess) {
    constexpr auto config = LimiterConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Limiter<float>> limiter =
            Limiter<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
}

TEST(LimiterTest, CreateLimiterFailureInvalidSampleRate) {
    constexpr auto config = LimiterConfiguration<float>{
            .sampleRate = 0,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Limiter<float>> limiter =
            Limiter<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(LimiterTest, CreateLimiterFailureInvalidAttack) {
    constexpr auto config = LimiterConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(0),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Limiter<float>> limiter =
            Limiter<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(LimiterTest, CreateLimiterFailureInvalidRelease) {
    constexpr auto config =
            LimiterConfiguration<float>{.sampleRate = 48000,
                                        .threshold = -10.0f,
                                        .attack = std::chrono::milliseconds(10),
                                        .release = std::chrono::milliseconds(0),
                                        .makeupGain = 5.0f,
                                        .kneeWidth = 5.0f};
    const std::optional<Limiter<float>> limiter =
            Limiter<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(LimiterTest, ProcessSampleBelowThreshold) {
    constexpr auto config = LimiterConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Limiter<float>> limiter = Limiter<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    float sample = 0.1f; // -20 dB
    limiter->process(sample);
    ASSERT_FLOAT_EQ(sample, 0.1f);
}

TEST(LimiterTest, ProcessSampleAboveThreshold) {
    constexpr auto config = LimiterConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Limiter<float>> limiter = Limiter<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    float sample = 1.0f; // 0 dB
    limiter->process(sample);
    ASSERT_LT(sample, 1.0f);
}

TEST(LimiterTest, ResetLimiter) {
    constexpr auto config = LimiterConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Limiter<float>> limiter = Limiter<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    limiter->reset();
    float sample = 1.0f;
    limiter->process(sample);
    ASSERT_LT(sample, 1.0f);
}
