// include/beman/execution26/detail/single_sender_value_type.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SINGLE_SENDER_VALUE_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SINGLE_SENDER_VALUE_TYPE

#include <beman/execution26/detail/value_types_of_t.hpp>
#include <beman/execution26/detail/decayed_tuple.hpp>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <variant>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, typename Env>
    struct single_sender_value_type_helper;

    template <typename Sender, typename Env>
        requires requires{
            typename ::beman::execution26::value_types_of_t<
                Sender, Env, ::std::decay_t, ::std::type_identity_t>;
            }
    struct single_sender_value_type_helper<Sender, Env>
    {
        using type = ::beman::execution26::value_types_of_t<
                Sender, Env, ::std::decay_t, ::std::type_identity_t>;
    };

    template <typename Sender, typename Env>
        requires ::std::same_as<
                ::std::variant<::std::tuple<>>,
                ::beman::execution26::value_types_of_t<
                    Sender, Env, ::std::tuple, ::std::variant>
            >
    struct single_sender_value_type_helper<Sender, Env>
    {
        using type = void;
    };

    template <typename Sender, typename Env>
        requires (not requires{
            typename ::beman::execution26::value_types_of_t<
                Sender, Env, ::std::decay_t, ::std::type_identity_t>;
            })
        && (not ::std::same_as<
                ::std::variant<::std::tuple<>>,
                ::beman::execution26::value_types_of_t<
                    Sender, Env, ::std::tuple, ::std::variant>
            >)
        && requires{
            typename ::beman::execution26::value_types_of_t<
                Sender, Env, ::beman::execution26::detail::decayed_tuple, ::std::type_identity_t>;
        }
    struct single_sender_value_type_helper<Sender, Env>
    {
        using type = ::beman::execution26::value_types_of_t<
                        Sender,
                        Env,
                        ::beman::execution26::detail::decayed_tuple,
                        ::std::type_identity_t>;
    };

    template <typename Sender, typename Env>
    using single_sender_value_type
        = single_sender_value_type_helper<Sender, Env>::type;
}

// ----------------------------------------------------------------------------

#endif