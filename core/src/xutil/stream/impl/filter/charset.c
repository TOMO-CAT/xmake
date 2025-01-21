#include "xutil/charset/chartset.h"
#include "xutil/stream/filter.h"

// the charset filter type
typedef struct __xu_filter_charset_t
{
    // the filter base
    xu_filter_t base;

    // the from type
    xu_size_t ftype;

    // the to type
    xu_size_t ttype;

} xu_filter_charset_t;

static __xu_inline__ xu_filter_charset_t* xu_filter_charset_cast(xu_filter_t* filter)
{
    // check
    xu_assert_and_check_return_val(filter && filter->type == XU_FILTER_TYPE_CHARSET, xu_null);
    return (xu_filter_charset_t*)filter;
}
static xu_long_t xu_filter_charset_spak(xu_filter_t* filter, xu_static_stream_ref_t istream,
                                        xu_static_stream_ref_t ostream, xu_long_t sync)
{
    // check
    xu_filter_charset_t* cfilter = xu_filter_charset_cast(filter);
    xu_assert_and_check_return_val(
        cfilter && XU_CHARSET_TYPE_OK(cfilter->ftype) && XU_CHARSET_TYPE_OK(cfilter->ttype) && istream && ostream, -1);

    // spak it
    xu_long_t real = xu_charset_conv_bst(cfilter->ftype, cfilter->ttype, istream, ostream);

    // no data and sync end? end it
    if (!real && sync < 0) real = -1;

    // ok?
    return real;
}
static xu_bool_t xu_filter_charset_ctrl(xu_filter_t* filter, xu_size_t ctrl, xu_va_list_t args)
{
    // check
    xu_filter_charset_t* cfilter = xu_filter_charset_cast(filter);
    xu_assert_and_check_return_val(cfilter && ctrl, xu_false);

    // ctrl
    switch (ctrl)
    {
    case XU_FILTER_CTRL_CHARSET_GET_FTYPE:
    {
        // the pftype
        xu_size_t* pftype = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_break(pftype);

        // get ftype
        *pftype = cfilter->ftype;

        // ok
        return xu_true;
    }
    case XU_FILTER_CTRL_CHARSET_SET_FTYPE:
    {
        // set ftype
        cfilter->ftype = (xu_size_t)xu_va_arg(args, xu_size_t);

        // ok
        return xu_true;
    }
    case XU_FILTER_CTRL_CHARSET_GET_TTYPE:
    {
        // the pttype
        xu_size_t* pttype = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_break(pttype);

        // get ttype
        *pttype = cfilter->ttype;

        // ok
        return xu_true;
    }
    case XU_FILTER_CTRL_CHARSET_SET_TTYPE:
    {
        // set ttype
        cfilter->ttype = (xu_size_t)xu_va_arg(args, xu_size_t);

        // ok
        return xu_true;
    }
    default: break;
    }
    return xu_false;
}

xu_filter_ref_t xu_filter_init_from_charset(xu_size_t fr, xu_size_t to)
{
    // done
    xu_bool_t            ok     = xu_false;
    xu_filter_charset_t* filter = xu_null;
    do
    {
        // make filter
        filter = xu_malloc0_type(xu_filter_charset_t);
        xu_assert_and_check_break(filter);

        // init filter
        if (!xu_filter_init((xu_filter_t*)filter, XU_FILTER_TYPE_CHARSET)) break;
        filter->base.spak = xu_filter_charset_spak;
        filter->base.ctrl = xu_filter_charset_ctrl;

        // init the from and to charset
        filter->ftype = fr;
        filter->ttype = to;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit filter
        xu_filter_exit((xu_filter_ref_t)filter);
        filter = xu_null;
    }

    // ok?
    return (xu_filter_ref_t)filter;
}
