//  :copyright: (c) 2015-2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup AutoCOM
 *  \brief Templates to automate dispparams construction.
 */

#pragma once

#include "variant.hpp"
#include "util/enum.hpp"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable:4800)
#endif          // MSVC


namespace autocom
{
// ENUM
// ----


/** \brief Strongly-typed enum wrapper for dispatch identifiers.
 */
enum class DispatchFlags: WORD
{
    METHOD          = DISPATCH_METHOD,
    GET             = DISPATCH_PROPERTYGET,
    PUT             = DISPATCH_PROPERTYPUT,
    PUTREF          = DISPATCH_PROPERTYPUTREF,
};

AUTOCOM_FLAGS(DispatchFlags)

// CONSTANTS
// ---------

extern const DispatchFlags GET;
extern const DispatchFlags METHOD;
extern const DispatchFlags PUT;
extern const DispatchFlags PUTREF;

// FORWARD
// -------

class DispParams;

// SFINAE
// ------

template <typename T>
using IsDispParams = std::is_convertible<T&, DispParams&>;

template <typename T>
constexpr bool IsDispParamsV = IsDispParams<T>::value;

// FOWARDERS
// ---------


/** \brief No-op sink for argument-free params.
 */
inline void setArg(VariantList &variants,
    const size_t index)
{}


/** \brief Forward parameter to rvargs.
 */
template <typename T>
void setArg(VariantList &variants,
    const size_t index,
    T &&t)
{
    // use set(), rather than variant.set(), so clear() is not called
    set(variants[index], AUTOCOM_FWD(t));
}


/** \brief Forward parameters in argument order.
 *
 *  DISPPARAMS::rgvargs takes arguments in reverse sorted order,
 *  so the argument list is the index count after unpacking the item.
 *
 *  Example, Arg0->3, Arg1->2, Arg2->1, Arg3->0
 */
template <
    typename T,
    typename... Ts
>
void setArg(VariantList &variants,
    const size_t index,
    T t,
    Ts&&... ts)
{
    setArg(variants, sizeof...(Ts), t);
    setArg(variants, sizeof...(Ts)-1, AUTOCOM_FWD(ts)...);
}


// OBJECTS
// -------


/** \brief Wrapper around DISPPARAMS object.
 */
class DispParams
{
protected:
    DISPPARAMS dp = {nullptr, nullptr, 0, 0};
    VariantList vargs;
    DISPID named = DISPID_PROPERTYPUT;

    void reset(const bool useNamed);

public:
    DispParams();
    DispParams(const DispParams &other);
    DispParams & operator=(const DispParams &other);
    DispParams(DispParams &&other);
    DispParams & operator=(DispParams &&other);

    ~DispParams();

    // SETTERS
    template <typename... Ts>
    void setArgs(Ts&&... ts);
    void setFlags(const DispatchFlags flags);

    // GETTERS
    DISPPARAMS * params();
    const DISPPARAMS * params() const;
    const VariantList & args() const;
};


// IMPLEMENTATION
// --------------


/** \brief Set argument list for dispparams.
 */
template <typename... Ts>
void DispParams::setArgs(Ts&&... ts)
{
    constexpr size_t size = sizeof...(Ts);
    vargs.resize(size);
    setArg(vargs, sizeof...(Ts)-1, AUTOCOM_FWD(ts)...);
    dp.rgvarg = const_cast<Variant*>(vargs.data());
    dp.cArgs = size;
}

}   /* autocom */

#ifdef _MSC_VER
#   pragma warning(pop)
#endif          // MSVC
