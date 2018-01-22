// Copyright Steinwurf ApS 2011.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo_core/basic_factory.hpp>
#include <kodo_core/coefficient_info.hpp>
#include <kodo_core/coefficient_value_access.hpp>
#include <kodo_core/common_encoder_layers.hpp>
#include <kodo_core/const_shallow_storage_layers.hpp>
#include <kodo_core/default_on_systematic_encoder.hpp>
#include <kodo_core/final_layer.hpp>
#include <kodo_core/finite_field.hpp>
#include <kodo_core/linear_block_encoder.hpp>
#include <kodo_core/payload_info.hpp>
#include <kodo_core/payload_rank_encoder.hpp>
#include <kodo_core/plain_symbol_id_size.hpp>
#include <kodo_core/plain_symbol_id_writer.hpp>
#include <kodo_core/rank_info.hpp>
#include <kodo_core/storage_aware_encoder.hpp>
#include <kodo_core/symbol_id_encoder.hpp>
#include <kodo_core/trace_layer.hpp>
#include <kodo_core/write_symbol_tracker.hpp>
#include <kodo_core/zero_symbol_encoder.hpp>

#include <kodo_rlnc/on_the_fly_generator.hpp>

namespace kodo_rlnc_c
{
using on_the_fly_encoder_stack = kodo_core::payload_rank_encoder<
    kodo_core::payload_info<
    // Codec Header API
    kodo_core::default_on_systematic_encoder<
    kodo_core::symbol_id_encoder<
    // Symbol ID API
    kodo_core::plain_symbol_id_writer<
    kodo_core::plain_symbol_id_size<
    // Coefficient Generator API
    kodo_rlnc::on_the_fly_generator<
    // Encoder API
    kodo_core::common_encoder_layers<
    kodo_core::rank_info<
    // Coefficient Storage API
    kodo_core::coefficient_value_access<
    kodo_core::coefficient_info<
    // Symbol Storage API
    kodo_core::const_shallow_storage_layers<
    // Finite Field API
    kodo_core::finite_field<
    // Trace Layer
    kodo_core::trace_layer<
    // Final Layer
    kodo_core::final_layer
    >>>>>>>>>>>>>>;
}