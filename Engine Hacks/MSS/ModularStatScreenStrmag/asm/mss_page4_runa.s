.thumb
.include "mss_defs.s"

.equ GaidenStatScreen, PersonalInfoTable+4

page_start

draw_textID_at 13, 3, textID=0xd4c, width=16, colour=Blue

@ first like
mov    r0,r8
ldr    r1,[r0]               @load character pointer
ldrb   r1,[r1,#0x4]	         @load character number
adr    r0,PersonalInfoTable  @load first like
ldr    r0,[r0]
mov    r2,#12
mul    r1,r2
@add    r1,#2
ldrh   r0,[r0,r1]		@load textid
mov    r3, r7
mov r1, #12
ldrh r2,[r3] @current number
add r2,r1 @for the next one.
strb r1, [r3, #4] @store width
strb r2, [r3, #8] @assign the next one.
blh BufferText
mov    r2, #0x0
str    r2, [sp]
str    r0, [sp, #4]
mov    r2, #0 @colour
mov    r0, r7
ldr    r1, =(tile_origin+(0x20*2*3)+(2*17))
mov    r3, #0
blh    DrawText, r4
add    r7, #8

@ second like
mov    r0,r8
ldr    r1,[r0]               @load character pointer
ldrb   r1,[r1,#0x4]	         @load character number
adr    r0,PersonalInfoTable  @load first like
ldr    r0,[r0]
mov    r2,#12
mul    r1,r2
add    r1,#2
ldrh   r0,[r0,r1]		@load textid
mov    r3, r7
mov r1, #12
ldrh r2,[r3] @current number
add r2,r1 @for the next one.
strb r1, [r3, #4] @store width
strb r2, [r3, #8] @assign the next one.
blh BufferText
mov    r2, #0x0
str    r2, [sp]
str    r0, [sp, #4]
mov    r2, #0 @colour
mov    r0, r7
ldr    r1, =(tile_origin+(0x20*2*5)+(2*17))
mov    r3, #0
blh    DrawText, r4
add    r7, #8

draw_textID_at 13, 7, textID=0xd4d, width=16, colour=Blue

@ first dislike
mov    r0,r8
ldr    r1,[r0]               @load character pointer
ldrb   r1,[r1,#0x4]	         @load character number
adr    r0,PersonalInfoTable  @load first like
ldr    r0,[r0]
mov    r2,#12
mul    r1,r2
add    r1,#4
ldrh   r0,[r0,r1]		@load textid
mov    r3, r7
mov r1, #12
ldrh r2,[r3] @current number
add r2,r1 @for the next one.
strb r1, [r3, #4] @store width
strb r2, [r3, #8] @assign the next one.
blh BufferText
mov    r2, #0x0
str    r2, [sp]
str    r0, [sp, #4]
mov    r2, #0 @colour
mov    r0, r7
ldr    r1, =(tile_origin+(0x20*2*7)+(2*17))
mov    r3, #0
blh    DrawText, r4
add    r7, #8

@ second dislike
mov    r0,r8
ldr    r1,[r0]               @load character pointer
ldrb   r1,[r1,#0x4]	         @load character number
adr    r0,PersonalInfoTable  @load first like
ldr    r0,[r0]
mov    r2,#12
mul    r1,r2
add    r1,#6
ldrh   r0,[r0,r1]		@load textid
mov    r3, r7
mov r1, #12
ldrh r2,[r3] @current number
add r2,r1 @for the next one.
strb r1, [r3, #4] @store width
strb r2, [r3, #8] @assign the next one.
blh BufferText
mov    r2, #0x0
str    r2, [sp]
str    r0, [sp, #4]
mov    r2, #0 @colour
mov    r0, r7
ldr    r1, =(tile_origin+(0x20*2*9)+(2*17))
mov    r3, #0
blh    DrawText, r4
add    r7, #8

draw_textID_at 13, 11, textID=0xd4f, width=16, colour=Blue

@ age
mov    r0,r8
ldr    r1,[r0]               @load character pointer
ldrb   r1,[r1,#0x4]	         @load character number
adr    r0,PersonalInfoTable  @load first like
ldr    r0,[r0]
mov    r2,#12
mul    r1,r2
add    r1,#8
ldrh   r0,[r0,r1]		@load textid
mov    r3, r7
mov r1, #5
ldrh r2,[r3] @current number
add r2,r1 @for the next one.
strb r1, [r3, #4] @store width
strb r2, [r3, #8] @assign the next one.
blh BufferText
mov    r2, #0x0
str    r2, [sp]
str    r0, [sp, #4]
mov    r2, #0 @colour
mov    r0, r7
ldr    r1, =(tile_origin+(0x20*2*11)+(2*16))
mov    r3, #0
blh    DrawText, r4
add    r7, #8

draw_textID_at 21, 11, textID=0xd4e, width=16, colour=Blue

@ height
mov    r0,r8
ldr    r1,[r0]               @load character pointer
ldrb   r1,[r1,#0x4]	         @load character number
adr    r0,PersonalInfoTable  @load first like
ldr    r0,[r0]
mov    r2,#12
mul    r1,r2
add    r1,#10
ldrh   r0,[r0,r1]		@load textid
mov    r3, r7
mov r1, #5
ldrh r2,[r3] @current number
add r2,r1 @for the next one.
strb r1, [r3, #4] @store width
strb r2, [r3, #8] @assign the next one.
blh BufferText
mov    r2, #0x0
str    r2, [sp]
str    r0, [sp, #4]
mov    r2, #0 @colour
mov    r0, r7
ldr    r1, =(tile_origin+(0x20*2*11)+(2*25))
mov    r3, #0
blh    DrawText, r4
add    r7, #8

@ Next let's draw Gaiden spells if the hack is installed.
ldr r0, GaidenStatScreen
cmp r0, #0x00
beq SkipGaidenDraw
	@ Gaiden magic is installed. Call the function for stat screen drawing.
	mov lr, r0
	mov r0, #13 @ X coordinate.
	mov r1, #13 @ Y coordinate.
	mov r2, r7  @ Current TextHandle.
	.short 0xF800
	mov r0, r7 @ Next "blank" TextHandle.
SkipGaidenDraw:

page_end

.align
.ltorg
PersonalInfoTable:
