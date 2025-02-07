#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/prefix.h"
#include "xutil/container/element.h"


/* *******************************************************
 * macros
 */

// the item maxn
#define XU_BLOOM_FILTER_ITEM_MAXN_MICRO                 (1 << 16)
#define XU_BLOOM_FILTER_ITEM_MAXN_SMALL                 (1 << 20)
#define XU_BLOOM_FILTER_ITEM_MAXN_LARGE                 (1 << 24)

/* *******************************************************
 * types
 */

/*! the bloom filter type
 *
 * A Bloom filter is a space-efficient probabilistic data structure,
 * conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.
 * False positive matches are possible, but false negatives are not;
 * i.e. a query returns either "possibly in set" or "definitely not in set".
 * Elements can be added to the set, but not removed (though this can be addressed with a "counting" filter).
 * The more elements that are added to the set, the larger the probability of false positives.
 *
 * Assume that a hash function selects each array position with equal probability.
 * If m is the number of bits in the array, and k is the number of hash functions,
 * then the probability that a certain bit is not set to 1 by a certain hash function
 * during the insertion of an element is then
 * 1 - 1 / m
 *
 * The probability that it is not set to 1 by any of the hash functions is
 * (1 - 1/ m) ^ k
 *
 * If we have inserted n elements, the probability that a certain bit is still 0 is
 * (1 - 1/ m) ^ kn
 *
 * the probability that it is 1 is therefore
 * 1 - ((1 - 1/ m) ^ kn)
 *
 * Now test membership of an element that is not in the set.
 * Each of the k array positions computed by the hash functions is 1 with a probability as above.
 * The probability of all of them being 1,
 * which would cause the algorithm to erroneously claim that the element is in the set, is often given as
 * p = (1 - ((1 - 1/ m) ^ kn))^k ~= (1 - e^(-kn/m))^k
 *
 * For a given m and n, the value of k (the number of hash functions) that minimizes the probability is
 * k = (m / n) * ln2 ~= (m / n) * (9 / 13)
 *
 * which gives
 * 2 ^ -k ~= 0.6185 ^ (m / n)
 *
 * The required number of bits m, given n (the number of inserted elements)
 * and a desired false positive probability p (and assuming the optimal value of k is used)
 * can be computed by substituting the optimal value of k in the probability expression above:
 * p = (1 - e ^-(m/nln2)n/m))^(m/nln2)
 *
 * which can be simplified to:
 * lnp = -m/n * (ln2)^2
 *
 * This optimal results in:
 * s = m/n = -lnp / (ln2 * ln2) = -log2(p) / ln2
 * k = s * ln2 = -log2(p) <= note: this k will be larger
 *
 * compute s(m/n) for given k and p:
 * p = (1 - e^(-kn/m))^k = (1 - e^(-k/s))^k
 * => lnp = k * ln(1 - e^(-k/s))
 * => (lnp) / k = ln(1 - e^(-k/s))
 * => e^((lnp) / k) = 1 - e^(-k/s)
 * => e^(-k/s) = 1 - e^((lnp) / k) = 1 - (e^lnp)^(1/k) = 1 - p^(1/k)
 * => -k/s = ln(1 - p^(1/k))
 * => s = -k / ln(1 - p^(1/k)) and define c = p^(1/k)
 * => s = -k / ln(1 - c)) and ln(1 + x) ~= x - 0.5x^2 while x < 1
 * => s ~= -k / (-c-0.5c^2) = 2k / (2c + c * c)
 *
 * so
 * c = p^(1/k)
 * s = m / n = 2k / (2c + c * c)
 */
typedef __xu_typeref__(bloom_filter);

/// the probability of false positives
typedef enum __xu_bloom_filter_probability_e
{
    XU_BLOOM_FILTER_PROBABILITY_0_1         = 3 ///!< 1 / 2^3 = 0.125 ~= 0.1
,   XU_BLOOM_FILTER_PROBABILITY_0_01        = 6 ///!< 1 / 2^6 = 0.015625 ~= 0.01
,   XU_BLOOM_FILTER_PROBABILITY_0_001       = 10 ///!< 1 / 2^10 = 0.0009765625 ~= 0.001
,   XU_BLOOM_FILTER_PROBABILITY_0_0001      = 13 ///!< 1 / 2^13 = 0.0001220703125 ~= 0.0001
,   XU_BLOOM_FILTER_PROBABILITY_0_00001     = 16 ///!< 1 / 2^16 = 0.0000152587890625 ~= 0.00001
,   XU_BLOOM_FILTER_PROBABILITY_0_000001    = 20 ///!< 1 / 2^20 = 0.00000095367431640625 ~= 0.000001

}xu_bloom_filter_probability_e;

/* *******************************************************
 * interfaces
 */

/*! init bloom filter
 *
 * @note not supports iterator
 *
 * @param probability   the probability of false positives
 * @param hash_count    the hash count: < 16
 * @param item_maxn     the item maxn
 * @param element       the element only for hash
 *
 * @return              the bloom filter
 */
xu_bloom_filter_ref_t   xu_bloom_filter_init(xu_size_t probability, xu_size_t hash_count, xu_size_t item_maxn, xu_element_t element);

/*! exit bloom filter
 *
 * @param bloom_filter  the bloom filter
 */
xu_void_t               xu_bloom_filter_exit(xu_bloom_filter_ref_t bloom_filter);

/*! clear bloom filter
 *
 * @param bloom_filter  the bloom filter
 */
xu_void_t               xu_bloom_filter_clear(xu_bloom_filter_ref_t bloom_filter);

/*! set data to the bloom filter
 *
 * @code
 * if (xu_bloom_filter_set(filter, data))
 * {
 *     xu_trace_i("this data not exists, set ok!");
 * }
 * else
 * {
 *     xu_trace_i("this data have been existed, set failed!");
 *
 *     // note: maybe false positives
 * }
 * @endcode
 *
 * @param bloom_filter  the bloom filter
 * @param data          the item data
 *
 * @return              return xu_false if the data have been existed, otherwise set it and return xu_true
 */
xu_bool_t               xu_bloom_filter_set(xu_bloom_filter_ref_t bloom_filter, xu_cpointer_t data);

/*! get data to the bloom filter
 *
 * @code
 * if (xu_bloom_filter_get(filter, data))
 * {
 *     xu_trace_i("this data have been existed, get ok!");
 *
 *     // note: maybe false positives
 * }
 * else
 * {
 *     xu_trace_i("this data not exists, get failed!");
 * }
 * @endcode
 *
 * @param bloom_filter  the bloom filter
 * @param data          the item data
 *
 * @return              return xu_true if the data exists, otherwise return xu_false
 */
xu_bool_t               xu_bloom_filter_get(xu_bloom_filter_ref_t bloom_filter, xu_cpointer_t data);

/* get data
 *
 * @param bloom_filter  the bloom filter
 *
 * @return              the bloom filter data
 */
xu_byte_t const*        xu_bloom_filter_data(xu_bloom_filter_ref_t bloom_filter);

/* get data size
 *
 * @param bloom_filter  the bloom filter
 *
 * @return              the bloom filter data size
 */
xu_size_t               xu_bloom_filter_size(xu_bloom_filter_ref_t bloom_filter);

/* set data, we can use this to copy data from another bloom filter
 *
 * @param bloom_filter  the bloom filter
 * @param data          the bloom filter data
 * @param size          the bloom filter size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_bloom_filter_data_set(xu_bloom_filter_ref_t bloom_filter, xu_byte_t const* data, xu_size_t size);
