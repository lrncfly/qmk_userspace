# # Append your implementation source file to the build queue
QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += st7789_spi
QUANTUM_PAINTER_LVGL_INTEGRATION = yes
ST7789 = true

ST7789_ENABLE = yes
LVGL_ENABLE = yes
# Ensure the hardware master driver leaves the compilation stack
SPI_MASTER_ENABLE = no

# SRC += theme.c
# SRC += ui_elements.c

# # LVGL fonts
# SRC += fonts/montserratbold14.c
# SRC += fonts/montserratbold13.c
# SRC += fonts/dmsans13.c
# SRC += fonts/dmsans14.c
# SRC += fonts/dmsans20medium.c
# SRC += fonts/dmsansbold40.c
# SRC += fonts/dmsans14bold.c
# SRC += fonts/jetbrainsmono13.c
# SRC += fonts/jetbrainsmono14bold.c
# SRC += fonts/jetbrainsmonoextrabold60.c

# # Screens
# SRC += screens/screen_base.c
# SRC += screens/screen_pomodoro.c
# SRC += screens/menu.c
# SRC += screens/utilities.c
