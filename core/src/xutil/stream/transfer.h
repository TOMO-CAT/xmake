#pragma once

#include "xutil/prefix.h"

/*! the transfer func type
 *
 * @param state     the stream state
 * @param offset    the istream offset
 * @param size      the istream size, no size: -1
 * @param save      the saved size
 * @param rate      the current rate, bytes/s
 * @param priv      the func private data
 *
 * @return          xu_true: ok and continue it if need, xu_false: break it
 */
typedef xu_bool_t (*xu_transfer_func_t)(xu_size_t state, xu_hize_t offset, xu_hong_t size, xu_hize_t save,
                                        xu_size_t rate, xu_cpointer_t priv);

/*! transfer stream to stream
 *
 * @param istream   the istream
 * @param ostream   the ostream
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer(xu_stream_ref_t istream, xu_stream_ref_t ostream, xu_size_t lrate, xu_transfer_func_t func,
                      xu_cpointer_t priv);

/*! transfer stream to url
 *
 * @param istream   the istream
 * @param ourl      the output url
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_to_url(xu_stream_ref_t istream, xu_char_t const* ourl, xu_size_t lrate, xu_transfer_func_t func,
                             xu_cpointer_t priv);

/*! transfer stream to data
 *
 * @param istream   the istream
 * @param odata     the output data
 * @param osize     the output size
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_to_data(xu_stream_ref_t istream, xu_byte_t* odata, xu_size_t osize, xu_size_t lrate,
                              xu_transfer_func_t func, xu_cpointer_t priv);

/*! transfer url to url
 *
 * @param iurl      the input url
 * @param ourl      the output url
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_url(xu_char_t const* iurl, xu_char_t const* ourl, xu_size_t lrate, xu_transfer_func_t func,
                          xu_cpointer_t priv);

/*! transfer url to stream
 *
 * @param iurl      the input url
 * @param ostream   the ostream
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_url_to_stream(xu_char_t const* iurl, xu_stream_ref_t ostream, xu_size_t lrate,
                                    xu_transfer_func_t func, xu_cpointer_t priv);

/*! transfer url to data
 *
 * @param iurl      the input url
 * @param odata     the output data
 * @param osize     the output size
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_url_to_data(xu_char_t const* iurl, xu_byte_t* odata, xu_size_t osize, xu_size_t lrate,
                                  xu_transfer_func_t func, xu_cpointer_t priv);

/*! transfer data to url
 *
 * @param idata     the input data
 * @param isize     the input size
 * @param ourl      the output url
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_data_to_url(xu_byte_t const* idata, xu_size_t isize, xu_char_t const* ourl, xu_size_t lrate,
                                  xu_transfer_func_t func, xu_cpointer_t priv);

/*! transfer data to stream
 *
 * @param idata     the input data
 * @param isize     the input size
 * @param ostream   the ostream
 * @param lrate     the limit rate and no limit if 0, bytes/s
 * @param func      the save func and be optional
 * @param priv      the func private data
 *
 * @return          the saved size, failed: -1
 */
xu_hong_t xu_transfer_data_to_stream(xu_byte_t const* idata, xu_size_t isize, xu_stream_ref_t ostream, xu_size_t lrate,
                                     xu_transfer_func_t func, xu_cpointer_t priv);
