; SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.text
.globl _on_aim_assist
_on_aim_assist:
    ; Forzar siempre el bloque de "sí usar aim assist"
    jmp dword ptr [_yes_using_analog_movement_jmp]
