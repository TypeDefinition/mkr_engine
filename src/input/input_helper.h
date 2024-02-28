#pragma once

#include "input/input.h"
#include "input/keycode.h"

namespace mkr {
    /**
        \brief Input Utility Class
    */
    class input_helper {
    public:
        input_helper() = delete;

        /**
            \brief Generate an input mask given a input context, controller index and keycode.
            \param _input_context The input context.
            \param _controller_index The controller index.
            \param _keycode The keycode.
            \return An input_mask representing the given input_context, controller_index and keycode.
        */
        static input_mask_t get_input_mask(input_context _input_context, controller_index _controller_index, mkr::keycode _keycode) {
            uint64_t context_mask = (uint64_t) _input_context;
            context_mask <<= 48;
            context_mask &= 0xFFFF000000000000;

            uint64_t controller_mask = (uint64_t) _controller_index;
            controller_mask <<= 32;
            controller_mask &= 0x0000FFFF00000000;

            uint64_t keycode_mask = (uint64_t) _keycode;
            keycode_mask &= 0x00000000FFFFFFFF;

            return context_mask | controller_mask | keycode_mask;
        }

        /**
            \brief Extract the input context from an input mask.
            \param _input_mask The input_mask to extract from.
            \return The input_context of the input_mask.
            \sa get_input_mask, get_controller_index, get_keycode, compare_context, compare_controller_index, compare_keycode, compare_mask
        */
        static input_context get_context(input_mask_t _input_mask) {
            _input_mask >>= 48;
            _input_mask &= 0x000000000000FFFF;
            return static_cast<input_context>(_input_mask);
        }

        /**
            \brief Extract the controller index from an input mask.
            \param _input_mask The input mask to extract from.
            \return The controller index of the input mask.
            \sa get_input_mask, get_context, get_keycode, compare_context, compare_controller_index, compare_keycode, compare_mask
        */
        static controller_index get_controller_index(input_mask_t _input_mask) {
            _input_mask >>= 32;
            _input_mask &= 0x000000000000FFFF;
            return static_cast<controller_index>(_input_mask);
        }

        /**
            \brief Extract the keycode from an input mask.
            \param _input_mask The input mask to extract from.
            \return The keycode of the input mask.
            \sa get_input_mask, get_context, get_controller_index, compare_context, compare_controller_index, compare_keycode, compare_mask
        */
        static mkr::keycode get_keycode(input_mask_t _input_mask) {
            _input_mask &= 0x00000000FFFFFFFF;
            return static_cast<mkr::keycode>(_input_mask);
        }

        /**
            \brief Compare 2 input contexts.
            \param _context_a The first input context to compare.
            \param _context_b The second input context to compare.
            \return Returns (_context_a & _context_b) != 0.
            \sa get_input_mask, get_context, get_controller_index, get_keycode, compare_controller_index, compare_keycode, compare_mask
        */
        static bool compare_context(input_context _context_a, input_context _context_b) {
            return _context_a & _context_b;
        }

        /**
            \brief Compare 2 controller index.
            \param _controller_index_a The first controller index to compare.
            \param _controller_index_b The second controller index to compare.
            \return Returns (_controller_index_a & _controller_index_b) != 0.
            \sa get_input_mask, get_context, get_controller_index, get_keycode, compare_context, compare_keycode, compare_mask
        */
        static bool compare_controller_index(controller_index _controller_index_a, controller_index _controller_index_b) {
            return _controller_index_a & _controller_index_b;
        }

        /**
            \brief Compare 2 keycodes.
            \param _keycode_a The first keycode to compare.
            \param _keycode_b The second keycode to compare.
            \return Returns _keycode_a == _keycode_b.
            \sa get_input_mask, get_context, get_controller_index, get_keycode, compare_context, compare_controller_index, compare_mask
        */
        static bool compare_keycode(mkr::keycode _keycode_a, mkr::keycode _keycode_b) {
            return _keycode_a == _keycode_b;
        }

        /**
            \brief Compare 2 input_mask.
            \param _mask_a The first input mask to compare.
            \param _mask_b The second input mask to compare.
            \return Returns true result of comparing the masks' controller index, keycode and input context.
            \sa get_input_mask, get_context, get_controller_index, get_keycode, compare_context, compare_controller_index, compare_keycode
        */
        static bool compare_mask(input_mask_t _mask_a, input_mask_t _mask_b) {
            input_context context_a = get_context(_mask_a);
            input_context context_b = get_context(_mask_b);

            controller_index controller_index_a = get_controller_index(_mask_a);
            controller_index controller_index_b = get_controller_index(_mask_b);

            mkr::keycode keycode_a = get_keycode(_mask_a);
            mkr::keycode keycode_b = get_keycode(_mask_b);

            return compare_context(context_a, context_b) &&
                   compare_controller_index(controller_index_a, controller_index_b) &&
                   compare_keycode(keycode_a, keycode_b);
        }
    };
} // mkr