#ifndef LINCE_KEYCODES_H
#define LINCE_KEYCODES_H

/** @brief Codes for keyboard keys, identical to GLFW values.
*/
typedef enum LinceKey {
    LinceKey_Unknown		= -1,
    LinceKey_Space			= 32,
    LinceKey_Apostrophe     = 39,  /** ' */
    LinceKey_Comma			= 44,  /** , */
    LinceKey_Minus			= 45,  /** - */
    LinceKey_Period		    = 46,  /** . */
    LinceKey_Slash			= 47,  /** / */
    LinceKey_0				= 48,
    LinceKey_1				= 49,
    LinceKey_2				= 50,
    LinceKey_3				= 51,
    LinceKey_4				= 52,
    LinceKey_5				= 53,
    LinceKey_6				= 54,
    LinceKey_7				= 55,
    LinceKey_8				= 56,
    LinceKey_9				= 57,
    LinceKey_Semicolon		= 59,  /** ; */
    LinceKey_Equal			= 61,  /** = */
    LinceKey_a				= 65,
    LinceKey_b				= 66,
    LinceKey_c				= 67,
    LinceKey_d				= 68,
    LinceKey_e 			    = 69,
    LinceKey_f 			    = 70,
    LinceKey_g 			    = 71,
    LinceKey_h 			    = 72,
    LinceKey_i 			    = 73,
    LinceKey_j 			    = 74,
    LinceKey_k 			    = 75,
    LinceKey_l 			    = 76,
    LinceKey_m 			    = 77,
    LinceKey_n 			    = 78,
    LinceKey_o 			    = 79,
    LinceKey_p 			    = 80,
    LinceKey_q 			    = 81,
    LinceKey_r 			    = 82,
    LinceKey_s 			    = 83,
    LinceKey_t 			    = 84,
    LinceKey_u 			    = 85,
    LinceKey_v 			    = 86,
    LinceKey_w 			    = 87,
    LinceKey_x 			    = 88,
    LinceKey_y 			    = 89,
    LinceKey_z 			    = 90,
    LinceKey_LeftBracket 	= 91,  /** [ */ 
    LinceKey_Backslash 	    = 92,  /** \ */ 
    LinceKey_RightBracket   = 93,  /** ] */ 
    LinceKey_GraveAccent 	= 96,  /** ` */ 
    LinceKey_World1 		= 161, //> non-US #1 
    LinceKey_World2 		= 162, //> non-US #2 
    LinceKey_Escape 		= 256,
    LinceKey_Enter 		    = 257,
    LinceKey_Tab 			= 258,
    LinceKey_Backspace 	    = 259,
    LinceKey_Insert 		= 260,
    LinceKey_Delete 		= 261,
    LinceKey_Right 		    = 262,
    LinceKey_Left 			= 263,
    LinceKey_Down 			= 264,
    LinceKey_Up 			= 265,
    LinceKey_PageUp 		= 266,
    LinceKey_PageDown 	    = 267,
    LinceKey_Home 			= 268,
    LinceKey_End 			= 269,
    LinceKey_CapsLock 	    = 280,
    LinceKey_ScrollLock 	= 281,
    LinceKey_NumLock 		= 282,
    LinceKey_PrintScreen	= 283,
    LinceKey_Pause 		    = 284,
    LinceKey_F1 			= 290,  //> Function key
    LinceKey_F2 			= 291,  //> Function key
    LinceKey_F3 			= 292,  //> Function key
    LinceKey_F4 			= 293,  //> Function key
    LinceKey_F5 			= 294,  //> Function key
    LinceKey_F6 			= 295,  //> Function key
    LinceKey_F7 			= 296,  //> Function key
    LinceKey_F8 			= 297,  //> Function key
    LinceKey_F9 			= 298,  //> Function key
    LinceKey_F10 			= 299,  //> Function key
    LinceKey_F11 			= 300,  //> Function key
    LinceKey_F12 			= 301,  //> Function key
    LinceKey_F13 			= 302,  //> Function key
    LinceKey_F14 			= 303,  //> Function key
    LinceKey_F15 			= 304,  //> Function key
    LinceKey_F16 			= 305,  //> Function key
    LinceKey_F17 			= 306,  //> Function key
    LinceKey_F18 			= 307,  //> Function key
    LinceKey_F19 			= 308,  //> Function key
    LinceKey_F20 			= 309,  //> Function key
    LinceKey_F21 			= 310,  //> Function key
    LinceKey_F22 			= 311,  //> Function key
    LinceKey_F23 			= 312,  //> Function key
    LinceKey_F24 			= 313,  //> Function key
    LinceKey_F25 			= 314,  //> Function key
    LinceKey_Kp0 			= 320,  //> Keypad key
    LinceKey_Kp1 			= 321,  //> Keypad key
    LinceKey_Kp2 			= 322,  //> Keypad key
    LinceKey_Kp3 			= 323,  //> Keypad key
    LinceKey_Kp4 			= 324,  //> Keypad key
    LinceKey_Kp5 			= 325,  //> Keypad key
    LinceKey_Kp6 			= 326,  //> Keypad key
    LinceKey_Kp7 			= 327,  //> Keypad key
    LinceKey_Kp8 			= 328,  //> Keypad key
    LinceKey_Kp9 			= 329,  //> Keypad key
    LinceKey_KpDecimal 	    = 330,  //> Keypad key
    LinceKey_KpDivide 	    = 331,  //> Keypad key
    LinceKey_KpMultiply 	= 332,  //> Keypad key
    LinceKey_KpSubtract 	= 333,  //> Keypad key
    LinceKey_KpAdd 		    = 334,  //> Keypad key
    LinceKey_KpEnter 	   	= 335,  //> Keypad key
    LinceKey_KpEqual 		= 336,  //> Keypad key
    LinceKey_LeftShift 	    = 340,
    LinceKey_LeftControl	= 341,
    LinceKey_LeftAlt 		= 342,
    LinceKey_LeftSuper 	    = 343,
    LinceKey_RightShift 	= 344,
    LinceKey_RightControl   = 345,
    LinceKey_RightAlt 	    = 346,
    LinceKey_RightSuper 	= 347,
    LinceKey_Menu 			= 348,
    LinceKey_Last 			= LinceKey_Menu
} LinceKey;

#endif // LINCE_KEYCODES_H