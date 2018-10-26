function skill_prayer_of_healing_5_cost(caster, target, skill)
  if(not caster:HasResource(RESOURCE_MANA, 1070)) then
    caster:Send("You don't have enough mana.\n\r")
    return 0
  end
  return 1
end
function skill_prayer_of_healing_5_cast(caster, target, skill)
  caster:ConsumeMana(1070)
  local healval = math.ceil(1075 + caster:GetIntellect())
  local overheal = ""
  if(caster:GetGroup() == nil) then
    if(healval > (caster:GetMaxHealth() - caster:GetHealth())) then
        overheal =  " (" .. (healval - (caster:GetMaxHealth() - caster:GetHealth())) .. " overheal)" 
        healval = caster:GetMaxHealth() - caster:GetHealth()
    end
    caster:Send("|WYou have completed your spell. You gain " .. healval .. " health")
    if(overheal ~= 0) then
        caster:Send(overheal)
    end
    caster:Send(".|X\n\r")
    caster:Message("|W" .. caster:GetName() .. " has completed " .. caster:HisHer() .." spell. " .. caster:GetName() .. " gains " .. healval .. " health.|X", MSG_ROOM_NOTCHAR, nil)
    caster:OneHeal(caster, healval)
  else
    for i = 0, MAX_RAID_SIZE - 1 do
      local player = caster:GetGroup():GetMember(i)
      if(player ~= nil) then
        if(caster == player) then
          if(healval > (caster:GetMaxHealth() - caster:GetHealth())) then
            overheal =  " (" .. (healval - (caster:GetMaxHealth() - caster:GetHealth())) .. " overheal)" 
            healval = caster:GetMaxHealth() - caster:GetHealth()
          end
          caster:Send("|WYou have completed your spell. You gain " .. healval .. " health")
          if(overheal ~= 0) then
            caster:Send(overheal)
          end
          caster:Send(".|X\n\r")
          caster:Message("|W" .. caster:GetName() .. " has completed " .. caster:HisHer() .." spell. " .. caster:GetName() .. " gains " .. healval .. " health.|X", MSG_ROOM_NOTCHAR, nil)
        else
          if(healval > (player:GetMaxHealth() - player:GetHealth())) then
            overheal =  " (" .. (healval - (player:GetMaxHealth() - player:GetHealth())) .. " overheal)" 
            healval = player:GetMaxHealth() - player:GetHealth()
          end
          caster:Send("|WYou have completed your spell. " .. player:GetName() .. " gains " .. healval .. " health")
          if(overheal ~= 0) then
            caster:Send(overheal)
          end
          caster:Send(".|X\n\r")
          player:Send("|W" .. caster:GetName() .. " has completed " .. caster:HisHer() .. " spell. You gain " .. healval .. " health.|X\n\r")
          caster:Message("|W" .. caster:GetName() .. " has completed " .. caster:HisHer() .." spell. " .. player:GetName() .. " gains " .. healval .. " health.|X", MSG_ROOM_NOTCHARVICT, player)
        end
        caster:OneHeal(player, healval)
      end
    end
  end
end
