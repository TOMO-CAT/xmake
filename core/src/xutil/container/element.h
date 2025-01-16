#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the element ref type
typedef struct __xu_element_t* xu_element_ref_t;

/*! the element data hash function type
 *
 * @param element               the element
 * @param data                  the element data
 * @param mask                  the hash mask
 * @param index                 the hash index
 *
 * @return                      the hash value
 */
typedef xu_size_t (*xu_element_hash_func_t)(xu_element_ref_t element, xu_cpointer_t data, xu_size_t mask,
                                            xu_size_t index);

/*! the element data compare function type
 *
 * @param element               the element
 * @param ldata                 the left-hand data
 * @param rdata                 the right-hand data
 *
 * @return                      equal: 0, 1: >, -1: <
 */
typedef xu_long_t (*xu_element_comp_func_t)(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata);

/*! the element data function type
 *
 * @param element               the element
 * @param buff                 the element data address
 *
 * @return                      the element data
 */
typedef xu_pointer_t (*xu_element_data_func_t)(xu_element_ref_t element, xu_cpointer_t buff);

/*! the element data string function type
 *
 * @param element               the element
 * @param data                  the element data
 * @param cstr                  the string buffer
 * @param maxn                  the string buffer maximum size
 *
 * @return                      the string pointer
 */
typedef xu_char_t const* (*xu_element_cstr_func_t)(xu_element_ref_t element, xu_cpointer_t data, xu_char_t* cstr,
                                                   xu_size_t maxn);

/*! the element free function type
 *
 * @param element               the element
 * @param buff                  the element buffer
 */
typedef xu_void_t (*xu_element_free_func_t)(xu_element_ref_t element, xu_pointer_t buff);

/*! the element duplicate function type
 *
 * allocate a new element and copy the element data
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param data                  the element data
 */
typedef xu_void_t (*xu_element_dupl_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data);

/*! the element replace function type
 *
 * free the previous element data and duplicate the new data
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param data                  the element data
 */
typedef xu_void_t (*xu_element_repl_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data);

/*! the element copy function type
 *
 * only copy the element data and not allocate new element
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param data                  the element data
 */
typedef xu_void_t (*xu_element_copy_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data);

/*! the elements free function type
 *
 * free some elements
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param size                  the element count
 */
typedef xu_void_t (*xu_element_nfree_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_size_t size);

/*! the elements duplicate function type
 *
 * duplicate some elements
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param data                  the element data
 * @param size                  the element count
 */
typedef xu_void_t (*xu_element_ndupl_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data,
                                             xu_size_t size);

/*! the elements replace function type
 *
 * replace some elements
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param data                  the element data
 * @param size                  the element count
 */
typedef xu_void_t (*xu_element_nrepl_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data,
                                             xu_size_t size);

/*! the elements copy function type
 *
 * copy some elements
 *
 * @param element               the element
 * @param buff                  the element buffer
 * @param data                  the element data
 * @param size                  the element count
 */
typedef xu_void_t (*xu_element_ncopy_func_t)(xu_element_ref_t element, xu_pointer_t buff, xu_cpointer_t data,
                                             xu_size_t size);

// the element type
typedef enum __xu_element_type_t
{
    XU_ELEMENT_TYPE_NULL = 0 //!< null
    ,
    XU_ELEMENT_TYPE_LONG = 1 //!< long
    ,
    XU_ELEMENT_TYPE_SIZE = 2 //!< size
    ,
    XU_ELEMENT_TYPE_UINT8 = 3 //!< uint8
    ,
    XU_ELEMENT_TYPE_UINT16 = 4 //!< uint16
    ,
    XU_ELEMENT_TYPE_UINT32 = 5 //!< uint32
    ,
    XU_ELEMENT_TYPE_STR = 6 //!< string
    ,
    XU_ELEMENT_TYPE_PTR = 7 //!< pointer
    ,
    XU_ELEMENT_TYPE_MEM = 8 //!< memory
    ,
    XU_ELEMENT_TYPE_OBJ = 9 //!< object
    ,
    XU_ELEMENT_TYPE_TRUE = 10 //!< true
    ,
    XU_ELEMENT_TYPE_USER = 11 //!< the user-defined type

} xu_element_type_t;

// the element type
typedef struct __xu_element_t
{
    /// the element type
    xu_uint16_t type;

    /// the element flag
    xu_uint16_t flag;

    /// the element size
    xu_uint16_t size;

    /// the priv data
    xu_cpointer_t priv;

    /// the hash function
    xu_element_hash_func_t hash;

    /// the compare function
    xu_element_comp_func_t comp;

    /// the data function
    xu_element_data_func_t data;

    /// the string function
    xu_element_cstr_func_t cstr;

    /// the free element
    xu_element_free_func_t free;

    /// the duplicate function
    xu_element_dupl_func_t dupl;

    /// the replace function
    xu_element_repl_func_t repl;

    /// the copy function
    xu_element_copy_func_t copy;

    /// the free elements function
    xu_element_nfree_func_t nfree;

    /// the duplicate elements function
    xu_element_ndupl_func_t ndupl;

    /// the replace elements function
    xu_element_nrepl_func_t nrepl;

    /// the copy elements function
    xu_element_ncopy_func_t ncopy;

} xu_element_t;

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! the null element, no space
 *
 * @return          the element
 */
xu_element_t xu_element_null(xu_noarg_t);

/*! the true element, no space
 *
 * .e.g for hash data
 *
 * @return          the element
 */
xu_element_t xu_element_true(xu_noarg_t);

/*! the long element
 *
 * @return          the element
 */
xu_element_t xu_element_long(xu_noarg_t);

/*! the size element
 *
 * @return          the element
 */
xu_element_t xu_element_size(xu_noarg_t);

/*! the uint8 element
 *
 * @return          the element
 */
xu_element_t xu_element_uint8(xu_noarg_t);

/*! the uint16 element for
 *
 * @return          the element
 */
xu_element_t xu_element_uint16(xu_noarg_t);

/*! the uint32 element
 *
 * @return          the element
 */
xu_element_t xu_element_uint32(xu_noarg_t);

/*! the string element
 *
 * @param is_case   is case?
 *
 * @return          the element
 */
xu_element_t xu_element_str(xu_bool_t is_case);

/*! the pointer element
 *
 * @note if the free function have been hooked, the nfree need hook too.
 *
 * @param free      the element free function
 * @param priv      the private data of the element free function
 *
 * @return          the element
 */
xu_element_t xu_element_ptr(xu_element_free_func_t free, xu_cpointer_t priv);

/*! the object element
 *
 * @return          the element
 */
xu_element_t xu_element_obj(xu_noarg_t);

/*! the memory element with the fixed space
 *
 * @param size      the element size
 * @param free      the element free function
 * @param priv      the private data of the element free function
 *
 * @return          the element
 */
xu_element_t xu_element_mem(xu_size_t size, xu_element_free_func_t free, xu_cpointer_t priv);
