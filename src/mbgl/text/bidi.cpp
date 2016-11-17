#include <memory>

#include <mbgl/text/bidi.hpp>
#include <unicode/ubidi.h>
#include <unicode/ushape.h>

namespace mbgl {

BiDi::BiDi() {
    UErrorCode errorCode = U_ZERO_ERROR;
    transform = ubiditransform_open(&errorCode); // Only error is failure to allocate memory, in
                                                 // that case ubidi_transform would fall back to
                                                 // creating transform object on the fly
}

BiDi::~BiDi() {
    if (transform)
        ubiditransform_close(transform);
}

std::u16string BiDi::bidiTransform(const std::u16string& input) {
    UErrorCode errorCode = U_ZERO_ERROR;

    std::unique_ptr<UChar[]> outputText =
        std::make_unique<UChar[]>(input.size() * 2); // Maximum output of ubidi_transform is twice
                                                     // the size of input according to
                                                     // ubidi_transform.h
    uint32_t outputLength = ubiditransform_transform(
        transform, input.c_str(), static_cast<int32_t>(input.size()), outputText.get(),
        static_cast<int32_t>(input.size()) * 2,
        UBIDI_DEFAULT_LTR,     // Assume input is LTR unless strong RTL characters are found
        UBIDI_LOGICAL,         // Input is in logical order
        UBIDI_LTR,             // Output is in "visual LTR" order
        UBIDI_VISUAL,          //  ''
        UBIDI_MIRRORING_ON,    // Use mirroring lookups for things like parentheses that need mirroring
                               // in RTL text
        U_SHAPE_LETTERS_SHAPE, // Add options here for handling numbers in bidirectional text
        &errorCode);

    // If the algorithm fails for any reason, fall back to non-transformed text
    if (U_FAILURE(errorCode))
        return input;

    return std::u16string(outputText.get(), outputLength);
}

bool BiDi::baseDirectionRightToLeft(const std::u16string& input) {
    // This just looks for the first character with a strong direction property, it does not perform
    // the BiDi algorithm
    return ubidi_getBaseDirection(input.c_str(), static_cast<int32_t>(input.size())) == UBIDI_RTL;
}

} // end namespace mbgl
