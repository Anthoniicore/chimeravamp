// SPDX-License-Identifier: GPL-3.0-only

#include "aim_assist.hpp"
#include "../chimera.hpp"
#include "../signature/hook.hpp"
#include "../signature/signature.hpp"
#include "../event/frame.hpp"
#include "../output/output.hpp"

extern "C" {
    std::uint8_t *using_analog_movement = nullptr;
    std::byte *not_using_analog_movement_jmp = nullptr;
    std::byte *yes_using_analog_movement_jmp = nullptr;

    void on_aim_assist();
}

namespace Chimera {
    static float *auto_aim_width_addr = nullptr;
    static float *auto_aim_range_addr = nullptr;

    static inline float *resolve_abs_mov_target(const Signature &sig) {
        // sig.data() -> C7 05 [addr] [imm]
        auto instr = sig.data();
        if (!instr) return nullptr;
        auto addr32 = *reinterpret_cast<const std::uint32_t *>(instr + 2);
        return reinterpret_cast<float *>(addr32);
    }

    // Ajusta magnetismo y desactiva auto-aim en mouse
    static void apply_mouse_magnetism() {
        if (!auto_aim_width_addr || !auto_aim_range_addr) return;

        // Opcional: más robustez (si ya usas WinAPI, descomenta):
        // if (IsBadWritePtr(auto_aim_width_addr, sizeof(float)) || IsBadWritePtr(auto_aim_range_addr, sizeof(float))) return;

        if (!*using_analog_movement) {
            *auto_aim_width_addr = 1.50f; // magnetismo fuerte para mouse
            *auto_aim_range_addr = 2.0f;  // sin corrección de mira en mouse
        } else {
            *auto_aim_width_addr = 1.50f; // valor normal en control
            *auto_aim_range_addr = 2.0f;  // auto-aim normal en control
        }
    }

    void set_up_aim_assist_fix() noexcept {
        // NOP a should_use_aim_assist para que lea el flag externo
        auto *should_use_aim_assist_addr = get_chimera().get_signature("should_use_aim_assist_sig").data();
        using_analog_movement = *reinterpret_cast<std::uint8_t **>(should_use_aim_assist_addr + 2);
        static const SigByte nop[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
        write_code_s(should_use_aim_assist_addr, nop);

        // Hook principal de aim assist
        auto *aim_assist = get_chimera().get_signature("aim_assist_sig").data();
        not_using_analog_movement_jmp = aim_assist + 0x2 + 0x6 + 0x36E;
        yes_using_analog_movement_jmp = aim_assist + 0x2 + 0x6;
        static Hook hook;
        const void *old_fn;
        write_function_override(aim_assist, hook, reinterpret_cast<const void *>(on_aim_assist), &old_fn);

        // Resolver direcciones ABSOLUTAS de los floats desde las firmas (no escribir en la instrucción)
        auto &sig_width = get_chimera().get_signature("auto_aim_width_sig");
        auto &sig_range = get_chimera().get_signature("auto_aim_range_sig");
        auto_aim_width_addr = resolve_abs_mov_target(sig_width);
        auto_aim_range_addr = resolve_abs_mov_target(sig_range);

        if (!auto_aim_width_addr || !auto_aim_range_addr) {
            console_error("No se resolvieron auto_aim_width o auto_aim_range (punteros nulos)");
            return;
        }

        console_output("auto_aim_width_addr = %p", auto_aim_width_addr);
        console_output("auto_aim_range_addr = %p", auto_aim_range_addr);

        add_preframe_event(apply_mouse_magnetism);

        console_output("Magnetismo mouse activo: width y range aplicados cada frame");
    }
}

