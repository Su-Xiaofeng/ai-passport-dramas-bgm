idf_component_register(
    SRCS "main.c" "queen_bgm.c" "bgm_model.c" "bgm_adpcm.c" "bgm_ui.c" "bgm_tracks_data.c"
         "../assets/fonts/bgm_font_12.c" "../assets/fonts/bgm_font_16.c" "../assets/fonts/bgm_font_20.c"
    INCLUDE_DIRS "."
    REQUIRES bsp esp_timer
)

# PCM must stay off the audio worker stack.
set_source_files_properties("queen_bgm.c" PROPERTIES COMPILE_OPTIONS "-Werror=frame-larger-than=512")
