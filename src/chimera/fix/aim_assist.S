/* SPDX-License-Identifier: GPL-3.0-only */
.intel_syntax noprefix

.text
.globl _on_aim_assist
.extern _using_analog_movement
.extern _not_using_analog_movement_jmp
.extern _yes_using_analog_movement_jmp
.extern _auto_aim_width_addr   /* dirección expuesta desde C++ */

_on_aim_assist:
    mov eax, [_using_analog_movement]
    cmp byte ptr [eax], 1
    jnz force_mouse_magnetism

    /* Joystick → aim assist normal */
    jmp dword ptr [_yes_using_analog_movement_jmp]

force_mouse_magnetism:
    /* Mouse/teclado → forzar magnetismo alto en disparos */
    mov ebx, [_auto_aim_width_addr]
    mov dword ptr [ebx], 0x3E000000   /* 0.12f */

    /* Pero saltar a la rutina de "no analog movement"
       para que no se altere el movimiento del mouse */
    jmp dword ptr [_not_using_analog_movement_jmp]
