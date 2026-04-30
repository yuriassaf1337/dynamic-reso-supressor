cmake_minimum_required(VERSION 3.15)
project(DynResoSuppressor VERSION 0.3)

add_subdirectory(JUCE)

juce_add_plugin(DynResoSuppressor
    PLUGIN_MANUFACTURER_CODE Dyer
    PLUGIN_CODE DyEQ
    FORMATS VST3
    PRODUCT_NAME "dyn-reso-suppressor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_add_binary_data(PluginData SOURCES
    assets/fonts/Hack-Regular.ttf
)

juce_generate_juce_header(DynResoSuppressor)

target_sources(DynResoSuppressor PRIVATE
    Source/DynamicEqualizer.cpp
    Source/PluginEditor.cpp
    Source/PluginProcessor.cpp
    Source/SpectrumComponent.cpp
    Source/SpectrumPlotObject.cpp
)

target_compile_options(DynResoSuppressor PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:SHELL:-include cstring>"
    "$<$<COMPILE_LANGUAGE:C>:SHELL:-include string.h>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fpermissive>"
    -Wno-error
)

target_compile_definitions(DynResoSuppressor PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_USE_ACCESSIBILITY=0
    JUCE_USE_JPEGLIB=0
    JUCE_USE_WINDOWS_MEDIA_FORMAT=0
    JUCE_DIRECT2D=0
)

target_link_libraries(DynResoSuppressor
    PRIVATE
        PluginData
        $<$<PLATFORM_ID:Windows>:comctl32;imm32;dcomp;dwmapi;d2d1;d3d11;dxgi;dwrite;wininet;winmm;ws2_32;version;shlwapi;dbghelp;ole32;oleaut32;uuid;kernel32;user32;advapi32;shell32;sapi>
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
