

define rkarm 
	target extended-remote :4242
end

define reset
	monitor reset halt
end

rkarm 

b _reset
b main 

reset 
activate_dashboard
