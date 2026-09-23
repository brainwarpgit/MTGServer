object_mobile_vehicle_swoopracer_black = object_mobile_vehicle_shared_swoopracer_black:new {
	templateType = VEHICLE,
	decayRate = 50, -- Damage tick per decay cycle, matching the standard swoop
	decayCycle = 600 -- Time in seconds per cycle
}

ObjectTemplates:addTemplate(object_mobile_vehicle_swoopracer_black, "object/mobile/vehicle/swoopracer_black.iff")
