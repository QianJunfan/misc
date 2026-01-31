.section .text
.globl _start

_start:
	jal func

	li a7, 93
	ecall

func:
	addi	sp, sp, -32
	sw	ra, 28(sp)
	sw	s0, 24(sp)
	addi	s0, sp, 32

	li	a5, 3
	sw	a5, -20(s0)

	lw	a4, -20(s0)
	li	a5, 1
	ble	a4, a5, .L2
	lw	a5, -20(s0)
	j	.L3

.L2:
	lw	a5, -20(s0)
	neg	a5, a5

.L3:
	mv	a0, a5
	lw	ra, 28(sp)
	lw	s0, 24(sp)
	addi	sp, sp, 32
	jr	ra
