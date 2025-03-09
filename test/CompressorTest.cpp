/// CompressorTest.cpp

#include <gtest/gtest.h>
#include "Compressor.h"

TEST(CompressorTest, CreateCompressorSuccess) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
}

TEST(CompressorTest, CreateCompressorFailureInvalidSampleRate) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 0,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(CompressorTest, CreateCompressorFailureInvalidAttack) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(0),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(CompressorTest, CreateCompressorFailureInvalidRelease) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(0),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    const std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_FALSE(limiter.has_value());
}

TEST(CompressorTest, ProcessSampleBelowThreshold) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    float sample = 0.1f; // -20 dB
    limiter->process(sample);
    ASSERT_FLOAT_EQ(sample, 0.1f);
}

TEST(CompressorTest, ProcessSampleAboveThreshold) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    float sample = 1.0f; // 0 dB
    limiter->process(sample);
    ASSERT_LT(sample, 1.0f);
}

TEST(CompressorTest, ResetCompressor) {
    constexpr auto config = CompressorConfiguration<float>{
            .sampleRate = 48000,
            .threshold = -10.0f,
            .attack = std::chrono::milliseconds(10),
            .release = std::chrono::milliseconds(100),
            .ratio = 5.0f,
            .makeupGain = 5.0f,
            .kneeWidth = 5.0f};
    std::optional<Compressor<float>> limiter =
            Compressor<float>::create(config);
    ASSERT_TRUE(limiter.has_value());
    limiter->reset();
    float sample = 1.0f;
    limiter->process(sample);
    ASSERT_LT(sample, 1.0f);
}
