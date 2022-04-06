SECTION .text

GLOBAL usb_legacy_ps2_controller_write_control_byte_aa
GLOBAL usb_legacy_ps2_controller_read_response_byte
GLOBAL usb_legacy_ps2_controller_enable_first_port
GLOBAL usb_legacy_ps2_controller_write_control_byte_ab

; https://wiki.osdev.org/%228042%22_PS/2_Controller#PS.2F2_Controller_IO_Ports
; 0x60    Read/Write    Data Port
; 0x64    Read          Status Register
; 0x64    Write         Command Register

usb_legacy_ps2_controller_write_control_byte_aa:
    mov al, 0xAA
    out 0x64, al

    ret

usb_legacy_ps2_controller_enable_first_port:
    mov al, 0xAE
    out 0x64, al

    ret

usb_legacy_ps2_controller_write_control_byte_ab:
    mov al, 0xAB
    out 0x64, al

    ret

; Response bytes are read from Data port 0x60. Before reading response bytes,
; bit 0 of status register needs to be set indicating that response byte has
; arrived.
usb_legacy_ps2_controller_read_response_byte:
.wait_for_response_byte:
    in al, 0x64
    and al, 0x01
    cmp al, 0x01
    jne .wait_for_response_byte

    in al, 0x60

    ret
