function level_desert_night_interact_button1()
	print("[LUA] Button1 Interact called")

	togglePointlight(0)

	return 0
end

function level_desert_night_interact_button2()
	print("[LUA] Button2 Interact called")

	togglePointlight(1)

	return 0
end

function level_desert_night_interact_button3()
	print("[LUA] Button2 Interact called")

	togglePointlight(2)

	return 0
end

function level_desert_night_interact_button4()
	print("[LUA] Button2 Interact called")

	togglePointlight(3)

	return 0
end

function level_desert_night_interact_button5()
	print("[LUA] Button3 Interact called")

	--x,y,z = getObjectPosition(0)
	--setObjectPosition(0,x+10,y,z)

	health = getObjectHealth(0)
	print("Player has "..health.." health")

	if(health < 100) then
		addToObjectHealth(0,0.1)
	end

	health = getObjectHealth(0)
	print("Player has "..health.." health")



	return 0
end