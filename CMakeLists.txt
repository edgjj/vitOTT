cmake_minimum_required (VERSION 3.15.0)

project(vitOTT VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(modules/JUCE)
    
set(VOTT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(VOTT_SRC_DIR ${VOTT_ROOT_DIR}/src)

set(VITOTT_FORMATS VST3 AU)

# add LV2 for Linux builds
if (UNIX AND NOT APPLE)
    list (APPEND VITOTT_FORMATS LV2)
endif()

juce_add_plugin(vitOTT
    PRODUCT_NAME "vitOTT"
    COMPANY_NAME tstd                      		
    IS_SYNTH FALSE                              
    NEEDS_MIDI_INPUT FALSE                      
    NEEDS_MIDI_OUTPUT FALSE                     
    IS_MIDI_EFFECT FALSE                        
    COPY_PLUGIN_AFTER_BUILD TRUE                
    PLUGIN_MANUFACTURER_CODE TstD               
    PLUGIN_CODE VOTT                            
    FORMATS ${VITOTT_FORMATS}
    LV2URI https://github.com/edgjj/vitOTT
    
    VST3_CATEGORIES Fx Dynamics
    VST2_CATEGORY kPlugCategEffect
    AAX_CATEGORY AAX_EPlugInCategory_Effect AAX_ePlugInCategory_Dynamics
    
)            

juce_generate_juce_header(vitOTT)

set(VOTT_SRC 
    ${VOTT_SRC_DIR}/PluginProcessor.cpp
    ${VOTT_SRC_DIR}/PluginProcessor.h
    ${VOTT_SRC_DIR}/PluginEditor.cpp
    ${VOTT_SRC_DIR}/PluginEditor.h
)

file(GLOB_RECURSE VOTT_VITAL_DSP_SRC "${VOTT_SRC_DIR}/vital_dsp/*")
file(GLOB_RECURSE VOTT_VITAL_UI_SRC "${VOTT_SRC_DIR}/vital_ui/*")
file(GLOB_RECURSE VOTT_VITAL_BRIDGE_SRC "${VOTT_SRC_DIR}/bridge/*")

file(GLOB VOTT_VITAL_FONTS_TTF "${VOTT_ROOT_DIR}/resources/*.ttf")
file(GLOB VOTT_VITAL_FONTS_OTF "${VOTT_ROOT_DIR}/resources/*.otf")
set(VOTT_VITAL_RESOURCES 
    ${VOTT_VITAL_FONTS_TTF}
    ${VOTT_VITAL_FONTS_OTF}
    "${VOTT_ROOT_DIR}/resources/default.vitalskin"
)

juce_add_binary_data(vitOTT_binarydata SOURCES
    ${VOTT_VITAL_RESOURCES}
)

source_group("Vital DSP Sources" FILES ${VOTT_VITAL_DSP_SRC})
source_group("Vital UI Sources" FILES ${VOTT_VITAL_UI_SRC})
source_group("vitOTT Sources" FILES ${VOTT_SRC})
source_group("vitOTT Bridge Sources" FILES ${VOTT_VITAL_BRIDGE_SRC})

target_sources(vitOTT PRIVATE 
    ${VOTT_SRC}
    ${VOTT_VITAL_BRIDGE_SRC}
    ${VOTT_VITAL_DSP_SRC}
    ${VOTT_VITAL_UI_SRC}
)

target_compile_definitions(vitOTT
    PUBLIC 
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_JACK=1
    JUCE_ALSA=1
)

target_include_directories(vitOTT
    PRIVATE
    ${VOTT_SRC_DIR}/vital_dsp
    ${VOTT_SRC_DIR}/vital_ui
    ${VOTT_SRC_DIR}/vital_ui/look_and_feel
    ${VOTT_SRC_DIR}/vital_dsp/framework
    ${VOTT_SRC_DIR}/vital_dsp/utilities
    ${VOTT_ROOT_DIR}/3rd-party/nlohmann_json
    ${VOTT_SRC_DIR}/bridge
)

target_link_libraries(vitOTT
    PRIVATE
        vitOTT_binarydata
        juce::juce_dsp
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
