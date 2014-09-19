; crc8 algorithm
; crc8 generator = x^8 + x^5 + x^4 + x^0

#include "p18f2680.inc"

			UDATA
shift res 2


			CODE

crc8

init_code
	
	
streamloop
	MOVLW 0xFE			; -2 -> W
	MOVF PLUSW1, W		; &stream (FSR2-2) -> W
	MOVWF FSR0L			; Address of stream[0]
	
	MOVFF BSR, FSR0H	; High bits of 

;init_shiftloop
;	MOVLW 0x08
;	MOVWF I
;	
;shiftloop
;	CLRC
;	RLCF SHIFTL
;	RLCF SHIFTH
;	
;	BNC 0x02
;	
;	MOVF GEN, W
;	XORWF SHIFTH
;	
;	DECFSZ I
;	GOTO shiftloop
;	
;	INCF STREAM
;	DECFSZ LEN
;	GOTO streamloop
;	
;return_code
	
	RETURN
	
			GLOBAL crc8
			END
	