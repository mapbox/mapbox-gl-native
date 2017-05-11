#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <vector>

namespace mbgl {

/*
    Computes the longest common subsequence (LCS) of sequences A and B, represented
    by pairs of random access iterators. The result is output to the provided output
    iterator. Equality of elements is determined by the provided comparator, defaulting
    to ==.

    The algorithm used is the O(ND) time and space algorithm from:

        Myers, Eugene W. An O(ND) Difference Algorithm and Its Variations. Algorithmica
        (1986) 1: 251. http://xmailserver.org/diff2.pdf

    For understanding this algorithm, http://simplygenius.net/Article/DiffTutorial1 is
    also helpful.

    TODO: implement the O(N) space refinement presented in the paper.
*/
template <class InIt, class OutIt, class Equal>
OutIt longest_common_subsequence(InIt a, InIt endA,
                                 InIt b, InIt endB,
                                 OutIt outIt,
                                 Equal eq) {
    const std::ptrdiff_t N = endA - a;
    const std::ptrdiff_t M = endB - b;
    const std::ptrdiff_t D = N + M;

    if (D == 0) {
        return outIt;
    }

    std::vector<std::vector<std::ptrdiff_t>> vs;

    // Self-executing lambda to allow `return` to break from inner loop, and avoid shadowing `v`.
    [&] () {
        std::vector<std::ptrdiff_t> v;
        v.resize(2 * D + 1);
        v[1] = 0;

        // Core of the algorithm: greedily find farthest-reaching D-paths for increasing
        // values of D. Store the farthest-reaching endpoints found in each iteration for
        // later reconstructing the LCS.
        for (std::ptrdiff_t d = 0; d <= D; ++d) {
            for (std::ptrdiff_t k = -d; k <= d; k += 2) {
                std::ptrdiff_t x = (k == -d || (k != d && v.at(k - 1 + D) < v.at(k + 1 + D)))
                    ? v.at(k + 1 + D)       // moving down
                    : v.at(k - 1 + D) + 1;  // moving right

                std::ptrdiff_t y = x - k;

                while (x < N && y < M && eq(a[x], b[y])) {
                    x++;
                    y++;
                }

                v[k + D] = x;

                if (x >= N && y >= M) {
                    vs.push_back(v);
                    return;
                }
            }

            vs.push_back(v);
        }
    }();

    std::ptrdiff_t x = N;
    std::ptrdiff_t y = M;

    using E = typename std::iterator_traits<InIt>::value_type;
    std::vector<E> lcsReverse;

    // Reconstruct the LCS using the farthest-reaching endpoints stored above.
    for (std::ptrdiff_t d = vs.size() - 1; x > 0 || y > 0; --d) {
        const std::vector<std::ptrdiff_t>& v = vs.at(d);
        const std::ptrdiff_t k = x - y;
        const bool down = (k == -d || (k != d && v.at(k - 1 + D) < v.at(k + 1 + D)));
        const std::ptrdiff_t kPrev = down ? k + 1 : k - 1;

        x = v.at(kPrev + D);
        y = x - kPrev;

        for (std::ptrdiff_t c = v[k + D]; c != (down ? x : x + 1); --c) {
            lcsReverse.push_back(a[c - 1]);
        }
    }

    return std::copy(lcsReverse.rbegin(), lcsReverse.rend(), outIt);
}

template < typename InIt, typename OutIt >
OutIt longest_common_subsequence(InIt a, InIt endA,
                                 InIt b, InIt endB,
                                 OutIt outIt) {
    return longest_common_subsequence(a, endA, b, endB, outIt, std::equal_to<>());
}

} // namespace mbgl
