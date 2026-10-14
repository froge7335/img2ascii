export module charsets;
import std;


using namespace std;

export inline constexpr const char* STANDARD = " .:-=+*#%@";
// braille expressed as ⠁ ⠃ ⠇ ⡇ ⣇ ⣧ ⣷ ⣿
export inline constexpr const char* BRAILLE = " " "\xE2\xA0\x81" "\xE2\xA0\x83" "\xE2\xA0\x87" "\xE2\xA1\x87" "\xE2\xA3\x87" "\xE2\xA3\xA7" "\xE2\xA3\xB7" "\xE2\xA3\xBF";

export vector<string> splitUtf8(const string& s) {
    vector<string> glyphs;
 
    for (size_t i = 0; i < s.size();) {
        const auto lead = static_cast<unsigned char>(s[i]);
 
        size_t len = 0;
        if (lead < 0x80)              len = 1;
        else if ((lead >> 5) == 0x06) len = 2;
        else if ((lead >> 4) == 0x0E) len = 3;
        else if ((lead >> 3) == 0x1E) len = 4;
 
        if (len == 0 || i + len > s.size()) {
            throw invalid_argument("character ramp is not valid UTF-8");
        }
        for (size_t k = 1; k < len; ++k) {
            if ((static_cast<unsigned char>(s[i + k]) & 0xC0) != 0x80) {
                throw invalid_argument("character ramp is not valid UTF-8");
            }
        }
 
        glyphs.push_back(s.substr(i, len));
        i += len;
    }
 
    return glyphs;
}

export vector<string> presetRamp(const string& name) {
    if (name == "standard") return splitUtf8(STANDARD);
    if (name == "braille")  return splitUtf8(BRAILLE);
 
    throw invalid_argument("unknown preset '" + name + "' (expected standard, braille, or custom)");
}