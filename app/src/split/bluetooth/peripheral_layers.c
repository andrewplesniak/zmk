
#include <zephyr/types.h>
#include <zephyr/sys/util.h>

#include <zmk/split/bluetooth/peripheral_layers.h>
#include <zmk/keymap.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static uint32_t peripheral_layers = 0;

void set_peripheral_layers_state(uint32_t new_layers) {
    LOG_DBG("Peripheral: set_peripheral_layers_state: 0x%08x", new_layers);
    peripheral_layers = new_layers;
}

bool peripheral_layer_active(uint8_t layer) {
    return (peripheral_layers & (BIT(layer))) == (BIT(layer));
};

uint8_t peripheral_highest_layer_active(void) {
    LOG_DBG("Peripheral: peripheral_highest_layer_active called. peripheral_layers: 0x%08x", peripheral_layers);
    if (peripheral_layers > 0) {
        for (uint8_t layer = ZMK_KEYMAP_LAYERS_LEN - 1; layer > 0; layer--) {
            // Layer 0 is the base layer, it's always "active" in a sense, but highest_layer_active
            // should return a layer that is explicitly set in peripheral_layers.
            // If only layer 0 is active (e.g. peripheral_layers is BIT(0) or 0 after clearing higher layers),
            // this loop might not behave as expected if it relies on layer > 0.
            // The original logic `|| layer == 0` inside the loop for `layer > 0` is a bit confusing.
            // Let's stick to the diff's logic for now.
            if ((peripheral_layers & (BIT(layer))) == (BIT(layer))) {
                LOG_DBG("Peripheral: peripheral_highest_layer_active returning: %d", layer);
                return layer;
            }
        }
        // If loop finishes, it means only layer 0 might be active or no layers explicitly.
        // Check if layer 0 is the only one.
        if ((peripheral_layers & BIT(0)) == BIT(0)) {
             LOG_DBG("Peripheral: peripheral_highest_layer_active returning layer 0 as it's set.", 0);
             return 0;
        }
    }
    // If peripheral_layers is 0 or only layer 0 was set and not found by loop.
    LOG_DBG("Peripheral: peripheral_highest_layer_active returning default: 0 (peripheral_layers: 0x%08x)", peripheral_layers);
    return 0;
}
