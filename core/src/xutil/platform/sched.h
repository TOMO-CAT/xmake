#pragma once

#include "xutil/macros/assert.h"
#include "xutil/macros/type.h"
#include "xutil/prefix.h"
#include "xutil/utils/bits.h"

#if defined(XU_CONFIG_POSIX_HAVE_SCHED_YIELD)
#    include <sched.h>
#else
#    include "xutil/platform/time.h"
#endif

/* *******************************************************
 *  macros
 * *******************************************************
 */

// sched yield
#if defined(XU_CONFIG_POSIX_HAVE_SCHED_YIELD)
#    define xu_sched_yield() sched_yield()
#else
#    define xu_sched_yield() xu_usleep(1)
#endif

// cpu affinity
#define XU_CPUSET_SIZE XU_CPU_BITSIZE
#define XU_CPUSET_FFS(pset) (xu_sched_affinity_cpu_ffs(pset))
#define XU_CPUSET_EMPTY(pset) (xu_sched_affinity_cpu_empty(pset))
#define XU_CPUSET_COUNT(pset) (xu_sched_affinity_cpu_count(pset))
#define XU_CPUSET_ZERO(pset) (xu_sched_affinity_cpu_zero(pset))
#define XU_CPUSET_SET(cpu, pset) (xu_sched_affinity_cpu_set((xu_int_t)(cpu), (pset)))
#define XU_CPUSET_CLR(cpu, pset) (xu_sched_affinity_cpu_clr((xu_int_t)(cpu), (pset)))
#define XU_CPUSET_ISSET(cpu, pset) (xu_sched_affinity_cpu_isset((xu_int_t)(cpu), (pset)))
#define XU_CPUSET_AND(pdstset, psrcset1, psrcset2) (xu_sched_affinity_cpu_and((pdstset), (psrcset1), (psrcset2)))
#define XU_CPUSET_OR(pdstset, psrcset1, psrcset2) (xu_sched_affinity_cpu_or((pdstset), (psrcset1), (psrcset2)))
#define XU_CPUSET_XOR(pdstset, psrcset1, psrcset2) (xu_sched_affinity_cpu_xor((pdstset), (psrcset1), (psrcset2)))
#define XU_CPUSET_EQUAL(set1ptr, pset2) (xu_sched_affinity_cpu_equal((set1ptr), (pset2)))

/* *******************************************************
 *  types
 * *******************************************************
 */
// the cpuset type
typedef union __xu_cpuset_t
{
    xu_byte_t cpuset[XU_CPU_BITBYTE];
    xu_size_t _cpuset;

} xu_cpuset_t, *xu_cpuset_ref_t;

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
/*! set cpu affinity for the given process id
 *
 * @param pid       the process id, set to the current process if be zero
 * @param cpuset    the cpu set
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_sched_setaffinity(xu_size_t pid, xu_cpuset_ref_t cpuset);

/*! get cpu affinity from the given process id
 *
 * @param pid       the process id, get the current process if be zero
 * @param cpuset    the cpu set
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_sched_getaffinity(xu_size_t pid, xu_cpuset_ref_t cpuset);

/* *******************************************************
 *  implementation
 * *******************************************************
 */

/*! get the first cpu index from the given cpu set
 *
 * @param pset  the cpu set
 *
 * @return      the cpu index
 */
static __xu_inline__ xu_int_t xu_sched_affinity_cpu_ffs(xu_cpuset_ref_t pset)
{
    xu_assert(pset);
#if XU_CPU_BIT64
    return (xu_int_t)xu_bits_fb1_u64_le(pset->_cpuset);
#else
    return (xu_int_t)xu_bits_fb1_u32_le(pset->_cpuset);
#endif
}

/*! get cpu count from the given cpu set
 *
 * @param pset  the cpu set
 *
 * @return      the cpu count
 */
static __xu_inline__ xu_int_t xu_sched_affinity_cpu_count(xu_cpuset_ref_t pset)
{
    xu_assert(pset);
#if XU_CPU_BIT64
    return (xu_int_t)xu_bits_cb1_u64(pset->_cpuset);
#else
    return (xu_int_t)xu_bits_cb1_u32(pset->_cpuset);
#endif
}

/*! the given cpu set is empty?
 *
 * @param pset  the cpu set
 *
 * @return      xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_sched_affinity_cpu_empty(xu_cpuset_ref_t pset)
{
    xu_assert(pset);
    return pset->_cpuset == 0;
}

/*! clear the given cpu set
 *
 * @param pset  the cpu set
 */
static __xu_inline__ xu_void_t xu_sched_affinity_cpu_zero(xu_cpuset_ref_t pset)
{
    xu_assert(pset);
    pset->_cpuset = 0;
}

/*! set cpu to the given cpu set
 *
 * @param cpu   the cpu index
 * @param pset  the cpu set
 */
static __xu_inline__ xu_void_t xu_sched_affinity_cpu_set(xu_int_t cpu, xu_cpuset_ref_t pset)
{
    xu_assert(pset && cpu >= 0 && cpu < XU_CPUSET_SIZE);
    pset->_cpuset |= ((xu_size_t)1 << cpu);
}

/*! clear cpu index in the given cpu set
 *
 * @param cpu   the cpu index
 * @param pset  the cpu set
 */
static __xu_inline__ xu_void_t xu_sched_affinity_cpu_clr(xu_int_t cpu, xu_cpuset_ref_t pset)
{
    xu_assert(pset && cpu >= 0 && cpu < XU_CPUSET_SIZE);
    pset->_cpuset &= ~((xu_size_t)1 << cpu);
}

/*! Is the given cpu index is setted?
 *
 * @param cpu   the cpu index
 * @param pset  the cpu set
 *
 * @return      exists this cpu (not zero)
 */
static __xu_inline__ xu_int_t xu_sched_affinity_cpu_isset(xu_int_t cpu, xu_cpuset_ref_t pset)
{
    xu_assert(pset && cpu >= 0 && cpu < XU_CPUSET_SIZE);
    return ((pset->_cpuset & ((xu_size_t)1 << cpu)) != (xu_size_t)0);
}

/*! compute bits(and) for the given cpusets
 *
 * @param pdstset   the dest cpu set
 * @param psrcset1  the src cpu set1
 * @param psrcset2  the src cpu set2
 */
static __xu_inline__ xu_void_t xu_sched_affinity_cpu_and(xu_cpuset_ref_t pdstset, xu_cpuset_ref_t psrcset1,
                                                         xu_cpuset_ref_t psrcset2)
{
    xu_assert(pdstset && psrcset1 && psrcset2);
    pdstset->_cpuset = psrcset1->_cpuset & psrcset2->_cpuset;
}

/*! compute bits(or) for the given cpusets
 *
 * @param pdstset   the dest cpu set
 * @param psrcset1  the src cpu set1
 * @param psrcset2  the src cpu set2
 */
static __xu_inline__ xu_void_t xu_sched_affinity_cpu_or(xu_cpuset_ref_t pdstset, xu_cpuset_ref_t psrcset1,
                                                        xu_cpuset_ref_t psrcset2)
{
    xu_assert(pdstset && psrcset1 && psrcset2);
    pdstset->_cpuset = psrcset1->_cpuset | psrcset2->_cpuset;
}

/*! compute bits(xor) for the given cpusets
 *
 * @param pdstset   the dest cpu set
 * @param psrcset1  the src cpu set1
 * @param psrcset2  the src cpu set2
 */
static __xu_inline__ xu_void_t xu_sched_affinity_cpu_xor(xu_cpuset_ref_t pdstset, xu_cpuset_ref_t psrcset1,
                                                         xu_cpuset_ref_t psrcset2)
{
    xu_assert(pdstset && psrcset1 && psrcset2);
    pdstset->_cpuset = psrcset1->_cpuset ^ psrcset2->_cpuset;
}

/*! Is equal with the given cpu sets ?
 *
 * @param pset1     the cpu set1
 * @param pset2     the cpu set2
 *
 * @return          is equal? (not zero)
 */
static __xu_inline__ xu_int_t xu_sched_affinity_cpu_equal(xu_cpuset_ref_t pset1, xu_cpuset_ref_t pset2)
{
    xu_assert(pset1 && pset2);
    return pset1->_cpuset == pset2->_cpuset;
}
