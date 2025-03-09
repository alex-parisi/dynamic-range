# Dynamic Range Processors

This repository contains a collection of dynamic range processors in modern C++. The processors are implemented as a template class, which allows for easy use with different data types. The processors are implemented in a header-only style, so no linking is required.


---

### Usage:

To use the processors, simply include the header files in your project and create an instance of a processor. Here is an example of how to use a processor:

```cpp
#include "Limiter.h"

/// Create a limiter object
constexpr auto config = LimiterConfiguration<float>{.sampleRate = 48000,   // Hz
                                                    .threshold = -10.0f,   // dB
                                                    .attack = 0.01f,
                                                    .release = 0.1f,
                                                    .makeupGain = 5.0f,    // dB
                                                    .kneeWidth = 5.0f};    // dB
std::optional<Limiter<float>> limiter = Limiter<float>::create(config);

if (limiter.has_value()) {

    /// Process a vector of samples in-place:
    std::vector<float> samplesVector = {1.0f, 0.5f, 0.25f};
    limiter->process(samplesVector);

    /// Process an array of samples in-place:
    std::array<float, 3> samplesArray = {1.0f, 0.5f, 0.25f};
    limiter->process(samplesArray.data(), samplesArray.size());
    
}
```

When using a processor's `::create` function, it returns an `std::optional` so you can check if the object was created successfully. If the object was not created successfully, the `std::optional` will be empty. This is useful to prevent initializing a processor with invalid parameters.

If the processor is invalid, and you try to process data with it, the object will not modify the data and will return false.

---

### Notes:

- It's always recommended to template the objects as `double` for the best precision to and reduce the chance of encountering quantization noise. 