func:
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	s0, 8(sp)
	addi	s0, sp, 16

	li	a5, 10

	mv	a0, a5
	lw	ra, 12(sp)
	lw	s0, 8(sp)
	addi	sp, sp, 16

main:
	addi	sp, sp, -32
	sw	ra, 28(sp)
	sw	r0, 24(sp)
	addi 	s0, sp, 32

	call 	func
	sw	a0, -20(s0)

	lw	a5, -20(s0)
	mv	a0, a5
	lw	ra, 28(sp)
	lw	r0, 24(sp)
	addi	sp, sp, 32
	jr	ra
