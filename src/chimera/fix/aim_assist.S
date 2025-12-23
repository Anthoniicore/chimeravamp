/* SPDX-License-Identifier: GPL-3.0-only */
.intel_syntax noprefix

.text
.globl _on_aim_assist
.extern _using_analog_movement
.extern _not_using_analog_movement_jmp
.extern _yes_using_analog_movement_jmp

_on_aim_assist:
    /* Read analog movement flag pointer */
    mov eax, [_using_analog_movement]
    /* Compare byte at that address with 1 */
    cmp byte ptr [eax], 1

    /* Regardless of input, jump to "yes using analog movement" path
       to force aim assist enabled for mouse and joystick */
    jmp dword ptr [_yes_using_analog_movement_jmp]
