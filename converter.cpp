import std;
import charsets;

using namespace std;

extern "C" unsigned char* decode_rgba(const unsigned char* data, int size, int* w, int* h, const char** err);
extern "C" void free_rgba(unsigned char* pixels);

constexpr const char* usage = "usage: converter <standard|braille|custom \"<ramp>\"> <low|med|high|xhigh> <image>";

int resolutionColumns(const string& name) {
    if (name == "low")   return 40;
    if (name == "med")   return 80;
    if (name == "high")  return 160;
    if (name == "xhigh") return 240;

    throw invalid_argument("unknown resolution '" + name + "' (expected low, med, high, or xhigh)");
}

vector<unsigned char> readFile(const string& path) {
    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("cannot open '" + path + "'");

    return vector<unsigned char>(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    try {
        vector<string> ramp;
        int next = 2;
        if (argc > 1 && string(argv[1]) == "custom") {
            if (argc != 5) throw invalid_argument("wrong number of arguments");
            ramp = splitUtf8(argv[2]);
            if (ramp.empty()) throw invalid_argument("custom ramp is empty");
            next = 3;
        } else {
            if (argc != 4) throw invalid_argument("wrong number of arguments");
            ramp = presetRamp(argv[1]);
        }

        const int columns = resolutionColumns(argv[next]);
        const string path = argv[next + 1];

        const vector<unsigned char> bytes = readFile(path);
        int w = 0, h = 0;
        const char* err = nullptr;
        unsigned char* raw = decode_rgba(bytes.data(), static_cast<int>(bytes.size()), &w, &h, &err);
        if (!raw) throw runtime_error("cannot decode '" + path + "': " + err);
        const vector<unsigned char> pixels(raw, raw + static_cast<size_t>(w) * h * 4);
        free_rgba(raw);

        const int cols = min(columns, w);
        // 0.5 because terminal cells are about twice as tall as they are wide
        const int rows = clamp(static_cast<int>(lround(cols * static_cast<double>(h) / w * 0.5)), 1, h);
        const int n = static_cast<int>(ramp.size());

        string out;
        for (int r = 0; r < rows; ++r) {
            const int y0 = r * h / rows, y1 = (r + 1) * h / rows;
            for (int c = 0; c < cols; ++c) {
                const int x0 = c * w / cols, x1 = (c + 1) * w / cols;

                double sum = 0;
                for (int y = y0; y < y1; ++y) {
                    for (int x = x0; x < x1; ++x) {
                        const unsigned char* p = &pixels[(static_cast<size_t>(y) * w + x) * 4];
                        sum += (0.2126 * p[0] + 0.7152 * p[1] + 0.0722 * p[2]) / 255 * p[3] / 255;
                    }
                }

                const double lum = sum / ((y1 - y0) * (x1 - x0));
                out += ramp[min(n - 1, static_cast<int>(lum * n))];
            }
            out += '\n';
        }

        cout << out;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << "\n" << usage << "\n";
        return 1;
    }
}
