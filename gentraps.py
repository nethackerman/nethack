x = 0
y = 0
n = 0
traps = [ 'polymorph', 'fire' ]
lines = [ it.strip() for it in open('dat/vinstquest.des', 'r').readlines() ]
gogo = False
for it in lines:
	if not gogo:
		gogo = 'MAP' == it
	else:
		if 'ENDMAP' == it:
			break
		x = 0
		for x in range(0, len(it)):
			if '#' == it[x]:
				print('TRAP:"%s",(%d,%d)' % (traps[n % len(traps)], x, y))
				n += 1
		y += 1
