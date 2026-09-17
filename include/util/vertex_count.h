#ifndef GRAPH_RECOGNITION_VERTEX_COUNT_H
#define GRAPH_RECOGNITION_VERTEX_COUNT_H

/**
 * @file vertex_count.h
 * @brief Range check shared by the fixed-n enumerators
 *
 * Every enumerator that takes a vertex count n (enumerate_<type>_..._graphs(n, ...))
 * has a range of n it can represent: the library's vertex-indexed vectors have
 * size n + 1 as an int, so n + 1 must fit in int; enumerators that keep a
 * neighbourhood or a candidate set in one 64-bit word stop at n = 63 or 64; the
 * directed canonical form stops at 33. Outside that range the enumerator cannot
 * answer, and an empty result would be a false count (every class here has a
 * member on every large n, the edgeless graph at least), so the entry function
 * throws std::invalid_argument instead. The CLIs map that exception to exit
 * status 1 (malformed / unsupported input), see util/cli_main.h.
 */

#include <climits>
#include <sstream>
#include <stdexcept>
#include <string>

namespace graph_recognition {

/**
 * @brief Largest vertex count any function of this library can represent
 *
 * Vertex-indexed vectors have size n + 1 as an int, so n = INT_MAX is not
 * representable. Enumerators without a tighter bound use this one.
 */
const int max_vertex_count = INT_MAX - 1;

/**
 * @brief Throws std::invalid_argument unless 0 <= n <= max_n
 * @param function Name of the calling entry function, for the message
 * @param n Vertex count to check
 * @param max_n Largest n the caller can represent (max_vertex_count when the
 *        caller has no tighter bound)
 *
 * The message reads "<function>: n = <n> is outside the supported range
 * 0..<max_n>".
 */
inline void require_vertex_count(const char* function, int n,
                                 int max_n = max_vertex_count) {
    if (n >= 0 && n <= max_n) return;
    std::ostringstream msg;
    msg << function << ": n = " << n << " is outside the supported range 0.."
        << max_n;
    throw std::invalid_argument(msg.str());
}

}  // namespace graph_recognition

#endif  // GRAPH_RECOGNITION_VERTEX_COUNT_H
