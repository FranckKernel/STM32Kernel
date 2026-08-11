# ARM Thumb-2: Complete `IT` Instruction Reference

This document covers every single variant of the `IT` (If-Then) instruction in Thumb-2.

---

## Legend

- **`C`** = The condition you write (e.g., `EQ`, `GT`, `NE`, `CS`).
- **`Opp`** = The logical opposite of `C` (e.g., `NE` if `C` is `EQ`, `LE` if `C` is `GT`).
- **NOP** = The instruction is fetched but does not write its result.

---

## Table 1: Full `IT` Variant Reference

| Mnemonic | Suffix Length | Total Instr. | Pattern (Instr 1→4) | Description |
| :--- | :--- | :--- | :--- | :--- |
| `IT` | 0 | 1 | `C` | Only the very next instruction is conditional. |
| `ITT` | 1 | 2 | `C, C` | Next 2 instructions both execute on `C`. |
| `ITE` | 1 | 2 | `C, Opp` | Then (`C`), Else (`Opp`). |
| `ITTT` | 2 | 3 | `C, C, C` | All 3 execute on `C`. |
| `ITTE` | 2 | 3 | `C, C, Opp` | Then, Then, Else. |
| `ITET` | 2 | 3 | `C, Opp, C` | Then, Else, Then. |
| `ITEE` | 2 | 3 | `C, Opp, Opp` | Then, Else, Else. |
| `ITTTT` | 3 | 4 | `C, C, C, C` | All 4 execute on `C`. |
| `ITTTE` | 3 | 4 | `C, C, C, Opp` | Then, Then, Then, Else. |
| `ITTET` | 3 | 4 | `C, C, Opp, C` | Then, Then, Else, Then. |
| `ITTEE` | 3 | 4 | `C, C, Opp, Opp` | Then, Then, Else, Else. |
| `ITETT` | 3 | 4 | `C, Opp, C, C` | Then, Else, Then, Then. |
| `ITETE` | 3 | 4 | `C, Opp, C, Opp` | Then, Else, Then, Else. |
| `ITEET` | 3 | 4 | `C, Opp, Opp, C` | Then, Else, Else, Then. |
| `ITEEE` | 3 | 4 | `C, Opp, Opp, Opp` | Then, Else, Else, Else. |

---

## Table 2: Real Code Examples (Using `EQ` as `C`)

All examples use `EQ` (Z==1) as the base condition. `Opp` = `NE` (Z==0).  
**✓** = Executes. **✗** = NOP (does nothing).

### 1 Instruction (Suffix length 0)

| Variant | Code | If `Z=1` (EQ true) | If `Z=0` (EQ false) |
| :--- | :--- | :--- | :--- |
| **`IT`** | `CMP R0, #0`<br>`IT EQ`<br>`MOVEQ R1, #42`<br>`MOV R2, #99` | `MOVEQ` = ✓<br>`MOV` = ✓ | `MOVEQ` = ✗<br>`MOV` = ✓ |

---

### 2 Instructions (Suffix length 1)

| Variant | Code | If `Z=1` (EQ true) | If `Z=0` (EQ false) |
| :--- | :--- | :--- | :--- |
| **`ITT`** | `CMP R0, #0`<br>`ITT EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2` | R1=✓, R2=✓ | R1=✗, R2=✗ |
| **`ITE`** | `CMP R0, #0`<br>`ITE EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R1, #2` | R1=✓ (gets 1)<br>R1=✗ (doesn't overwrite) | R1=✗<br>R1=✓ (gets 2) |

---

### 3 Instructions (Suffix length 2)

| Variant | Code | If `Z=1` (EQ true) | If `Z=0` (EQ false) |
| :--- | :--- | :--- | :--- |
| **`ITTT`** | `CMP R0, #0`<br>`ITTT EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2`<br>`MOVEQ R3, #3` | All 3 ✓ | All 3 ✗ |
| **`ITTE`** | `CMP R0, #0`<br>`ITTE EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2`<br>`MOVNE R3, #3` | R1=✓, R2=✓, R3=✗ | R1=✗, R2=✗, R3=✓ |
| **`ITET`** | `CMP R0, #0`<br>`ITET EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R2, #2`<br>`MOVEQ R3, #3` | R1=✓, R2=✗, R3=✓ | R1=✗, R2=✓, R3=✗ |
| **`ITEE`** | `CMP R0, #0`<br>`ITEE EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R2, #2`<br>`MOVNE R3, #3` | R1=✓, R2=✗, R3=✗ | R1=✗, R2=✓, R3=✓ |

---

### 4 Instructions (Suffix length 3)

| Variant | Code | If `Z=1` (EQ true) | If `Z=0` (EQ false) |
| :--- | :--- | :--- | :--- |
| **`ITTTT`** | `CMP R0, #0`<br>`ITTTT EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2`<br>`MOVEQ R3, #3`<br>`MOVEQ R4, #4` | All 4 ✓ | All 4 ✗ |
| **`ITTTE`** | `CMP R0, #0`<br>`ITTTE EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2`<br>`MOVEQ R3, #3`<br>`MOVNE R4, #4` | R1,R2,R3=✓; R4=✗ | R1,R2,R3=✗; R4=✓ |
| **`ITTET`** | `CMP R0, #0`<br>`ITTET EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2`<br>`MOVNE R3, #3`<br>`MOVEQ R4, #4` | R1,R2,R4=✓; R3=✗ | R1,R2,R4=✗; R3=✓ |
| **`ITTEE`** | `CMP R0, #0`<br>`ITTEE EQ`<br>`MOVEQ R1, #1`<br>`MOVEQ R2, #2`<br>`MOVNE R3, #3`<br>`MOVNE R4, #4` | R1,R2=✓; R3,R4=✗ | R1,R2=✗; R3,R4=✓ |
| **`ITETT`** | `CMP R0, #0`<br>`ITETT EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R2, #2`<br>`MOVEQ R3, #3`<br>`MOVEQ R4, #4` | R1,R3,R4=✓; R2=✗ | R1,R3,R4=✗; R2=✓ |
| **`ITETE`** | `CMP R0, #0`<br>`ITETE EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R2, #2`<br>`MOVEQ R3, #3`<br>`MOVNE R4, #4` | R1,R3=✓; R2,R4=✗ | R1,R3=✗; R2,R4=✓ |
| **`ITEET`** | `CMP R0, #0`<br>`ITEET EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R2, #2`<br>`MOVNE R3, #3`<br>`MOVEQ R4, #4` | R1,R4=✓; R2,R3=✗ | R1,R4=✗; R2,R3=✓ |
| **`ITEEE`** | `CMP R0, #0`<br>`ITEEE EQ`<br>`MOVEQ R1, #1`<br>`MOVNE R2, #2`<br>`MOVNE R3, #3`<br>`MOVNE R4, #4` | R1=✓; R2,R3,R4=✗ | R1=✗; R2,R3,R4=✓ |

---

## The Golden Rule (Quick Mnemonic)

- The **first instruction** after `IT` is **always** `C` (the condition you wrote).
- **Every letter** after `IT` (`T` or `E`) maps to the **next instruction** in order:
  - `T` = **T**hen → same as `C`.
  - `E` = **E**lse → opposite of `C`.
- **Total instructions in the block** = `1 + (number of letters after IT)`.
- Maximum suffix length is **3 letters**, giving a maximum of **4 instructions**.

### Example:
If you see `ITETE GT`:
- `IT` + `E T E` = 3 letters = 4 instructions.
- Pattern: `GT`, `LE`, `GT`, `LE`.

---

## Forbidden Instructions Inside an IT Block

The following instructions **cannot** be placed inside an IT block. The assembler will throw an error if you try:

- Any branch instruction: `B`, `BL`, `BLX`, `BX`.
- `CBZ` / `CBNZ`.
- `IT` itself (you cannot nest IT blocks).
- Any instruction that modifies the PC directly.

---

## How the Assembler Helps You

If you write old-style ARM code like this:

```asm
CMP   R0, #5
MOVEQ R1, #10
MOVNE R1, #20
