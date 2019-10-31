; This is a not-portable way to generate Wine library
; for i386 that contains only one forwarded export,
; dinput8.DirectInput8Create -> dinput8hook.DirectInput8Create.
; For a portable way, build an empty library with Winelib and dinput8.spec.
;
; Wine core expects 64K+4K uninitialized buffer before PE headers
; to put the PE module here (Win32 requires 64K alignment;
; for such a buffer, one could always find 4K-page with 64K-alignment
; regardless of where the system has put the module).
; The linker cannot handle uninitialized data outside of .bss,
; so the linker generates an output file with 64K+4K zeroes inside.
;
; So we generate the output ELF by hand.
; (Of course, we could just reserve enough data in .bss,
; let the linker do it's job and then shuffle bytes at run-time,
; but come on, let's have fun!)
include 'elf.inc'
format ELF dynamic at 0
entry 0 ; it is ignored for shared libraries anyway

; Unwind stuff. Not really needed, but let's get fancy
segment readable gnuehframe
ehframe_base:
	db	1	; version
	db	DW_EH_PE_pcrel+DW_EH_PE_sdata2	; eh_frame_ptr encoding
	db	DW_EH_PE_udata2	; fde_count encoding
	db	DW_EH_PE_datarel+DW_EH_PE_sdata4	; table encoding (glibc prefers this value)
fde_count = 3
	dw	eh_frame - $
	dw	fde_count
	dd	_init - ehframe_base, fde1 - ehframe_base
	dd	get_eip - ehframe_base, fde2 - ehframe_base
	dd	__wine_dll_register_plt - ehframe_base, fde3 - ehframe_base
segment readable executable
eh_frame:
	dd	fde1 - $ - 4	; length
	dd	0	; token of CIE
	db	1	; version
	db	'zR',0	; augmentation string
	db	1	; code alignment factor, ULEB128-encoded
	db	(-4) and 0x7F	; data alignment factor, SLEB128-encoded
	db	reg_retaddr	; return address column
	db	1	; augmentation length, 1 byte for 'R' augmentation
	db	DW_EH_PE_pcrel+DW_EH_PE_sdata2	; encoding of EIP ranges
	db	DW_CFA_def_cfa, reg_esp, 4	; caller's frame is in esp+4 by default
	db	DW_CFA_offset+reg_retaddr, 1	; return address is in [caller's frame+1*(-4)] by default
	align_zero 4
fde1:
	dd	fde2 - $ - 4	; length
	dd	$ - eh_frame	; CIE pointer
	dw	_init - $	; EIP range start
	dw	_init.end - _init	; EIP range size
	db	0		; augmentation length
	declare_cfi_storage _init
	align_zero 4
fde2:
	dd	fde3 - $ - 4	; length
	dd	$ - eh_frame	; CIE pointer
	dw	get_eip - $	; EIP range start
	dw	get_eip.end - get_eip	; EIP range size
	db	0		; augmentation length
; default is OK, no custom DWARF code needed
	align_zero 4
fde3:
	dd	eh_frame_end - $ - 4	; length
	dd	$ - eh_frame	; CIE pointer
	dw	__wine_dll_register_plt - $	; EIP range start
	dw	__wine_dll_register_plt.end - __wine_dll_register_plt	; EIP range size
	db	0		; augmentation length
	declare_cfi_storage __wine_dll_register_plt
	align_zero 4
eh_frame_end:

; ELF dynamic linker stuff
symhash:
; no exported symbols
	dd	1	; nbuckets
	dd	1	; symbias
	dd	1	; Bloom filter size
	dd	0	; Bloom filter hash shift
	dd	0	; empty Bloom filter
	dd	0	; no data in bucket 0
symtab:
	dd	0, 0, 0, 0	; zero symbol
__wine_dll_register_symtab = ($ - symtab) / 0x10
	dd	__wine_dll_register_string - strtab, 0, 0, STB_GLOBAL shl 4
strtab:
	db	0	; zero string
__wine_dll_register_string:
	db	'__wine_dll_register',0
strtab_end:

; The actual code
_init:
	cfi_startproc _init
	call	get_eip
@@:
	add	eax, dllname - @b
	push	eax
	cfi_def_cfa_offset 8
	sub	eax, dllname - ntheader
	push	eax
	cfi_def_cfa_offset 12
	add	dword [eax + reloc1 - ntheader], eax
	add	dword [eax + reloc2 - ntheader], eax
	call	dword [eax+gotplt+12-ntheader]
	add	esp, 8
	cfi_def_cfa_offset 4
	ret
	cfi_endproc _init
.end:

get_eip:
	mov	eax, [esp]
	ret
.end:

; ELF dynamic linker stuff, lazy-load stub, assumes eax == ntheader
__wine_dll_register_plt:
	cfi_startproc __wine_dll_register_plt
	push	0
	cfi_def_cfa_offset 8
	push	dword [eax+gotplt+4-ntheader]
	cfi_def_cfa_offset 12
	jmp	dword [eax+gotplt+8-ntheader]
	cfi_endproc __wine_dll_register_plt
.end:

align_zero 4

; Wine core requirements
segment readable writable
pe_headers	rb	65536 + 4096

; ELF dynamic linker stuff
segment readable writable dynamic
dynamic:
	dd	DT_INIT, rva _init
	dd	DT_GNU_HASH, rva symhash
	dd	DT_STRTAB, rva strtab
	dd	DT_SYMTAB, rva symtab
	dd	DT_STRSZ, strtab_end - strtab
	dd	DT_SYMENT, 16 ; size of symtab entry
	dd	DT_PLTGOT, rva gotplt
	dd	DT_PLTRELSZ, jmprels_end - jmprels
	dd	DT_PLTREL, DT_REL ; PLT relocation type: REL
	dd	DT_JMPREL, rva jmprels
;	dd	DT_REL, rva rels
;	dd	DT_RELSZ, rels_end - rels
;	dd	DT_RELENT, 8
	dd	DT_NULL, 0
; patching those relocs by hand turns out to be a bit more efficient
;rels:
;	dd	rva reloc1, R_386_RELATIVE
;	dd	rva reloc2, R_386_RELATIVE
;rels_end:
jmprels:
	dd	rva gotplt + 12, (__wine_dll_register_symtab shl 8) + R_386_JMP_SLOT
jmprels_end:
gotplt:
	dd	rva dynamic
	dd	0	; lazy-loader will patch this to dll handle
	dd	0	; lazy-loader will patch this to dl_runtime_resolve
	dd	rva __wine_dll_register_plt
			; lazy-loader will add dll address to this,
			; non-lazy-loader will patch this to the real __wine_dll_register

segment readable writable gnustack ; no executable stack

segment readable writable
; PE headers for Wine
ntheader:
; taken from wine/tools/winebuild/spec32.c
	dd	0x4550	; Signature
	dw	0x14C	; Machine
	dw	0	; NumberOfSections, ignored anyway
	dd	0	; TimeDateStamp
	dd	0	; PointerToSymbolTable
	dd	0	; NumberOfSymbols
	dw	0xE0	; SizeOfOptionalHeader
	dw	0x2102	; Characteristics: IMAGE_FILE_DLL|IMAGE_FILE_32BIT_MACHINE|IMAGE_FILE_EXECUTABLE_IMAGE
	dw	0x10B	; Magic
	db	7	; MajorLinkerVersion
	db	10	; MinorLinkerVersion
	dd	0	; SizeOfCode
	dd	0	; SizeOfInitializedData
	dd	0	; SizeOfUninitializedData
	dd	0	; AddressOfEntryPoint
	dd	0	; BaseOfCode
	dd	0	; BaseOfData
reloc1:
	dd	pe_headers - ntheader	; ImageBase, will be patched
	dd	0x1000	; SectionAlignment
	dd	0x1000	; FileAlignment
	dw	1, 0	; Major/MinorOperatingSystemVersion
	dw	0, 0	; Major/MinorImageVersion
	dw	4, 0	; Major/MinorSubsystemVersion
	dd	0	; Win32VersionValue
	dd	_end - ntheader ; SizeOfImage
	dd	0x1000	; SizeOfHeaders
	dd	0	; Checksum
	dw	0	; Subsystem = IMAGE_SUBSYSTEM_UNKNOWN
	dw	0x100	; DllCharacteristics = IMAGE_DLLCHARACTERISTICS_NX_COMPAT
	dd	0x100000, 0x1000	; SizeOfStackReserve/Commit
	dd	0x100000, 0x1000	; SizeOfHeapReserve/Commit
	dd	0	; LoaderFlags
	dd	16	; NumberOfRvaAndSizes
	dd	exports - ntheader, exports_end - exports	; IMAGE_DIRECTORY_ENTRY_EXPORT
	times 28 dd 0

; export table for Wine
exports:
	dd	0, 0, 0, dllname - ntheader, 1
	dd	1	; NumberOfFunctions
	dd	1	; NumberOfNames
	dd	export_functions - ntheader
	dd	export_names - ntheader
	dd	export_ordinals - ntheader
export_functions:
reloc2:
	dd	DirectInput8Create - ntheader	; absolute address, will be patched
export_names:
	dd	DirectInput8Create_name - ntheader
export_ordinals:
	dw	0
dllname	db	'dinput8.dll',0
; forward DirectInput8Create -> dinput8hook.DirectInput8Create
DirectInput8Create:
	db	'dinput8hook.'
DirectInput8Create_name:
	db	'DirectInput8Create',0
exports_end:
_end:
