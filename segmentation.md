CORRIDA 1
segmento 0:
	base = 100
	bound = 29
	(crece hacia arriba)
segmento 1:
	base = 66
	bound = 29
	(crece hacia abajo)

addres space virtual -> 64 -> 6 bits

VA 0: 0x13 (19) -> 0x13 (010011) -> topbit = 0 -> segment 0 -> 100 + 19 = 119 > 129 ? NO -> PA: 0x77 (119) 
	

VA 1: 0x11 (17) -> 0x11 (010001)  -> topbit = 0 -> segment 0 -> 100 + 17 = 117 > 129 ? NO -> PA: 0x75 (117)





CORRIDA 2
segmento 0:
	base = 80
	bound = 22
segmento 1:
	base = 185
	bound = 29

VA 0: 0x1f (31) -> 0x1f (011111) -> topbit = 0 -> segmento 0 -> 80 + 31 = 121 > 102 ? SI -> SEG VIO

VA 1: 0x09 (9) -> 0x09 (001001) -> topbit = 0 -> segmento 0 -> 9 + 80 = 89 > 102 ? NO -> PA: 0x59 (89)







Corrida 1 original:
	VA0: 19 --> 119
	VA1: 17 --> 117
Corrida 2 original:
	VA0: 31 --> Segmentation Fault
	VA1: 9 --> 89


Corrida 1 modificada:
	
segmento 0:
	base = 72
	bound = 18
segmento 1:
	base = 66
	bound = 29

	VA0: 19 --> 72 + 19 = 91 > 89 ? Si -> Segmentation Fault
	VA1: 17 --> 72 + 17 = 89 > 89 ? No -> PA: 89


Corrida 2 modificada:

segmento 0:
	base = 88
	bound = 32
segmento 1:
	base = 185
	bound = 29


	VA0: 31 -> 31 + 88 = 119 > ? No --> PA:119
	VA1: 9 --> 117 (No puedo) 9 + 88 = 97 != 117
Ambas direcciones virtuales caen en segmento 0, tendría que encontrar una base tal que 31 + base = 119 y 9 + base = 117



























