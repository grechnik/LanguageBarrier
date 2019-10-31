; symtab binding types
STB_LOCAL = 0
STB_GLOBAL = 1
STB_WEAK = 2
STB_GNU_UNIQUE = 10

; dynamic entry types
DT_NULL		= 0		; Marks end of dynamic section
DT_NEEDED	= 1		; Name of needed library
DT_PLTRELSZ	= 2		; Size in bytes of PLT relocs
DT_PLTGOT	= 3		; Processor defined value
DT_HASH		= 4		; Address of symbol hash table
DT_STRTAB	= 5		; Address of string table
DT_SYMTAB	= 6		; Address of symbol table
DT_RELA		= 7		; Address of Rela relocs
DT_RELASZ	= 8		; Total size of Rela relocs
DT_RELAENT	= 9		; Size of one Rela reloc
DT_STRSZ	= 10		; Size of string table
DT_SYMENT	= 11		; Size of one symbol table entry
DT_INIT		= 12		; Address of init function
DT_FINI		= 13		; Address of termination function
DT_SONAME	= 14		; Name of shared object
DT_RPATH	= 15		; Library search path (deprecated)
DT_SYMBOLIC	= 16		; Start symbol search here
DT_REL		= 17		; Address of Rel relocs
DT_RELSZ	= 18		; Total size of Rel relocs
DT_RELENT	= 19		; Size of one Rel reloc
DT_PLTREL	= 20		; Type of reloc in PLT
DT_DEBUG	= 21		; For debugging; unspecified
DT_TEXTREL	= 22		; Reloc might modify .text
DT_JMPREL	= 23		; Address of PLT relocs
DT_BIND_NOW	= 24		; Process relocations of object
DT_INIT_ARRAY	= 25		; Array with addresses of init fct
DT_FINI_ARRAY	= 26		; Array with addresses of fini fct
DT_INIT_ARRAYSZ	= 27		; Size in bytes of DT_INIT_ARRAY
DT_FINI_ARRAYSZ	= 28		; Size in bytes of DT_FINI_ARRAY
DT_RUNPATH	= 29		; Library search path
DT_FLAGS	= 30		; Flags for the object being loaded
DT_PREINIT_ARRAY = 32		; Array with addresses of preinit fct
DT_PREINIT_ARRAYSZ = 33		; size in bytes of DT_PREINIT_ARRAY

DT_GNU_PRELINKED = 0x6ffffdf5	; Prelinking timestamp
DT_GNU_CONFLICTSZ = 0x6ffffdf6	; Size of conflict section
DT_GNU_LIBLISTSZ = 0x6ffffdf7	; Size of library list
DT_CHECKSUM	= 0x6ffffdf8
DT_PLTPADSZ	= 0x6ffffdf9
DT_MOVEENT	= 0x6ffffdfa
DT_MOVESZ	= 0x6ffffdfb
DT_FEATURE_1	= 0x6ffffdfc	; Feature selection (DTF_*).
DT_POSFLAG_1	= 0x6ffffdfd	; Flags for DT_* entries, effecting
				; the following DT_* entry.
DT_SYMINSZ	= 0x6ffffdfe	; Size of syminfo table (in bytes)
DT_SYMINENT	= 0x6ffffdff	; Entry size of syminfo

DT_GNU_HASH	= 0x6ffffef5	; GNU-style hash table.
DT_TLSDESC_PLT	= 0x6ffffef6
DT_TLSDESC_GOT	= 0x6ffffef7
DT_GNU_CONFLICT	= 0x6ffffef8	; Start of conflict section
DT_GNU_LIBLIST	= 0x6ffffef9	; Library list
DT_CONFIG	= 0x6ffffefa	; Configuration information.
DT_DEPAUDIT	= 0x6ffffefb	; Dependency auditing.
DT_AUDIT	= 0x6ffffefc	; Object auditing.
DT_PLTPAD	= 0x6ffffefd	; PLT padding.
DT_MOVETAB	= 0x6ffffefe	; Move table.
DT_SYMINFO	= 0x6ffffeff	; Syminfo table.

DT_VERSYM	= 0x6ffffff0

DT_RELACOUNT	= 0x6ffffff9
DT_RELCOUNT	= 0x6ffffffa

DT_FLAGS_1	= 0x6ffffffb
DT_VERDEF	= 0x6ffffffc	; Address of version definition table
DT_VERDEFNUM	= 0x6ffffffd	; Number of version definitions
DT_VERNEED	= 0x6ffffffe	; Address of table with needed versions
DT_VERNEEDNUM	= 0x6fffffff	; Number of needed versions

DT_AUXILIARY    = 0x7ffffffd    ; Shared object to load before self
DT_FILTER       = 0x7fffffff    ; Shared object to get values from

; i386 relocation types
R_386_NONE	   = 0		; No reloc
R_386_32	   = 1		; Direct 32 bit
R_386_PC32	   = 2		; PC relative 32 bit
R_386_GOT32	   = 3		; 32 bit GOT entry
R_386_PLT32	   = 4		; 32 bit PLT address
R_386_COPY	   = 5		; Copy symbol at runtime
R_386_GLOB_DAT	   = 6		; Create GOT entry
R_386_JMP_SLOT	   = 7		; Create PLT entry
R_386_RELATIVE	   = 8		; Adjust by program base
R_386_GOTOFF	   = 9		; 32 bit offset to GOT
R_386_GOTPC	   = 10		; 32 bit PC relative offset to GOT
R_386_32PLT	   = 11
R_386_TLS_TPOFF	   = 14		; Offset in static TLS block
R_386_TLS_IE	   = 15		; Address of GOT entry for static TLS
				; block offset
R_386_TLS_GOTIE	   = 16		; GOT entry for static TLS block offset
R_386_TLS_LE	   = 17		; Offset relative to static TLS block
R_386_TLS_GD	   = 18		; Direct 32 bit for GNU version of
				; general dynamic thread local data
R_386_TLS_LDM	   = 19		; Direct 32 bit for GNU version of
				; local dynamic thread local data
				; in LE code
R_386_16	   = 20
R_386_PC16	   = 21
R_386_8		   = 22
R_386_PC8	   = 23
R_386_TLS_GD_32	   = 24		; Direct 32 bit for general dynamic
				; thread local data
R_386_TLS_GD_PUSH  = 25		; Tag for pushl in GD TLS code
R_386_TLS_GD_CALL  = 26		; Relocation for call to __tls_get_addr()
R_386_TLS_GD_POP   = 27		; Tag for popl in GD TLS code
R_386_TLS_LDM_32   = 28		; Direct 32 bit for local dynamic
				; thread local data in LE code
R_386_TLS_LDM_PUSH = 29		; Tag for pushl in LDM TLS code
R_386_TLS_LDM_CALL = 30		; Relocation for call to
				; __tls_get_addr() in LDM code
R_386_TLS_LDM_POP  = 31		; Tag for popl in LDM TLS code
R_386_TLS_LDO_32   = 32		; Offset relative to TLS block
R_386_TLS_IE_32	   = 33		; GOT entry for negated static TLS
				; block offset
R_386_TLS_LE_32	   = 34		; Negated offset relative to static
				; TLS block
R_386_TLS_DTPMOD32 = 35		; ID of module containing symbol
R_386_TLS_DTPOFF32 = 36		; Offset in TLS block
R_386_TLS_TPOFF32  = 37		; Negated offset in static TLS block
R_386_SIZE32	   = 38 	; 32-bit symbol size
R_386_TLS_GOTDESC  = 39		; GOT offset for TLS descriptor.
R_386_TLS_DESC_CALL = 40	; Marker of call through TLS
				; descriptor for relaxation.
R_386_TLS_DESC     = 41		; TLS descriptor containing
				; pointer to code and to
				; argument, returning the TLS
				; offset for the symbol.
R_386_IRELATIVE	   = 42		; Adjust indirectly by program base
R_386_GOT32X	   = 43		; Load from 32 bit GOT entry, relaxable.

; Pointer encoding in unwind info
DW_EH_PE_absptr = 0
DW_EH_PE_omit = 0xFF

DW_EH_PE_uleb128 = 0x01
DW_EH_PE_udata2 = 0x02
DW_EH_PE_udata4 = 0x03
DW_EH_PE_udata8 = 0x04
DW_EH_PE_sleb128 = 0x09
DW_EH_PE_sdata2 = 0x0A
DW_EH_PE_sdata4 = 0x0B
DW_EH_PE_sdata8 = 0x0C

DW_EH_PE_pcrel = 0x10
DW_EH_PE_textrel = 0x20
DW_EH_PE_datarel = 0x30
DW_EH_PE_funcrel = 0x40
DW_EH_PE_aligned = 0x50

DW_EH_PE_indirect = 0x80

; DWARF opcodes
DW_CFA_nop		= 0 ; no argument
DW_CFA_set_loc		= 1 ; arg: address
DW_CFA_advance_loc1 	= 2 ; arg: 1-byte delta
DW_CFA_advance_loc2	= 3 ; arg: 2-byte delta
DW_CFA_advance_loc4	= 4 ; arg: 4-byte delta
DW_CFA_offset_extended	= 5 ; arg1: ULEB128 reg, arg2: ULEB128 offs
DW_CFA_restore_extended	= 6 ; arg: ULEB128 reg
DW_CFA_undefined	= 7 ; arg: ULEB128 reg
DW_CFA_same_value	= 8 ; arg: ULEB128 reg
DW_CFA_register		= 9 ; arg1: ULEB128 reg, arg2: ULEB128 reg
DW_CFA_remember_state	= 10 ; no argument
DW_CFA_restore_state	= 11 ; no argument
DW_CFA_def_cfa		= 12 ; arg1: ULEB128 reg, arg2: ULEB128 offs
DW_CFA_def_cfa_register	= 13 ; arg: ULEB128 reg
DW_CFA_def_cfa_offset	= 14 ; arg: ULEB128 offs
DW_CFA_def_cfa_expression = 15 ; arg: expression block
DW_CFA_expression	= 16 ; arg1: ULEB128 reg, arg2: expression block
DW_CFA_offset_extended_sf = 17 ; arg1: ULEB128 reg, arg2: SLEB128 offs
DW_CFA_def_cfa_sf	= 18 ; arg1: ULEB128 reg, arg2: ULEB128 offs
DW_CFA_def_cfa_offset_sf = 19 ; arg: SLEB128 offs
DW_CFA_val_offset	= 20 ; arg1: ULEB128, arg2: ULEB128
DW_CFA_val_offset_sf	= 21 ; arg1: ULEB128, arg2: SLEB128
DW_CFA_val_expression	= 22 ; arg1: ULEB128, arg2: expression block

DW_CFA_advance_loc	= 0x40 ; +6 bits delta
DW_CFA_offset		= 0x80 ; +6 bits register, arg: ULEB128 offset
DW_CFA_restore		= 0xC0 ; +6 bits register

reg_ebx = 3
reg_esp = 4
reg_ebp = 5
reg_retaddr = 8

; fasm magic
macro declare_cfi_storage func
{
func#_cfi_storage:
	rb	func#_cfi_size
}

macro cfi_startproc func
{
_current_cfi_func equ func
_current_cfi_storage equ func#_cfi_storage
_current_cfi_size = 0
_current_cfi_eip = $
}

macro cfi_endproc func
{
func#_cfi_size = _current_cfi_size
}

macro _cfi_store_byte [val]
{
store byte val at _current_cfi_storage+_current_cfi_size
_current_cfi_size = _current_cfi_size + 1
}
macro _cfi_store_word [val]
{
store word val at _current_cfi_storage+_current_cfi_size
_current_cfi_size = _current_cfi_size + 2
}
macro _cfi_store_dword [val]
{
store dword val at _current_cfi_storage+_current_cfi_size
_current_cfi_size = _current_cfi_size + 4
}

macro _cfi_update_eip
{
_cfi_delta = $ - _current_cfi_eip
_current_cfi_eip = $
if _cfi_delta = 0
; nothing to do
else if _cfi_delta < 0x40
	_cfi_store_byte DW_CFA_advance_loc+_cfi_delta
else if _cfi_delta < 0x100
	_cfi_store_byte DW_CFA_advance_loc1, _cfi_delta
else if _cfi_delta < 0x10000
	_cfi_store_byte DW_CFA_advance_loc2
	_cfi_store_word _cfi_delta
else
	_cfi_store_byte DW_CFA_advance_loc4
	_cfi_store_dword _cfi_delta
end if
}

macro cfi_def_cfa_offset offs
{
_cfi_update_eip
_cfi_store_byte DW_CFA_def_cfa_offset, offs
}

macro cfi_offset reg, offs
{
if offs < 0 | offs >= 0x80
err Another encoding is needed
end if
_cfi_update_eip
_cfi_store_byte DW_CFA_offset+reg, offs
}

macro cfi_restore reg
{
_cfi_update_eip
_cfi_store_byte DW_CFA_restore+reg
}

macro align_zero value
{ rb (value - 1) - (rva $ + value - 1) mod value }
