

define rkarm 
	target extended-remote :4242
end

define reset
	monitor reset halt
end

define reg_check
    echo \n=== GPIOB PB6 raw ===\n
    p/x *(unsigned int*)0x40020400
    p/x *(unsigned int*)0x40020404
    p/x *(unsigned int*)0x40020408
    p/x *(unsigned int*)0x4002040C
    p/x *(unsigned int*)0x40020420
    p/x *(unsigned int*)0x40020424

    echo \n=== PB6 fields ===\n
    echo MODER[13:12]  expect 0x2\n
    p/x (*(unsigned int*)0x40020400 >> 12) & 0x3
    echo OTYPER[6]     expect 0x0\n
    p/x (*(unsigned int*)0x40020404 >>  6) & 0x1
    echo OSPEEDR[13:12] any\n
    p/x (*(unsigned int*)0x40020408 >> 12) & 0x3
    echo PUPDR[13:12]  expect 0x0\n
    p/x (*(unsigned int*)0x4002040C >> 12) & 0x3
    echo AFRL[27:24]   expect 0x2\n
    p/x (*(unsigned int*)0x40020420 >> 24) & 0xF

    echo \n=== TIM4 raw ===\n
    p/x *(unsigned int*)0x40000800
    p/x *(unsigned int*)0x40000804
    p/x *(unsigned int*)0x40000808
    p/x *(unsigned int*)0x4000080C
    p/x *(unsigned int*)0x40000810
    p/x *(unsigned int*)0x40000814
    p/x *(unsigned int*)0x40000818
    p/x *(unsigned int*)0x4000081C
    p/x *(unsigned int*)0x40000820
    p/x *(unsigned int*)0x40000824
    p/x *(unsigned int*)0x40000828
    p/x *(unsigned int*)0x4000082C
    p/x *(unsigned int*)0x40000834

    echo \n=== TIM4 CH1 fields ===\n
    echo CR1.CEN        expect 0x1\n
    p/x (*(unsigned int*)0x40000800 >> 0) & 0x1
    echo CCMR1.CC1S     expect 0x0\n
    p/x (*(unsigned int*)0x40000818 >> 0) & 0x3
    echo CCMR1.OC1PE    expect 0x1\n
    p/x (*(unsigned int*)0x40000818 >> 3) & 0x1
    echo CCMR1.OC1M     expect 0x6\n
    p/x (*(unsigned int*)0x40000818 >> 4) & 0x7
    echo CCER.CC1E      expect 0x1\n
    p/x (*(unsigned int*)0x40000820 >> 0) & 0x1
    echo CCER.CC1P      expect 0x0\n
    p/x (*(unsigned int*)0x40000820 >> 1) & 0x1
    echo PSC           expect 0x5F\n
    p/x *(unsigned int*)0x40000828 & 0xFFFF
    echo ARR           expect 0x3E7\n
    p/x *(unsigned int*)0x4000082C & 0xFFFF
    echo CCR1          expect 0x3E8\n
    p/x *(unsigned int*)0x40000834 & 0xFFFF

    echo \n=== struct-bug probe: 0x4000081E ===\n
    echo nonzero here + CCER=0 proves the ccer write is misaligned\n
    p/x *(unsigned int*)0x4000081E
end

rkarm 

file ./build/kernel.elf

b _reset
b main 

# reset 
activate_dashboard
