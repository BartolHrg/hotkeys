#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
#SingleInstance, Force



; Unesite·duljinu·polja:·3↵
; Unesite·duljinu·polja:·3↵
; Unesite·duljinu·polja:·3

^+v::
	text := Clipboard
	text := StrReplace(text, "·", " ")
	text := StrReplace(text, "`r`n", "\n")
	text := StrReplace(text, "↵\n" , "\n")
	text := StrReplace(text, "↵",    "\n")
	; Loop, Parse, text, 
	; {
	; 	MsgBox, % Ord(A_LoopField)
	; }
	SendInput, %text%
Return
