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

    // Aplica magnetismo fuerte cuando se usa mouse/teclado
    static void apply_mouse_magnetism() {
        if (!auto_aim_width_addr || IsBadWritePtr(auto_aim_width_addr, sizeof(float))) return;

        if (!*using_analog_movement) {
            *auto_aim_width_addr = 1.50f; // magnetismo fuerte para mouse
        } else {
            *auto_aim_width_addr = 1.50f; // valor normal para control
        }
    }

    void set_up_aim_assist_fix() noexcept {
        auto *should_use_aim_assist_addr = get_chimera().get_signature("should_use_aim_assist_sig").data();
        using_analog_movement = *reinterpret_cast<std::uint8_t **>(should_use_aim_assist_addr + 2);
        static const SigByte nop[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
        write_code_s(should_use_aim_assist_addr, nop);

        auto *aim_assist = get_chimera().get_signature("aim_assist_sig").data();
        not_using_analog_movement_jmp = aim_assist + 0x2 + 0x6 + 0x36E;
        yes_using_analog_movement_jmp = aim_assist + 0x2 + 0x6;
        static Hook hook;
        const void *old_fn;
        write_function_override(aim_assist, hook, reinterpret_cast<const void *>(on_aim_assist), &old_fn);

        // Localiza el float de magnetismo
        auto &sig = get_chimera().get_signature("auto_aim_width_sig");
        auto_aim_width_addr = reinterpret_cast<float*>(sig.data());

        if (!auto_aim_width_addr) {
            console_error("auto_aim_width_sig no encontró dirección válida");
            return;
        }

        console_output("auto_aim_width_addr = %p", auto_aim_width_addr);

        // Aplica magnetismo fuerte en mouse cada frame
        add_preframe_event(apply_mouse_magnetism);

        // Feedback en consola
        console_output("Magnetismo mouse activo: auto_aim_width parcheado");
    }
}
