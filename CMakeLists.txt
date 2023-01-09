cmake_minimum_required (VERSION 3.15.0)

project(vitOTT VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
	
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
)

file(GLOB_RECURSE VOTT_VITAL_SRC "${VOTT_SRC_DIR}/vital_dsp/*")

source_group("Vital Sources" FILES ${VOTT_VITAL_SRC})
source_group("vitOTT Sources" FILES ${VOTT_SRC})

target_sources(vitOTT PRIVATE 
	${VOTT_SRC}
    ${VOTT_VITAL_SRC}
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

target_link_libraries(vitOTT
    PRIVATE
		juce::juce_dsp
		juce::juce_audio_formats
		juce::juce_audio_plugin_client
		juce::juce_gui_extra
		juce::juce_gui_basics
		juce::juce_graphics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
