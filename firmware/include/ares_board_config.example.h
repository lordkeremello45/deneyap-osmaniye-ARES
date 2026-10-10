#pragma once

// Copy to firmware/include/ares_board_config.h (the real file is git-ignored).
// Use RAW MCU GPIO numbers only after confirming the exact DENEYAP Kart V2
// revision and official Arduino-core mapping. Do not copy D-label numbers here.
//
// #define ARES_LINK_RX_PIN <verified_raw_gpio_connected_to_other_board_TX>
// #define ARES_LINK_TX_PIN <verified_raw_gpio_connected_to_other_board_RX>
//
// Optional sensor GPIO definitions must also be verified against the physical
// breakout wiring before enabling ARES_ENABLE_* build flags.
