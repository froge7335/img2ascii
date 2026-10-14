#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"
#define SIMPLEWEBP_IMPLEMENTATION
#include "third_party/simplewebp.h"

#include <stdlib.h>
#include <string.h>

extern "C" unsigned char* decode_rgba(const unsigned char* data, int size, int* w, int* h, const char** err) {
    if (size >= 12 && memcmp(data, "RIFF", 4) == 0 && memcmp(data + 8, "WEBP", 4) == 0) {
        simplewebp* webp = nullptr;
        // simplewebp only reads from the buffer
        simplewebp_error result = simplewebp_load_from_memory(const_cast<unsigned char*>(data), size, nullptr, &webp);
        if (result != SIMPLEWEBP_NO_ERROR) {
            *err = simplewebp_get_error_text(result);
            return nullptr;
        }

        size_t width = 0, height = 0;
        simplewebp_get_dimensions(webp, &width, &height);

        auto* pixels = static_cast<unsigned char*>(malloc(width * height * 4));
        result = pixels ? simplewebp_decode(webp, pixels, nullptr) : SIMPLEWEBP_ALLOC_ERROR;
        simplewebp_unload(webp);
        if (result != SIMPLEWEBP_NO_ERROR) {
            free(pixels);
            *err = simplewebp_get_error_text(result);
            return nullptr;
        }

        *w = static_cast<int>(width);
        *h = static_cast<int>(height);
        return pixels;
    }

    unsigned char* pixels = stbi_load_from_memory(data, size, w, h, nullptr, 4);
    if (!pixels) *err = stbi_failure_reason();
    return pixels;
}

extern "C" void free_rgba(unsigned char* pixels) {
    free(pixels);
}
