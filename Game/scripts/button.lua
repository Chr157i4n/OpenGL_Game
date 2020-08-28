function test()
	print("[LUA] test called")

	a = "Test"

	ToggleFlashlight()
	TogglePointlight(0)

	return a
end

function interact_button1()
	print("[LUA] Button1 Interact called")

	TogglePointlight(0)

	return 0
end

function interact_button2()
	print("[LUA] Button2 Interact called")

	TogglePointlight(1)

	return 0
end