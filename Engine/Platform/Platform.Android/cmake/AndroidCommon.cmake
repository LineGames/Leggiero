cmake_minimum_required(VERSION 3.18.1)


# Basic Functions

function(le_tgt_setCppOptions targetName)

    set_property(TARGET ${targetName} PROPERTY CXX_STANDARD 20)

    target_compile_options(${targetName} PRIVATE -fexceptions)
    target_compile_options(${targetName} PRIVATE -frtti)

endfunction()
