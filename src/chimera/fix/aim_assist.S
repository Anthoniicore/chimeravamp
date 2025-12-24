# SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.text

# Declaración de símbolos externos provistos por C++
.extern _using_analog_movement
.extern _not_using_analog_movement_jmp
.extern _yes_using_analog_movement_jmp

.globl _on_aim_assist
_on_aim_assist:
    # Forzar siempre el bloque de "sí usar aim assist"
    jmp dword ptr [_yes_using_analog_movement_jmp]
