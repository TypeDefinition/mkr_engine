#pragma once

#include <cstdint>

/**
    \file input.h
    \brief
        When making games, for portability purposes, as well as our own sanity, we do not want to hardcode physical buttons to specific actions in our games.
        We should use a layer of abstraction. This is where our input system comes in.\n

        input_action (32bit unsigned integer) -
            An input action is the name we give to our actions. Examples can be JUMP, WALK or SHOOT. This is defined by the user.

        input_mask (16bit input_context + 16bit controller_index + 32bit keycode) -
            A 64bit mask made by combining an input_context, a controller_index and a keycode.
            To register a control, we will register with an input handler a input_action and a input_mask.
            This means that whenever the conditions of the input_mask is met (correct context, controller and physical button), an event representing input_action will be dispatched.

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
            This is a 32bit unsigned integer that represents the physical key being pressed.
*/
namespace mkr {
    /// \brief 32bit Input Action
    typedef uint32_t input_action_t;

    /// \brief 64bit Input Mask
    typedef uint64_t input_mask_t;

    /// \brief 16bit Input Context
    enum input_context : uint16_t {
        input_context_none = 0x0000,
        input_context_all = 0xFFFF,
        input_context_default = input_context_all,

        input_context_0 = 0x0001,
        input_context_1 = 0x0002,
        input_context_2 = 0x0004,
        input_context_3 = 0x0008,
        input_context_4 = 0x0010,
        input_context_5 = 0x0020,
        input_context_6 = 0x0040,
        input_context_7 = 0x0080,
        input_context_8 = 0x0100,
        input_context_9 = 0x0200,
        input_context_10 = 0x0400,
        input_context_11 = 0x0800,
        input_context_12 = 0x1000,
        input_context_13 = 0x2000,
        input_context_14 = 0x4000,
        input_context_15 = 0x8000,
    };

    /// \brief 16bit Controller Index
    enum controller_index : uint16_t {
        controller_index_none = 0x0000,
        controller_index_all = 0xFFFF,
        controller_index_default = controller_index_all,

        controller_index_0 = 0x0001,
        controller_index_1 = 0x0002,
        controller_index_2 = 0x0004,
        controller_index_3 = 0x0008,
        controller_index_4 = 0x0010,
        controller_index_5 = 0x0020,
        controller_index_6 = 0x0040,
        controller_index_7 = 0x0080,
        controller_index_8 = 0x0100,
        controller_index_9 = 0x0200,
        controller_index_10 = 0x0400,
        controller_index_11 = 0x0800,
        controller_index_12 = 0x1000,
        controller_index_13 = 0x2000,
        controller_index_14 = 0x4000,
        controller_index_15 = 0x8000,
    };

    typedef uint32_t keycode_t;
} // mkr
