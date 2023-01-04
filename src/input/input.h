#pragma once

/**
    \file input.h
    \brief
        When making games, for portability purposes, as well as our own sanity, we do not want to hardcode physical buttons to specific actions in our games.
        We should use a layer of abstraction. This is where our input system comes in.\n\n

        input_name -
            An input name is the name we give to our actions. Examples can be JUMP, WALK or SHOOT. It is an unsigned integer with a value ranging from 0 to (MAX_INPUT_NAME-1).
            That means that we an have a maxiumum of MAX_INPUT_NAME input names.

        input_context -
            The controls of a game might differ depending on what is happening.
            Contexts is the name given to the scenario the game or player is currently in. Examples can be WALKING, DRIVING, COMBAT, MAIN_MENU, PAUSE_MENU.
            For example during the context COMBAT, Left-Click may mean PUNCH, but during the context DRIVING, Left-Click may mean HONK.
            This allows us to make the same physical controls do different things when different 
            We can represent these contexts using input_context. As input_context has 16bits, we can have up to 16 contexts.

        controller_index -
            It is possible to have multiple controllers active at the same time, such as in a couch co-op game.
            CBTControllerindex allows us to specify which controller we want to trigger which actions.
            For example in a couch co-op game, we want inputs from CONTROLLER_1 to control player 1, and CONTROLLER_2 to control player 2.
            We can represent these controllers using controller_index. As controller_index has 16bits, we can have up to 16 controllers.

        cbtKeycode -
            This is a 32bit unsigned integer that represents the physical key being pressed. For the full list of available keycodes, check CBTKeyode.h.

        cbtControllerMask (16bit ControllerIndex + 16bit InputContext + 32bit Keycode) -
            This is a 64bit mask made by combining a input_context, a controller_index and a cbtKeycode.
            To register a control, we will register with an InputHandler a input_name and a cbtControllerMask.
            This means that whenever the conditions of the cbtControllerMask is met (correct context, controller and physical button), an event representing input_name will be dispatched.
*/

#include "input/keycode.h"

namespace mkr {
    /// \brief Input name.
    typedef uint32_t input_name;

    /// \brief The maximum number of input names.
    #define MAX_INPUT_NAME 128

    /// \brief 64bit Input Mask.
    typedef uint64_t input_mask;

    /// \brief 16bit Controller Index.
    typedef uint16_t controller_index;

    #define CONTROLLER_NONE 0x0000
    #define CONTROLLER_ALL 0xFFFF

    #define CONTROLLER_1 0x0001
    #define CONTROLLER_2 0x0002
    #define CONTROLLER_3 0x0004
    #define CONTROLLER_4 0x0008
    #define CONTROLLER_5 0x0010
    #define CONTROLLER_6 0x0020
    #define CONTROLLER_7 0x0040
    #define CONTROLLER_8 0x0080
    #define CONTROLLER_9 0x0100
    #define CONTROLLER_10 0x0200
    #define CONTROLLER_11 0x0400
    #define CONTROLLER_12 0x0800
    #define CONTROLLER_13 0x1000
    #define CONTROLLER_14 0x2000
    #define CONTROLLER_15 0x4000
    #define CONTROLLER_16 0x8000

    #define CONTROLLER_DEFAULT CONTROLLER_ALL

    /// \brief Input Context
    typedef uint16_t input_context;

    #define INPUT_CONTEXT_NONE 0x0000
    #define INPUT_CONTEXT_ALL 0xFFFF

    #define INPUT_CONTEXT_1 0x0001
    #define INPUT_CONTEXT_2 0x0002
    #define INPUT_CONTEXT_3 0x0004
    #define INPUT_CONTEXT_4 0x0008
    #define INPUT_CONTEXT_5 0x0010
    #define INPUT_CONTEXT_6 0x0020
    #define INPUT_CONTEXT_7 0x0040
    #define INPUT_CONTEXT_8 0x0080

    #define INPUT_CONTEXT_9 0x0100
    #define INPUT_CONTEXT_10 0x0200
    #define INPUT_CONTEXT_11 0x0400
    #define INPUT_CONTEXT_12 0x0800
    #define INPUT_CONTEXT_13 0x1000
    #define INPUT_CONTEXT_14 0x2000
    #define INPUT_CONTEXT_15 0x4000
    #define INPUT_CONTEXT_16 0x8000

    #define INPUT_CONTEXT_DEFAULT INPUT_CONTEXT_ALL

    /**
        \brief Input Utility Class
    */
    class input {
    public:
        input() = delete;

        /**
            \brief Generate an input_mask given a input_context, controller_index and cbtKeycode.

            \param _context The input context.
            \param _controllerIndex The controller index.
            \param _keycode The keycode.

            \return An input_mask representing the given input_context, controller_index and cbtKeycode.

            \sa GetContext, GetControllerIndex, GetKeycode, CompareContext, CompareControllerIndex, CompareKeyCode, CompareMask
        */
        static input_mask
        GenerateInputMask(input_context _context, controller_index _controllerIndex, keycode _keyCode);

        /**
            \brief Extract the input_context from a input_mask.

            \param _inputMask The input_mask to extract from.

            \return The input_context of the input_mask.

            \sa GenerateInputMask, GetControllerIndex, GetKeycode, CompareContext, CompareControllerIndex, CompareKeyCode, CompareMask
        */
        static input_context GetContext(input_mask _inputMask);

        /**
            \brief Extract the controller_index from a input_mask.

            \param _inputMask The input_mask to extract from.

            \return The controller_index of the input_mask.

            \sa GenerateInputMask, GetContext, GetKeycode, CompareContext, CompareControllerIndex, CompareKeyCode, CompareMask
        */
        static controller_index GetControllerIndex(input_mask _inputMask);

        /**
            \brief Extract the cbtKeycode from a input_mask.

            \param _inputMask The input_mask to extract from.

            \return The cbtKeycode of the input_mask.

            \sa GenerateInputMask, GetContext, GetControllerIndex, CompareContext, CompareControllerIndex, CompareKeyCode, CompareMask
        */
        static keycode GetKeycode(input_mask _inputMask);

        /**
            \brief Compare 2 CBTInputContexts.

            \param _contextA The first input_context to compare.
            \param _contextB The second input_context to compare.

            \return Returns (_contextA & _contextB) != 0.

            \sa GenerateInputMask, GetContext, GetControllerIndex, GetKeycode, CompareControllerIndex, CompareKeyCode, CompareMask
        */
        static bool CompareContext(input_context _contextA, input_context _contextB);

        /**
            \brief Compare 2 controller_index.

            \param _controllerIndexA The first controller_index to compare.
            \param _controllerIndexB The second controller_index to compare.

            \return Returns (_controllerIndexA & _controllerIndexB) != 0.

            \sa GenerateInputMask, GetContext, GetControllerIndex, GetKeycode, CompareContext, CompareKeyCode, CompareMask
        */
        static bool CompareControllerIndex(controller_index _controllerIndexA, controller_index _controllerIndexB);

        /**
            \brief Compare 2 cbtKeycode.

            \param _keyCodeA The first cbtKeycode to compare.
            \param _keyCodeB The second cbtKeycode to compare.

            \return Returns _keyCodeA == _keyCodeB.

            \sa GenerateInputMask, GetContext, GetControllerIndex, GetKeycode, CompareContext, CompareControllerIndex, CompareMask
        */
        static bool CompareKeyCode(keycode _keyCodeA, keycode _keyCodeB);

        /**
            \brief Compare 2 input_mask.

            \param _maskA The first input_mask to compare.
            \param _maskB The second input_mask to compare.

            \return Returns true result of comparing the masks' controller index, keycode and input context.

            \sa GenerateInputMask, GetContext, GetControllerIndex, GetKeycode, CompareContext, CompareControllerIndex, CompareKeyCode
        */
        static bool CompareMask(input_mask _maskA, input_mask _maskB);
    };
}