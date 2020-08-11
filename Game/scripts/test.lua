a=7+11+math.sin(23.7)
a = a + 100

function AddStuff(a,b)
	print("[LUA] AddStuff("..a..","..b..") called \n")
	return a+b
end

function DoAThing(a,b)
	print("[LUA] DoAThing("..a..","..b..") called \n")

	c = HostFunction(a,b)

	return c
end