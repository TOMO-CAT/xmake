#pragma once

#include "xutil/prefix.h"

/*! get backtrace frames
 *
 * @param frames        the backtrace frames
 * @param nframe        the backtrace frame maxn
 * @param nskip         the backtrace frame skip
 *
 * @return              the real backtrace frame count
 */
xu_size_t xu_backtrace_frames(xu_pointer_t* frames, xu_size_t nframe, xu_size_t nskip);

/*! init backtrace frame symbols
 *
 * @param frames        the backtrace frames
 * @param nframe        the backtrace frame count
 *
 * @return              the backtrace frame symbols handle
 */
xu_handle_t xu_backtrace_symbols_init(xu_pointer_t* frames, xu_size_t nframe);

/*! get backtrace frame symbol name
 *
 * @param symbols       the symbols handle
 * @param frames        the backtrace frames
 * @param nframe        the backtrace frame count
 * @param iframe        the backtrace frame index
 *
 * @return              the symbol name
 */
xu_char_t const* xu_backtrace_symbols_name(xu_handle_t symbols, xu_pointer_t* frames, xu_size_t nframe,
                                           xu_size_t iframe);

/*! exit backtrace frame symbols
 *
 * @param symbols       the symbols handle
 */
xu_void_t xu_backtrace_symbols_exit(xu_handle_t symbols);

/*! dump backtrace
 *
 * @param prefix        the backtrace prefix
 * @param frames        the backtrace frames, dump the current frames if null
 * @param nframe        the backtrace frame count
 */
xu_void_t xu_backtrace_dump(xu_char_t const* prefix, xu_pointer_t* frames, xu_size_t nframe);
