TITLE Project 6B     (Project06.asm)

; Author: Gabrielle Josephson
; Last Modified: 12/09/2019
; Course / Project ID : CS 271 400 F19          
; Project Number: 6B							Date Due: 12/08/2019
;
; Description:  This program generates random combinations problems with the n value
; between 3 and 12 and the r value up to the n value. The user is then asked to solve the problem.
; The program then solves the problem using factorials, which are implemented with recursion.
; The program displays the answer and informs the user if his or her answer was correct.
; Then, the user has the option to perform more problems or quit.

INCLUDE Irvine32.inc

LO  = 3
HI  = 12
MAXSIZE = 100

mWriteStr	MACRO message
	push	edx
	mov		edx, message
	call	WriteString
	pop		edx
ENDM

.data
proTitle		BYTE	"Welcome to the Combinations Calculator", 0
author			BYTE	"Implemented by Gabrielle Josephson", 0
instruct_1		BYTE	"I'll give you a combinations problem.", 0
instruct_2		BYTE	"You enter your answer, and I'll let you know if you're right.", 0
prompt			BYTE	"How many ways can you choose? ", 0
answer			BYTE	33 DUP(?)
guess			DWORD	?
n				DWORD	?
r				DWORD	?
n_factorial		DWORD	1
r_factorial		DWORD	1
n_r_factorial	DWORD	1
result			DWORD	?
problem			BYTE	"Problem:", 0
n_element		BYTE	"Number of elements in the set: ", 0
r_element		BYTE	"Number of elements to choose from the set: ", 0
invalid			BYTE	"Invalid input. Please try again.", 0
answer1			BYTE	"There are ", 0
answer2			BYTE	" combinations of ", 0
answer3			BYTE	" items from a set of ", 0
incorrect		BYTE	"You need more practice.", 0
correct			BYTE	"You are correct!", 0
another			BYTE	"Another problem? (y/n): ", 0
yesno			BYTE	MAXSIZE	DUP(?)
bye				BYTE	"OK ... goodbye.", 0
	
.code
main PROC
	call		Randomize

	push		OFFSET proTitle
	push		OFFSET author
	push		OFFSET instruct_1
	push		OFFSET instruct_2
	call		introduction

AnotherProblem:
	mov			guess, 0				; Reset guess, n_factorial, r_factorial, and n_r_factorial if user chooses to perform multiple problems
	mov			n_factorial, 1
	mov			r_factorial, 1
	mov			n_r_factorial, 1

	push		OFFSET r_element
	push		OFFSET n_element
	push		OFFSET problem
	push		OFFSET n
	push		OFFSET r
	call		showProblem

	push		OFFSET guess
	push		OFFSET invalid
	push		OFFSET prompt
	push		OFFSET answer
	call		getData
	
	push		OFFSET n_factorial
	push		OFFSET r_factorial
	push		OFFSET n_r_factorial
	push		n
	push		r
	push		OFFSET result
	call		combinations

	push		OFFSET correct
	push		OFFSET incorrect
	push		guess
	push		OFFSET answer3
	push		OFFSET answer2
	push		OFFSET answer1
	push		n
	push		r
	push		result
	call		showResults

More:
	mWriteStr	OFFSET	another
	mov			edx, OFFSET yesno
	mov			ecx, MAXSIZE
	call		ReadString				; Get user's answer about whether or not s/he wants to do another problem
	call		CrLf
	mov			ecx, eax
	cmp			ecx, 1					; If answer is longer than one character, it is invalid
	jne			TryAgain
	mov			esi, OFFSET yesno
	cld
	lodsb
	cmp			al, 121					; Compare answer to y and n
	je			AnotherProblem
	cmp			al, 110
	je			TheEnd
TryAgain:
	mWriteStr	OFFSET invalid
	call		CrLf
	jmp			More
TheEnd:
	mWriteStr	OFFSET bye

	
	exit	; exit to operating system
main ENDP

; ******************************************************************
; This procedure displays a welcome message and outlines what the
; program does.
; Receives: Addresses of proTitle, author, instruct_1, and instruct_2
;			one the system stack.
; Preconditions: None
; Postconditions: None
; Registers changed: edx
; ******************************************************************
introduction	PROC
	push		ebp
	mov			ebp, esp
	PUSHAD
	mWriteStr	[ebp+20]
	call		CrLf
	mWriteStr	[ebp+16]
	call		CrLf
	mWriteStr	[ebp+12]
	call		CrLf
	mWriteStr	[ebp+8]
	call		CrLf
	POPAD
	pop			ebp
	ret			16
introduction	ENDP

; ******************************************************************
; This procedure calculates random integers n (3-12) and r (1-n) and
;		displays a combinations problem.
; Receives: Addresses of n_element, r_element, n, r, and problem on
;			the system stack. Global constants HI and LO
; Preconditions: None
; Postconditions: Values of n and r elements are in n and r respectively.
; Registers changed: eax, ebx, ecx, edx
; ******************************************************************

showProblem		PROC
	push		ebp
	mov			ebp, esp
	PUSHAD
	mWriteStr	[ebp+16]
	call		CrLf
	mov			ebx, [ebp+12]
	mov			ecx, [ebp+8]
	mWriteStr	[ebp+20]
	mov			eax, HI				; Calculate random integer between 3 and 12 for n
	sub			eax, LO
	inc			eax
	call		RandomRange
	add			eax, LO				
	mov			[ebx], eax			; Move value into n
	call		WriteDec
	call		CrLf
	call		RandomRange			; Calculate random integer between 1 and n for r
	inc			eax
	mov			[ecx], eax			; Move value into r
	mWriteStr	[ebp+24]
	call		WriteDec
	call		CrLf
	POPAD
	pop			ebp
	ret			20
showProblem		ENDP

; ******************************************************************
; This procedure prompts user to enter their solution for the
;			combinations problem defined in showProblem. Input is validated to
;			be numerical.
; Receives: Addresses of guess, invalid, prompt, and answer on the
;			system stack
; Preconditions: guess must be initialized as zero.
; Postconditions: user input stored as a string in answer and as an
;				  integer in guess.
; Registers changed: eax, ebx, ecx, edx, esi
; ******************************************************************

getData		PROC
	push	ebp
	mov		ebp, esp
	PUSHAD
Input:
	mWriteStr	[ebp+12]
	mov			edx, [ebp+8]
	mov			ecx, 32
	call		ReadString			; Get user input and store it in a string
	
	mov			ecx, eax
	mov			esi, [ebp+8]
	cld
Counter:
	lodsb							; Make sure that each value is an integer
	cmp			al, 48
	jl			notNum
	cmp			al, 57
	jg			notNum
	sub			al, 48
	cmp			ecx, 1
	je			OnesPlace
	push		ecx
	dec			ecx
	mov			ebx, 10
PlaceValue:						; Calculate the place value of number
	mul			ebx
	loop		PlaceValue
	pop			ecx
OnesPlace:
	mov			ebx, [ebp+20]
	add			[ebx], eax		; Add number to guess to convert string into integer
	mov			eax, 0
	loop		Counter
	jmp			EndData
notNum:
	mWriteStr	[ebp+16]		; Output error message if input is non-numerical
	call		CrLf
	mov			eax,0
	jmp			Input
EndData:
	POPAD
	pop			ebp
	ret			16
getData		ENDP

; ******************************************************************
; This procedure uses the values of n and r to calculate the combinations
;			problem. It uses the factorials of n, r, and (n-r)
; Receives: Address of n_factorial, r_factorial, n_r_factorial and
;			result. Values of n and r. All on system stack.
; Preconditions: n and r must be initialized. 3 <= n <= 12 and 1 <= r <= n
; Postconditions: Result of combinations problem is stored in result.
;				  The factorial values of n, r, and (n-r) are in
;				  n_factorial, r_factorial, and n_r_factorial, respectively.
; Registers changed: eax, ebx, ecx, edx
; ******************************************************************

combinations	PROC
	push		ebp
	mov			ebp, esp
	PUSHAD
	mov			eax, [ebp+16]
	sub			eax, [ebp+12]			; Calculate (n-r)
	cmp			eax, 0
	je			OneCombination			; If n and r are the same values, answer is 1
	push		[ebp+20]
	push		eax
	call		factorial
	mov			ebx, [ebp+20]
	mov			ecx, [ebx]				; move (n-r)! into ecx
	push		[ebp+24]
	push		[ebp+12]
	call		factorial
	mov			ebx, [ebp+24]
	mov			eax, [ebx]				; move r! into eax
	mul			ecx						; calculate r! * (n-r)! and store in eax
	mov			ecx, eax
	push		[ebp+28]
	push		[ebp+16]
	call		factorial
	mov			ebx, [ebp+28]
	mov			eax, [ebx]				; Store n! in eax
	mov			edx, 0
	div			ecx						; Calculate (n!)/[r! *(n-r)!] and store in eax
	mov			ebx, [ebp+8]
	mov			[ebx], eax
	jmp			EndCombinations
OneCombination:
	mov			ebx, [ebp+8]
	mov			eax, 1
	mov			[ebx], eax
EndCombinations:
	POPAD
	pop			ebp
	ret			24

combinations	ENDP

; ******************************************************************
; This procedure takes a value and an address, calculates the factorial
;			of the value, and stores it in the address.
; Receives: Address of n_factorial, r_factorial, or n_r_factorial and
;			value of n, r, or (n-r).
; Preconditions: n and r must be initialized
; Postconditions: Factorials are stored in n_factorial, r_factorial, or
;				  n_r_factorial
; Registers changed: eax, ebx, ecx, edx
; ******************************************************************

factorial		PROC
	push		ebp
	mov			ebp, esp
	PUSHAD
	mov			eax, [ebp+8]		; Move value to be factorialized into eax
	cmp			eax, 1				; If value is equal 1, recursion can be released
	jle			EndRecurse
	dec			eax		
	push		[ebp+12]
	push		eax
	call		factorial
	mov			edx, [ebp+12]
	mov			eax, [edx]			; Move previous product into eax
	mov			ebx, [ebp+8]
	mul			ebx
	mov			ecx, [ebp+12]
	mov			[ecx], eax			; Move next product into n_factorial, r_factorial, or n_r_factorial
	EndRecurse:
	POPAD
	pop			ebp
	ret			8
factorial		ENDP

; ******************************************************************
; This procedure displays the correct answer for the combinations
;			problem. It then compares the answer to the user's
;			answer and displays a message about whether or not
;			the user was correct.
; Receives: Values of result, n, r, and guess. Addresses of answer1,
;			answer2, answer3, correct, and incorrect. All on system
;			stack.
; Preconditions: result, n, r, and guess must all be initialized
; Postconditions: None
; Registers changed: eax, edx
; ******************************************************************


showResults		PROC
	push		ebp
	mov			ebp, esp
	PUSHAD
	call		CrLf
	mWriteStr	[ebp+20]			; Display combinations problem and answer
	mov			eax, [ebp+8]
	call		WriteDec
	mWriteStr	[ebp+24]
	mov			eax, [ebp+12]
	call		WriteDec
	mWriteStr	[ebp+28]
	mov			eax, [ebp+16]
	call		WriteDec
	call		CrLf
	mov			eax, [ebp+8]
	cmp			eax, [ebp+32]
	jne			Wrong				; Compare result and guess to determine if user got the problem right
	mWriteStr	[ebp+40]
	call		CrLf
	jmp			EndResults
Wrong:
	mWriteStr	[ebp+36]
	call		CrLf
EndResults:
	POPAD
	pop			ebp
	ret			24
showResults		ENDP

END main