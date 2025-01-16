#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the path maximum
#define XU_PATH_MAXN (4096)

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! translate and normalize the path
 *
 * @param path          the path
 * @param size          the path size, optional
 * @param maxn          the path maxn
 * @param mode          the translate mode
 *
 * @return              the real path size
 */
xu_size_t xu_path_translate(xu_char_t* path, xu_size_t size, xu_size_t maxn, xu_bool_t normalize);

/*! translate and normalize the path to the given destinate path
 *
 * basic:
 * - transform the path separator
 * - expand the user directory with the prefix: ~
 * - remove tail separator
 * - reduce the repeat path separator, "////" => "/"
 *
 * normalize:
 * - reduce "././" => "."
 * - reduce "/xxx/.." => "/"
 *
 * @param path          the path
 * @param size          the path size, optional
 * @param data          the data
 * @param maxn          the data maxn
 * @param mode          the translate mode
 *
 * @return              the real path size
 */
xu_size_t xu_path_translate_to(xu_char_t const* path, xu_size_t size, xu_char_t* data, xu_size_t maxn,
                               xu_bool_t normalize);

/*! the path is absolute?
 *
 * @param path          the path
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_path_is_absolute(xu_char_t const* path);

/*! get the absolute path which relative to the current directory
 *
 * @param path          the path
 * @param data          the path data
 * @param maxn          the path maxn
 *
 * @return              the absolute path
 */
xu_char_t const* xu_path_absolute(xu_char_t const* path, xu_char_t* data, xu_size_t maxn);

/*! get the absolute path which relative to the given root directory
 *
 * @param root          the root path
 * @param path          the path
 * @param data          the path data
 * @param maxn          the path maxn
 *
 * @return              the absolute path
 */
xu_char_t const* xu_path_absolute_to(xu_char_t const* root, xu_char_t const* path, xu_char_t* data, xu_size_t maxn);

/*! get the path which relative to the current directory
 *
 * @param path          the path
 * @param data          the path data
 * @param maxn          the path maxn
 *
 * @return              the relative path
 */
xu_char_t const* xu_path_relative(xu_char_t const* path, xu_char_t* data, xu_size_t maxn);

/*! get the path which relative to the given root directory
 *
 * @param root          the root path
 * @param path          the path
 * @param data          the path data
 * @param maxn          the path maxn
 *
 * @return              the relative path
 */
xu_char_t const* xu_path_relative_to(xu_char_t const* root, xu_char_t const* path, xu_char_t* data, xu_size_t maxn);

/*! get the directory of path
 *
 * @param path          the path
 * @param data          the path data
 * @param maxn          the path maxn
 *
 * @return              the directory of path
 */
xu_char_t const* xu_path_directory(xu_char_t const* path, xu_char_t* data, xu_size_t maxn);
