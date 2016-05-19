## RISC-V

RISC-V ist eine neue open-source ISA (instruction set architecture), die
ursprünglich für Lern- und Forschungsziele entwickelt wurde.

Es gibt zwei Versionen mit 32-bit und 64-bit Architektur. Wir werden uns
zuerst ausschließlich für die erste Variante interessieren.

Die Architektur ist in Module aufgeteilt, mit der Basis Integer Modul "I",
das in allen Implementierungen vorhanden sein muss. Es enthält ganzzahlige
Rechen-, Lade-, Store- und Kontrollflussinstruktionen. Es kann durch folgende
Standartmodule erweitert werden:

+ "M" - ganzzahlige Multiplikation und Division
+ "A" - atomicity für inter-processor Synchronization
+ "F" und "D" - für floats und doubles entsprechend

Die Instruktionen der Standard ISA sind 32-bit lang und sie sind aligned auf die 32-bit Grenzen. Jedoch unterstützt Risc-V auch Befehle mit variablen Länge, die aus einer beliebigen Anzahl von 16-bit parcels bestehen können.

Standartmässig wird little-endian als Speicherformat verwendet.

Es gibt 31 Allzweckregister (x1 - x31). Zusätzlich gibt es ein Register x0, der mit der Konstante 0 hardwired ist, und einen Programmzähler. Sie sind alle 32 bit breit.

### Integer Basis Instruktionen

Keine integer Rechenbefehle verursachen arithmetische Ausnahmen.

**Register-Immediate**

```asm
ADDI rd, rs1, 0  ; addiert 12-bit immediate mit dem Register r1, Ziel: rd
SLTI rd, rs1, 0  ; set less than immediate, rd = 1, falls rs1 < 0, sonst rd=0
SLTIU rd, rs1, 1 ; dasselbe, aber unsigned
```

ANDI, ORI, XORI sind logische Operationen, die AND, OR und XOR entsprechen. Ihr Befehlsformat ist gleich dem obigen.

Es gibt logische und arithmetische Shifts (SLLI, SRLI, SLAI, SRAI). Die Anzahl der geshifteten Bits ist gleich der Zahl, die aus den ersten fünf Bits der immediate entsteht.

In den nächsten beiden Befehlen ist die immediate 20 bit lang, anstatt 12.

```asm
LUI dest, immediate  ; load upper immediate
AUIPC dest, immediate  ; add upper immediate to pc
```

LUI platziert die Immediate in die höheren 20 bits des Zielregisters und die unteren 12 werden mit 0 gefüllt.

AUIPC addiert die Immediate mit den höheren 20 bits des Instruktionszählers und speichert das Ergebnis in das Zielregister.

**Register-Register**

```asm
ADD/SLT/SLTU dest, src1, src2
AND/OR/XOR   dest, src1, src2
SLL/SRL      dest, src1, src2
SUB/SRA      dest, src1, src2

SLTU rd, x0, rs2 ; setzt rd zu 1 nur wenn rs2 = 0
```

**NOP Instruktion**

```asm
ADDI x0, x0, 0
```

**Control Transfer Instruktionen**

```asm
JAL rd, immediate  ; rd = (pc + 4), Immediate 20 bit lang, range +-1MiB
```

JAL (jump and link) setzt den Befehlszähler auf den Wert (pc + immediate) und der vorherige Wert des (Befehlszählers + 4) wird in das Register rd gespeichert, der als eine Rücksprungadresse verwendet werden kann. Nach den Konventionen benutzt man dafür x1 und, wenn JAL ein unconditional jump ist, dann x0.

```asm
JALR rd, rs1, immediate  ; rd = (pc + 4), Immediate 12 bit
```

JALR setzt den Befehlszähler auf (rs1 + imm). Zusammen mit dem Befehl LUI kann man auf eine beliebige Adresse im Adressraum springen.

**Conditional Branches**

Ein wichtiger Aspekt der Risc-V ISA ist, dass es keine flags gibt, stattdessen werden Branchbefehle benutzt.

```asm
BEQ/BNE src1, src2, offset ; branch equal/not equal
BLT[U]/BGE[U]  src1, src2, offset  ; branch less/greater than
```

Falls die Bedingung erfüllt wird, wird der offset zu dem Befehlszähler addiert.

**Load/Store Instruktionen**

```asm
LW/LH/LB rd, rs1, offset ; kopiert 32/16/8 bits vom Speicher von der Adresse (rs1+offset)
SW/SH/SB rd, rs1, offset ; kopiert den Wert vom rd in den Speicher
```

Es gibt auch LHU und LBU (analog SHU, SBU). Der Unterschied ist, dass der kopierte 16/8 bit Wert wird zero-extended zu 32 bits, wogegen bei normalen LH und LB wird das sign-extended.
