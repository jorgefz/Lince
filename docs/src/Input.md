# Input

## LinceIsKeyPressed
```c
unsigned int LinceIsKeyPressed(int key)
```

## LinceIsMouseButtonPressed
```c
unsigned int LinceIsMouseButtonPressed(int button)
```

## LinceGetMousePos
```c
void LinceGetMousePos(float* xpos, float* ypos)
```

## LinceGetMouseX
```c
float LinceGetMouseX()
```

## LinceGetMouseY
```c
float LinceGetMouseY()
```

## LinceKey

Keycodes

| Enum | Value | Key |
| ---- | ----- | --- |
| LinceKey_Unknown		| -1  |	  	|
| LinceKey_Space		| 32  |	  	|
| LinceKey_Apostrophe	| 39  |	' 	|
| LinceKey_Comma		| 44  |	, 	|
| LinceKey_Minus		| 45  |	- 	|
| LinceKey_Period		| 46  |	. 	|
| LinceKey_Slash		| 47  |	/ 	|
| LinceKey_0			| 48  |	0 	|
| LinceKey_1			| 49  |	1 	|
| LinceKey_2			| 50  |	2 	|
| LinceKey_3			| 51  |	3 	|
| LinceKey_4			| 52  |	4 	|
| LinceKey_5			| 53  |	5 	|
| LinceKey_6			| 54  |	6 	|
| LinceKey_7			| 55  |	7 	|
| LinceKey_8			| 56  |	8 	|
| LinceKey_9			| 57  |	9 	|
| LinceKey_Semicolon	| 59  |	; 	|
| LinceKey_Equal		| 61  |	= 	|
| LinceKey_a			| 65  |	a 	|
| LinceKey_b			| 66  |	b 	|
| LinceKey_c			| 67  |	c 	|
| LinceKey_d			| 68  |	d 	|
| LinceKey_e 			| 69  |	e 	|
| LinceKey_f 			| 70  |	f 	|
| LinceKey_g 			| 71  |	g 	|
| LinceKey_h 			| 72  |	h 	|
| LinceKey_i 			| 73  |	i 	|
| LinceKey_j 			| 74  |	j 	|
| LinceKey_k 			| 75  |	k 	|
| LinceKey_l 			| 76  |	l 	|
| LinceKey_m 			| 77  |	m 	|
| LinceKey_n 			| 78  |	n 	|
| LinceKey_o 			| 79  |	o 	|
| LinceKey_p 			| 80  |	p 	|
| LinceKey_q 			| 81  |	q 	|
| LinceKey_r 			| 82  |	r 	|
| LinceKey_s 			| 83  |	s 	|
| LinceKey_t 			| 84  |	t 	|
| LinceKey_u 			| 85  |	u 	|
| LinceKey_v 			| 86  |	v 	|
| LinceKey_w 			| 87  |	w 	|
| LinceKey_x 			| 88  |	x 	|
| LinceKey_y 			| 89  |	y 	|
| LinceKey_z 			| 90  |	z 	|
| LinceKey_LeftBracket 	| 91  |	[ 	|
| LinceKey_Backslash	| 92  |	\ 	|
| LinceKey_RightBracket	| 93  |	] 	|
| LinceKey_GraveAccent 	| 96  |	` 	|
| LinceKey_World1 		| 161 | 	|
| LinceKey_World2 		| 162 | 	|
| LinceKey_Escape 		| 256 | 	|
| LinceKey_Enter 		| 257 | 	|
| LinceKey_Tab 			| 258 | 	|
| LinceKey_Backspace 	| 259 | 	|
| LinceKey_Insert 		| 260 | 	|
| LinceKey_Delete 		| 261 | 	|
| LinceKey_Right 		| 262 | 	|
| LinceKey_Left 		| 263 | 	|
| LinceKey_Down 		| 264 | 	|
| LinceKey_Up 			| 265 | 	|
| LinceKey_PageUp 		| 266 | 	|
| LinceKey_PageDown 	| 267 | 	|
| LinceKey_Home 		| 268 | 	|
| LinceKey_End 			| 269 | 	|
| LinceKey_CapsLock 	| 280 | 	|
| LinceKey_ScrollLock 	| 281 | 	|
| LinceKey_NumLock 		| 282 | 	|
| LinceKey_PrintScreen	| 283 | 	|
| LinceKey_Pause 		| 284 | 	|
| LinceKey_F1 			| 290 | 	|
| LinceKey_F2 			| 291 | 	|
| LinceKey_F3 			| 292 | 	|
| LinceKey_F4 			| 293 | 	|
| LinceKey_F5 			| 294 | 	|
| LinceKey_F6 			| 295 | 	|
| LinceKey_F7 			| 296 | 	|
| LinceKey_F8 			| 297 | 	|
| LinceKey_F9 			| 298 | 	|
| LinceKey_F10 			| 299 | 	|
| LinceKey_F11 			| 300 | 	|
| LinceKey_F12 			| 301 | 	|
| LinceKey_F13 			| 302 | 	|
| LinceKey_F14 			| 303 | 	|
| LinceKey_F15 			| 304 | 	|
| LinceKey_F16 			| 305 | 	|
| LinceKey_F17 			| 306 | 	|
| LinceKey_F18 			| 307 | 	|
| LinceKey_F19 			| 308 | 	|
| LinceKey_F20 			| 309 | 	|
| LinceKey_F21 			| 310 | 	|
| LinceKey_F22 			| 311 | 	|
| LinceKey_F23 			| 312 | 	|
| LinceKey_F24 			| 313 | 	|
| LinceKey_F25 			| 314 | 	|
| LinceKey_Kp0 			| 320 | 	|
| LinceKey_Kp1 			| 321 | 	|
| LinceKey_Kp2 			| 322 | 	|
| LinceKey_Kp3 			| 323 | 	|
| LinceKey_Kp4 			| 324 | 	|
| LinceKey_Kp5 			| 325 | 	|
| LinceKey_Kp6 			| 326 | 	|
| LinceKey_Kp7 			| 327 | 	|
| LinceKey_Kp8 			| 328 | 	|
| LinceKey_Kp9 			| 329 | 	|
| LinceKey_KpDecimal 	| 330 | 	|
| LinceKey_KpDivide 	| 331 | 	|
| LinceKey_KpMultiply 	| 332 | 	|
| LinceKey_KpSubtract 	| 333 |  	|
| LinceKey_KpAdd 		| 334 | 	|
| LinceKey_KpEnter 	   	| 335 | 	|
| LinceKey_KpEqual 		| 336 | 	|
| LinceKey_LeftShift 	| 340 | 	|
| LinceKey_LeftControl	| 341 | 	|
| LinceKey_LeftAlt 		| 342 | 	|
| LinceKey_LeftSuper 	| 343 | 	|
| LinceKey_RightShift 	| 344 | 	|
| LinceKey_RightControl | 345 | 	|
| LinceKey_RightAlt 	| 346 | 	|
| LinceKey_RightSuper 	| 347 | 	|
| LinceKey_Menu 		| 348 | 	|
| LinceKey_Last 		| LinceKey_Menu | |

## LinceMouseButton

Mouse button codes

| Enum 						| Value 			 |
| ------------------------- | ------------------ |
| LinceMouseButton_1 		| 0 				 |
| LinceMouseButton_2 		| 1 				 |
| LinceMouseButton_3 		| 2 				 |
| LinceMouseButton_4 		| 3 				 |
| LinceMouseButton_5 		| 4 				 |
| LinceMouseButton_6 		| 5 				 |
| LinceMouseButton_7 		| 6 				 |
| LinceMouseButton_8 		| 7 				 |
| LinceMouseButton_Last		| LinceMouseButton_8 |
| LinceMouseButton_Left		| LinceMouseButton_1 |
| LinceMouseButton_Right	| LinceMouseButton_2 |
| LinceMouseButton_Middle	| LinceMouseButton_3 |