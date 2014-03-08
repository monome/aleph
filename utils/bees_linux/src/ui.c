//Function declarations given in apps/bees/src/ui.h.
//gcc doesn't seem to like function declarations without definitions
//so here is some stuff that does nothing, as probably is happening with avr controller

void ui_init(void) {
    return;
}
void preset_init(void) {
    return;
}
u8 ui_loop(void) {
    return 1;
}
void preset_deinit(void) {
    return;
}
void ui_deinit(void) {
    return;
}

