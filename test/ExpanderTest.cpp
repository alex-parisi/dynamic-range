/// ExpanderTest.cpp

#include <gtest/gtest.h>
#include "Expander.h"

TEST(ExpanderTest, CreateExpanderSuccess) {
    constexpr auto config = ExpanderConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Expander<float>> limiter =
            Expander<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
}

TEST(ExpanderTest, CreateExpanderFailureInvalidSampleRate) {
    constexpr auto config = ExpanderConfiguration<float>{
            .sampleRate = 0,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Expander<float>> limiter =
            Expander<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(ExpanderTest, CreateExpanderFailureInvalidAttack) {
    constexpr auto config = ExpanderConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(0),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Expander<float>> limiter =
            Expander<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(ExpanderTest, CreateExpanderFailureInvalidRelease) {
    constexpr auto config = ExpanderConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(0),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Expander<float>> limiter =
            Expander<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(ExpanderTest, ResetExpander) {
    constexpr auto config = ExpanderConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Expander<float>> limiter = Expander<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    limiter->reset();
    float sample = 1.0f;
    limiter->process(sample);
    ASSERT_EQ(sample, 1.0f);
}
