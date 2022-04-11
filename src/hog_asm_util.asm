%macro get_register_64_value 1
GLOBAL get_register_64_value_%1
get_register_64_value_%1:
    mov rax, %1
    ret
%endmacro

get_register_64_value cr3
