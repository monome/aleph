#include "screen_startup.h"

void screen_startup(void) {
    u32 i;

#include "startup_glyph.c"

    // fill screenbuffer with solid background
    for (i = 0; i < GRAM_BYTES; i++) { screenBuf[i] = 0; }

    spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
    gpio_set_gpio_pin(OLED_REGISTER_PIN);
    for (i = 0; i < GRAM_BYTES; i++) { spi_write(OLED_SPI, screenBuf[i]); }

    spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);

    /// draw the glyph
    screen_draw_region(128 - 24 - 1, 64 - 32 - 1, 24, 32,
                       (u8*)aleph_hebrew_glyph);
}
