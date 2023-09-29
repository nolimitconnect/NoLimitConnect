#include "AssemblyConstants/aarch64/arm_defines.S"

function get_assembly_reverse_num, export=1
    mov w0, #4321
    ret
endfunc
