Original thumb: 16 bit architecture
Thumb 2: Mix 16 bit and 32 bit instruction
ARM (A32) : All instruction are 32bit wide


ow registers: R0–R7 – freely usable in all instructions (both 16-bit and 32-bit).

High registers: R8–R15 – limited usage in 16-bit instructions; mostly used in 32-bit instructions.
R13 = SP (Stack Pointer), R14 = LR (Link Register), R15 = PC (Program Counter).

LR: Link register. Remeber jal (jump and link) from mips. Similar. It holds the return address from a function call
Need to use the stack for nested function. Hence prologue and epilogue. 


====
# ARM Thumb-2: Program Status Register & Flags

In ARM (specifically Cortex‑M / Thumb‑2), there isn't a standalone "flags register" like x86's `EFLAGS`. 
Instead, the flags live inside a special 32‑bit register called the **xPSR** (Program Status Register), which is logically split into three sub‑registers.

The **flags** themselves are stored in the **APSR** (Application Program Status Register).

---

## 1. The Core Four Condition Flags (APSR)

These are the only flags used by conditional branches (`BEQ`, `BGT`) and `IT` blocks. They are automatically updated when you execute an instruction with the `S` suffix (e.g., `ADDS`, `SUBS`).

| Flag | Name | What it actually means | When is it set to 1? | Example |
| :--- | :--- | :--- | :--- | :--- |
| **N** | **Negative** | The result is negative. | Bit 31 (the sign bit) of the result is `1`. | `MOV R0, #-1` → `ADDS R1, R0, #0` (N=1). |
| **Z** | **Zero** | The result is zero. | Every bit of the result is `0`. | `SUBS R1, R1, R1` (Z=1). |
| **C** | **Carry** (or **Borrow**) | **Unsigned** overflow occurred. | - **Addition**: Result > `0xFFFFFFFF` (carry out). <br>- **Subtraction**: No borrow occurred (i.e., result ≥ 0). <br>- **Shifts**: The last bit shifted out.                       | `ADDS R0, R0, #1` when `R0 = 0xFFFFFFFF` (C=1). |
| **V** | **Overflow** | **Signed** overflow occurred. | Adding two positives gives a negative, OR adding two negatives gives a positive. (The sign of the result is mathematically wrong). | `ADDS R0, R0, #1` when `R0 = 0x7FFFFFFF` (largest positive) → V=1. |

---

## 2. Special Extras (APSR)

In ARMv7‑M and later, there are additional APSR bits used for DSP (Digital Signal Processing) and SIMD (Single Instruction Multiple Data) operations.

| Flag | Name | What it does |
| :--- | :--- | :--- |
| **Q** | **Saturation** | Set to `1` if a "saturating" arithmetic instruction (`QADD`, `QSUB`) clipped the result to a maximum/minimum value instead of wrapping around. Once set, it stays set until you manually clear it. |
| **GE\[3:0\]** | **Greater-than-or-Equal** | This is **4 separate bits** (one for each byte lane) used exclusively by SIMD instructions. They indicate which byte/halfword lanes in a parallel operation generated a borrow or comparison result. |

---

## 3. The Complete xPSR Layout (All 32 Bits)

The xPSR contains three logical sub‑registers packed into one 32‑bit register:

Bits 31–27: APSR (N, Z, C, V, Q)
Bits 26–25: Reserved / EPSR (some control bits)
Bit 24 : EPSR.T (Thumb state – always 1 for Thumb‑2)
Bits 15–10: EPSR.IT (the hidden ITSTATE – manages the IT instruction)
Bits 8–0 : IPSR (Interrupt number currently being processed)



| Sub‑register | Bits | Purpose |
| :--- | :--- | :--- |
| **APSR** | 31–27 (and Q/GE) | **Flags** – used for conditional execution. |
| **IPSR** | 8–0 | **Interrupt Program Status Register** – holds the exception/interrupt number. `0` = Thread mode; `14` = PendSV; etc. |
| **EPSR** | 26–24, 15–10 | **Execution Program Status Register** – holds the `T` bit (Thumb state) and the `ITSTATE` for conditional blocks. |

---

## 4. How to Read and Write the Flags

You cannot use `MOV` to access the APSR directly. You must use the special **`MRS`** (Move to Register from Special register) and **`MSR`** (Move to Special register from Register) instructions.

### Read the current flags into R0
```asm

MRS  R0, APSR      ; R0 now contains N, Z, C, V, Q, and GE bits
```


## 5. Condition Code Cheat Sheet

This is how the CPU translates `N`, `Z`, `C`, and `V` into the condition suffixes you use in `B{cond}` and `IT{cond}`.

| Suffix | Meaning | Flag Check |
| :--- | :--- | :--- |
| **EQ** | **Equal** | `Z == 1` |
| **NE** | **Not Equal** | `Z == 0` |
| **CS** / **HS** | **Carry Set** / **Unsigned Higher or Same** | `C == 1` |
| **CC** / **LO** | **Carry Clear** / **Unsigned Lower** | `C == 0` |
| **MI** | **Minus / Negative** | `N == 1` |
| **PL** | **Plus / Positive or Zero** | `N == 0` |
| **VS** | **Overflow Set** | `V == 1` |
| **VC** | **Overflow Clear** | `V == 0` |
| **HI** | **Unsigned Higher** | `(C == 1) && (Z == 0)` |
| **LS** | **Unsigned Lower or Same** | `(C == 0) \|\| (Z == 1)` |
| **GE** | **Signed Greater than or Equal** | `N == V` |
| **LT** | **Signed Less Than** | `N != V` |
| **GT** | **Signed Greater Than** | `(Z == 0) && (N == V)` |
| **LE** | **Signed Less than or Equal** | `(Z == 1) \|\| (N != V)` |
| **AL** | **Always** (default) | (Ignored – no flag check) |

=====
# ARM Thumb-2 Instruction Set Reference

## 1. Data Processing (Arithmetic & Logic)

These update the APSR (Application Program Status Register) flags (`N`, `Z`, `C`, `V`) **unless** you add an `S` suffix (e.g., `ADDS`).

| Instruction | 16-bit (narrow) example | 32-bit (wide) example |
| :--- | :--- | :--- |
| `ADD` | `ADD R0, R1` (R0 += R1) | `ADD R0, R1, R2` (R0 = R1 + R2) |
| `SUB` | `SUB R0, #1` | `SUB R0, R1, R2` |
| `MOV` | `MOV R0, #42` | `MOV R0, #0x12345678` (large imm) |
| `CMP` | `CMP R0, #10` | `CMP R0, R1` |
| `AND` / `ORR` / `EOR` | (limited) | `AND R0, R1` or `AND R0, R1, R2` |
| `LSL` / `LSR` / `ASR` (shifts) | `LSL R0, R1, #2` | `LSL R0, R1, R2` |


No CMPS, cmp already update the status flag register

> **Key detail:** 16‑bit immediate values are limited (8‑bit rotated). 32‑bit instructions allow larger immediates.

---

## 2. Memory Access (Load / Store)

ARM is a **load‑store** architecture: only `LDR` and `STR` touch memory. ALU operations work only on registers.

| Instruction | Description | Example |
| :--- | :--- | :--- |
| `LDR` | Load 32‑bit word | `LDR R0, [R1]` (load from address in R1) |
| `STR` | Store 32‑bit word | `STR R0, [R1, #4]` (store at R1+4) |
| `LDRB` / `STRB` | Load / Store 8‑bit byte | `LDRB R0, [R1, R2]` |
| `LDRH` / `STRH` | Load / Store 16‑bit halfword | `LDRH R0, [R1]` |
| `LDRD` / `STRD` | Load / Store doubleword (64‑bit) – **32‑bit only** | `LDRD R0, R1, [R2]` |

### Addressing Modes
- `[Rn]` – direct.
- `[Rn, #offset]` – immediate offset (indexed).
- `[Rn, Rm]` – register offset.
- `[Rn], #offset` – post‑indexed (update `Rn` after access).
- `[Rn, #offset]!` – pre‑indexed with write‑back (update `Rn` before access).

---

## 3. Branch & Control Flow

| Instruction | Description | Example |
| :--- | :--- | :--- |
| `B` | Unconditional branch | `B loop_label` |
| `B{cond}` | Conditional branch (`EQ`, `NE`, `GT`, `LT`, etc.) | `BEQ exit` (branch if Zero flag = 1) |
| `BL` | Branch with Link (function call) | `BL my_function` (PC = address, LR = return addr) |
| `BX` | Branch and eXchange (return from function) | `BX LR` (jump to address in LR) |
| `BLX` | Branch with Link and eXchange (call ARM/Thumb code) | `BLX R0` |
| `CBZ` / `CBNZ` | Compare and Branch if Zero / Non‑Zero (16‑bit, very useful) | `CBZ R0, skip` (if R0 == 0, branch) |


---

## 4. Special Thumb‑2 Unique Instructions

| Instruction | What it does |
| :--- | :--- |
| `IT` (If‑Then) | Creates a conditional block of up to 4 instructions. |
| `BFI` / `BFC` | Bitfield Insert / Clear – great for hardware register manipulation. |
| `UBFX` / `SBFX` | Unsigned / Signed Bitfield Extract. |
| `REV` / `REV16` / `REVSH` | Byte‑reverse (endianness swap). |
| `RBIT` | Reverse bits in a register. |
| `CLZ` | Count Leading Zeros. |
| `SXTB` / `SXTH` / `UXTB` / `UXTH` | Sign / Zero extend byte/halfword to 32‑bit. |


### 4.5 IT 


```c
if (R0 == 5) {
    R1 = 10;
} else {
    R1 = 20;
}
R2 = 99;


```

Scenario A: Using a Conditional Branch (B{cond})
```asm
        CMP   R0, #5      ; 1. SET flags. (Z=1 if R0==5, else Z=0)
        BNE   else_label  ; 2. USE flags. If Z==0 (Not Equal), JUMP to else_label.
                          ;    If Z==1 (Equal), we fall through to the next line.
        
        MOV   R1, #10     ; 3. Executes ONLY if R0==5 (because BNE didn't jump).
        B     end_if      ; 4. Unconditional jump to skip the else block.

else_label:
        MOV   R1, #20     ; 5. Executes ONLY if R0 != 5.

end_if:
        MOV   R2, #99     ; 6. Always executes, regardless of the condition.
```

Scenario B: Using an IT Block (No Branches)
```asm
        CMP   R0, #5      ; 1. SET flags (Z=1 if equal).
        ITE   EQ          ; 2. LOAD ITSTATE: "Next = EQ, Next = NE".
        MOVEQ R1, #10     ; 3. CPU checks ITSTATE. If Z=1, execute; else, NOP.
        MOVNE R1, #20     ; 4. CPU checks ITSTATE. If Z=0, execute; else, NOP.
        MOV   R2, #99     ; 5. ITSTATE is 0. This runs unconditionally.
```


Other example of the IT familly: 

```asm
ITTEE   EQ          ; Suffix "TEE" = 3 letters -> 4 total instructions
MOVEQ   R0, #1     ; 1st: T (Execute if EQ)
MOVEQ   R1, #2     ; 2nd: T (Execute if EQ)
MOVNE   R2, #3     ; 3rd: E (Execute if NE - opposite of EQ)
MOVNE   R3, #4     ; 4th: E (Execute if NE)

```

T: -> Condition C  (Execute if C)
E: -> Not C         (Execute if not C)

T stands for Then, E for else. 


IT TTEE : CC, notC notC 
IT TTTE : CCC, Not C  

Must match: 

```asm 
ITTEE   EQ          ; Pattern = EQ, EQ, NE, NE
MOVEQ   R0, #1      ; 1st: EQ -> matches T (OK)
MOVEQ   R1, #2      ; 2nd: EQ -> matches T (OK)
MOVEQ   R2, #3      ; 3rd: EQ -> should be NE! ERROR!
MOVNE   R3, #4      ; 4th: NE -> matches E (OK)
```

And you can't have 

```asm
IT TE EQ 
MOVEQ R0, #1 ; 
MOVLT R2, #2 ; Less then is not exactly the same boolean table as not equal!   

```
Otherwise, assembler error

---



## 5. Stack & Push / Pop (Function Prologue / Epilogue)

Thumb‑2 has dedicated `PUSH` and `POP` instructions.

```asm
PUSH {R0-R3, LR}   ; Save registers and link register on stack
...
POP  {R0-R3, PC}   ; Restore and return (PC = popped LR)
