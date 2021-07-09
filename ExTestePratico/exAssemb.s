        .data

        .equ SFR_BASE_HI, 0xBF88        # 16 MSbits of SFR area       
        .equ TRISE, 0x6100              # TRISE address is 0xBF886100      
        .equ PORTE, 0x6110              # PORTE address is 0xBF886110      
        .equ LATE,  0x6120              # LATE  address is 0xBF886120

        .equ getChar, 2
        .equ printStr, 8
        .equ readCoreTimer, 11
        .equ resetCoreTimer, 12
        .text
        .globl main

main:
        lui  $t1, SFR_BASE_HI
        lw   $t2, TRISE($t1)            # 	read (TRISE register)
        andi $t2, $t2, 0xFFE0           # 	configure RE0 = RE1 = RE2 = RE3 = 0 output -> 0x1111 1111 1111 0000
        sw   $t2, TRISE($t1)            # 	write (TRISE register)

while:
        
        li   $v0, getChar               # getChar()
        syscall

# - quando é premida a tecla 0, liga o LED0 (porto RE0);
if0:
        bne  $v0, '0', if1              # if(tecla == '0') {
        lw   $t2, LATE($t1)             #       read (LATE register)
        ori  $t2, $t2, 0x0001           #       set (LATE0) -> 0x0000 0000 0000 0001
        sw   $t2, LATE($t1)             #       write (LATE register)
        j while                         # }

# - quando é premida a tecla 1, liga o LED1 (porto RE1);
if1:
        bne  $v0, '1', if2              # if(tecla == '1') {
        lw   $t2, LATE($t1)             #       read (LATE register)
        ori  $t2, $t2, 0x0002           #       set (LATE0) -> 0x0000 0000 0000 0010
        sw   $t2, LATE($t1)             #       write (LATE register)
        j while                         # }

# - quando é premida a tecla 2, liga o LED2 (porto RE2);
if2:
        bne  $v0, '2', if3              # if(tecla == '2') {
        lw   $t2, LATE($t1)             #       read (LATE register)
        ori  $t2, $t2, 0x0004           #       set (LATE0) -> 0x0000 0000 0000 0100
        sw   $t2, LATE($t1)             #       write (LATE register)
        j while                         # }

# - quando é premida a tecla 3, liga o LED3 (porto RE3);
if3:
        bne  $v0, '3', else             # if(tecla == '3') {
        lw   $t2, LATE($t1)             #       read (LATE register)
        ori  $t2, $t2, 0x0008           #       set (LATE0) -> 0x0000 0000 0000 1000
        sw   $t2, LATE($t1)             #       write (LATE register)
        j while                         # }

# - quando é premida outra tecla qualquer, liga o LED4 (porto RE4) durante 2 segundos, e a seguir apaga todos os 5 LEDS.
else:
        lw   $t2, LATE($t1)             #       read (LATE register)
        ori  $t2, $t2, 0x0010           #       set (LATE0) -> 0x0000 0000 0001 0000
        sw   $t2, LATE($t1)             #       write (LATE register)
        
        li   $a0, 2000                  # 2000 ms <=> 2 s -> delay(2000)                          
        jal delay

        lw   $t2, LATE($t1)
        andi $t2, $t2, 0xFFE0           # Passados os 2 segundos o programa apaga os 5 LEDS
        sw   $t2, LATE($t1)             #       write (LATE register)

        j		while				# jump to while
        jr		$ra					# jump to $ra
        
###################################
delay:	
for:	blez	$a0, end
		
		li	$v0, resetCoreTimer			# resetCoreTimer();
		syscall
		
sleep:	li	$v0, readCoreTimer			# while(readCoreTimer() < K);
		syscall
		blt	$v0, 20000, sleep

		addi	$a0, $a0, -1 
		j for
		
end:	jr	$ra