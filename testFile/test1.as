L1	add	1,1,5
L2	slt	2,1,3
	j	L3
	sw	4,1,12
L3	lw	2,6,L1
	beq	2,1,end
	addi	7,1,L3
	.fill	45
	.space	L2
	lui	8,3
	jalr	11,13
end	halt