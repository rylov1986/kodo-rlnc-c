// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <kodo_rlnc_c/encoder.h>
#include <kodo_rlnc_c/decoder.h>

/// @example sparse_seed.c
///
/// Simple example showing how to encode and decode a block of memory using
/// the sparse seed codec. The coding coefficients are not sent directly
/// in the coded packets. The packet header includes a random seed and a
/// coefficient density value, and the decoder reconstructs the original
/// coding coefficients using these values. The maximum size of the header
/// is 9 bytes, which yields minimal overhead for a large number of symbols.

void trace_callback(const char* zone, const char* data, void* context)
{
    (void) context;

    if (strcmp(zone, "decoder_state") == 0 ||
        strcmp(zone, "symbol_coefficients_before_read_symbol") == 0)
    {
        printf("%s:\n", zone);
        printf("%s\n", data);
    }
}

int main()
{
    // Seed the random number generator to produce different data every time
    srand(time(NULL));

    // Set the number of symbols and the symbol size
    uint32_t symbols = 10;
    uint32_t symbol_size = 100;

    // Here we select the finite field to use.
    // For the sparse seed codec, we can choose krlnc_binary, krlnc_binary4 or
    // krlnc_binary8 (krlnc_binary is recommended for high performance)
    int32_t finite_field = krlnc_binary8;

    krlnc_encoder_factory_t encoder_factory = krlnc_new_encoder_factory(
        finite_field, symbols, symbol_size);

    krlnc_decoder_factory_t decoder_factory = krlnc_new_decoder_factory(
        finite_field, symbols, symbol_size);

    // Select the sparse seed codec that we wish to use
    krlnc_encoder_factory_set_coding_vector_format(
        encoder_factory, krlnc_sparse_seed);
    krlnc_decoder_factory_set_coding_vector_format(
        decoder_factory, krlnc_sparse_seed);

    krlnc_encoder_t encoder = krlnc_encoder_factory_build(encoder_factory);
    krlnc_decoder_t decoder = krlnc_decoder_factory_build(decoder_factory);

    // The coding vector density on the encoder is set with
    // krlnc_encoder_set_density().
    // Note: the density can be adjusted at any time. This feature can be used
    // to adapt to changing network conditions.
    printf("The density defaults to: %0.2f\n", krlnc_encoder_density(encoder));
    krlnc_encoder_set_density(encoder, 0.4);
    printf("The density was set to: %0.2f\n", krlnc_encoder_density(encoder));
    // A low density setting can lead to a large number of redundant symbols.
    // In practice, the value should be tuned to the specific scenario.

    // Allocate some storage for a "payload". The payload is what we would
    // eventually send over a network.
    uint32_t bytes_used;
    uint32_t payload_size = krlnc_encoder_payload_size(encoder);
    uint8_t* payload = (uint8_t*) malloc(payload_size);

    // Allocate input and output data buffers
    uint32_t block_size = krlnc_encoder_block_size(encoder);
    uint8_t* data_in = (uint8_t*) malloc(block_size);
    uint8_t* data_out = (uint8_t*) malloc(block_size);

    // Fill the input buffer with random data
    uint32_t i = 0;
    for (; i < block_size; ++i)
        data_in[i] = rand() % 256;

    // Assign the data buffers to the encoder and decoder
    krlnc_encoder_set_const_symbols(encoder, data_in, block_size);
    krlnc_decoder_set_mutable_symbols(decoder, data_out, block_size);

    // Install a custom trace function for the decoder
    krlnc_decoder_set_trace_callback(decoder, trace_callback, NULL);

    uint32_t lost_payloads = 0;
    uint32_t received_payloads = 0;
    while (!krlnc_decoder_is_complete(decoder))
    {
        // The encoder will use a certain amount of bytes of the payload buffer
        bytes_used = krlnc_encoder_write_payload(encoder, payload);
        printf("Payload generated by encoder, bytes used = %d\n", bytes_used);

        // Simulate a channel with a 50% loss rate
        if (rand() % 2)
        {
            lost_payloads++;
            printf("Symbol lost on channel\n\n");
            continue;
        }

        // Pass the generated packet to the decoder
        received_payloads++;
        krlnc_decoder_read_payload(decoder, payload);
        printf("Payload processed by decoder, current rank = %d\n\n",
               krlnc_decoder_rank(decoder));
    }

    printf("Number of lost payloads: %d\n", lost_payloads);
    printf("Number of received payloads: %d\n", received_payloads);

    // Check that we properly decoded the data
    if (memcmp(data_in, data_out, block_size) == 0)
    {
        printf("Data decoded correctly\n");
    }
    else
    {
        printf("Unexpected failure to decode, please file a bug report :)\n");
    }

    // Free the allocated buffers and the kodo objects
    free(data_in);
    free(data_out);
    free(payload);

    krlnc_delete_encoder(encoder);
    krlnc_delete_decoder(decoder);

    krlnc_delete_encoder_factory(encoder_factory);
    krlnc_delete_decoder_factory(decoder_factory);

    return 0;
}
