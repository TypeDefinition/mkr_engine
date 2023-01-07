#pragma once

#include "input/keycode.h"

/**
    \file input.h
    \brief
        When making games, for portability purposes, as well as our own sanity, we do not want to hardcode physical buttons to specific actions in our games.
        We should use a layer of abstraction. This is where our input system comes in.\n\n

        input_name (32bit unsigned integer) -
            An input name is the name we give to our actions. Examples can be JUMP, WALK or SHOOT.

        input_mask (16bit input_context + 16bit controller_index + 32bit keycode) -
            A 64bit mask made by combining an input_context, a controller_index and a keycode.
            To register a control, we will register with an input handler a input_name and a input_mask.
            This means that whenever the conditions of the input_mask is met (correct context, controller and physical button), an event representing input_name will be dispatched.

        input_context (16bit unsigned integer) -
            The controls of a game might differ depending on what is happening.
            Contexts is the name given to the scenario the game or player is currently in. Examples can be WALKING, DRIVING, COMBAT, MAIN_MENU, PAUSE_MENU.
            For example during the context COMBAT, left-click may mean PUNCH, but during the context DRIVING, left-click may mean HONK.
            This allows us to make the same physical controls do different things in different situations.
            We can represent these contexts using input_context. As input_context has 16bits, we can have up to 16 contexts.

        controller_index (16bit unsigned integer) -
            It is possible to have multiple controllers active at the same time, such as in a couch co-op game.
            Thus, controller_index allows us to specify which controller we want to trigger which actions.
            For example in a couch co-op game, we want inputs from controller_index_0 to control player 1, and controller_index_1 to control player 2.
            We can represent these controllers using controller_index. As controller_index has 16bits, we can have up to 16 controllers.

        keycode -
            This is a 32bit unsigned integer that represents the physical key being pressed. For the full list of available keycodes, check keycode.h.
*/

namespace mkr {
    /// \brief 64bit Input Mask
    typedef uint64_t input_mask;

    /// \brief 32bit Input Name
    typedef uint32_t input_name;

    /// \brief 16bit Input Context
    typedef uint16_t input_context;

    #define input_context_none 0x0000
    #define input_context_all 0xFFFF
    #define input_context_default input_context_all

    #define input_context_0 0x0001
    #define input_context_1 0x0002
    #define input_context_2 0x0004
    #define input_context_3 0x0008
    #define input_context_4 0x0010
    #define input_context_5 0x0020
    #define input_context_6 0x0040
    #define input_context_7 0x0080
    #define input_context_8 0x0100
    #define input_context_9 0x0200
    #define input_context_10 0x0400
    #define input_context_11 0x0800
    #define input_context_12 0x1000
    #define input_context_13 0x2000
    #define input_context_14 0x4000
    #define input_context_15 0x8000

    /// \brief 16bit Controller Index
    typedef uint16_t controller_index;

    #define controller_index_none 0x0000
    #define controller_index_all 0xFFFF
    #define controller_index_default controller_index_all

    #define controller_index_0 0x0001
    #define controller_index_1 0x0002
    #define controller_index_2 0x0004
    #define controller_index_3 0x0008
    #define controller_index_4 0x0010
    #define controller_index_5 0x0020
    #define controller_index_6 0x0040
    #define controller_index_7 0x0080
    #define controller_index_8 0x0100
    #define controller_index_9 0x0200
    #define controller_index_10 0x0400
    #define controller_index_11 0x0800
    #define controller_index_12 0x1000
    #define controller_index_13 0x2000
    #define controller_index_14 0x4000
    #define controller_index_15 0x8000

    /**
        \brief Input Utility Class
    */
    class input {
    public:
        input() = delete;

        /**
            \brief Generate an input mask given a input context, controller index and keycode.
            \param _input_context The input context.
            \param _controller_index The controller index.
            \param _keycode The keycode.
            \return An input_mask representing the given input_context, controller_index and keycode.
        */
        static input_mask get_input_mask(input_context _input_context, controller_index _controller_index, mkr::keycode _keycode) {
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
        static input_context get_context(input_mask _input_mask) {
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
        static controller_index get_controller_index(input_mask _input_mask) {
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
        static mkr::keycode get_keycode(input_mask _input_mask) {
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
            return (_context_a & _context_b) != 0;
        }

        /**
            \brief Compare 2 controller index.
            \param _controller_index_a The first controller index to compare.
            \param _controller_index_b The second controller index to compare.
            \return Returns (_controller_index_a & _controller_index_b) != 0.
            \sa get_input_mask, get_context, get_controller_index, get_keycode, compare_context, compare_keycode, compare_mask
        */
        static bool compare_controller_index(controller_index _controller_index_a, controller_index _controller_index_b) {
            return (_controller_index_a & _controller_index_b) != 0;
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
        static bool compare_mask(input_mask _mask_a, input_mask _mask_b) {
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
}