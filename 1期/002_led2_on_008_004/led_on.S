

/*
 * 点亮LED2: gpf5
 */

.text
.global _start

_start:

/* 配置GPF5为输出引脚
 * 把0x400写到地址0x56000050
 */
	ldr r1, =0x56000050
	ldr r0, =0x400	/* mov r0, #0x400 */
	str r0, [r1]


/* 设置GPF5输出高电平 
 * 把0写到地址0x56000054
 */
	ldr r1, =0x56000054
	ldr r0, =0	/* mov r0, #0 */
	str r0, [r1]

	/* 死循环 */
halt:
	b halt

