# upro tockice

(the instructions for running are in root [README.md](../README.md) of this project)  

This is project specific to my university  
Where input and expected output is given  
with spaces replaced by `·`  
and newlines with `↵`  

It replaces · with space, and ↵ with "\n" (**not** newline, but literal "\n")

After running  
`Ctrl+C` for copy (nothing modified)  
`Ctrl+V` for raw paste (nothing modified)  
`Ctrl+Shift+V` for paste with replacements  

There is script for AHK v1 and AHK v2.  
I recommend v2, but both work fine.  

example:

when copying

```
; Unesite·duljinu·polja:·3↵
; Unesite·duljinu·polja:·3↵
; Unesite·duljinu·polja:·3↵
```
you get  
```
; Unesite duljinu polja: 3\n; Unesite duljinu polja: 3\n; Unesite duljinu polja: 3\n
```
