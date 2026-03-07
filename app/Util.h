#include <vector>

template <class T>
bool reorder(std::vector<T>& vec, int idx, int delta)
{
    if (idx < 0 || idx > vec.size()) {
        return false;
    }
    if (idx + delta < 0 || idx + delta > vec.size()) {
        return false;
    }

    if (delta < 0) {
        for (int i = 0; i > delta; --i) {
            std::swap(vec.at(idx + i), vec.at(idx + i - 1));
        }
    } else if (delta > 0) {
        for (int i = 0; i < delta; ++i) {
            std::swap(vec.at(idx + i), vec.at(idx + i + 1));
        }
    } else {
        return false;
    }

    return true;
}
