

define rkarm 
	target extended-remote :4242
end

define reset
	monitor reset halt
end

rkarm 

file ./build/kernel.elf

b _reset
b main 

reset 
activate_dashboard
