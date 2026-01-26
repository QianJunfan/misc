main:
        # Add Immediate: addi rd, rs1, imd
        #       rd:     destination register
        #       rs1:    source register 1
        #       imd:    a numbaer from -2048 to 2047

        # rd  = rs1 + imd
        addi sp, sp, -16
        
        # in modern mainstream architectures (x86, arm, mips...),
        # the stack is typically placed at high addresses and grows downward,
        # while the heap is usually placed at low addresses and grows upward.
        
        sw ra, 12(sp)
        
        # In a standard RISC-V function prologue,
        # ra is often the first register saved after the stack pointer is adjusted.
        # Since the stack grows downward, the first item saved occupies the "highest" address
        # within that specific stack frame.

        # ra: return address

        # Store Word: sw rs2, offset(rs1)
        # target_addr = rs1 + offset
        # mem[target_addr] = rs2

        sw s0, 8(sp)
        # s0 typically serves as the Frame Pointer.
        # By convention, if you intend to modify s0,
        # you must first back up its original val so that it can be
        # restored to the caller upon exiting the func.
        
        addi s0, sp, 16

        li a5, 0
        mv a0, a5

        # RISC-V ABI convention: the a0 register is specifically
        # used to hold the func's return val.

        lw ra, 12(sp)
        # Load Word: lw rd, offset(rs1)
        # tar_addr = rs1 + offset 
        # rd = mem[tar_addr] 

        lw s0, 8(sp)
        addi sp, sp, 16
        jr ra


# save -> exec -> restore

