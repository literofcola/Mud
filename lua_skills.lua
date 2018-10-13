function skill_resurrection_cost(caster, target, skill)
  if(caster:InCombat()) then
    caster:Send("This spell cannot be can't while in combat.\n\r")
    return 0
  end
  if(target:IsNPC()) then
    caster:Send("This spell can only be cast on players.\n\r")
    return 0
  end
  if(target:IsAlive()) then
    caster:Send("That target is already alive!\n\r")
    return 0
  end
  if(not caster:GetCharacterAdjacentRoom(target)) then
    caster:Send("That target is out of range.\n\r")
    return 0
  end
  if(not caster:HasResource(RESOURCE_MANA, 1000)) then
    caster:Send("You don't have enough mana.\n\r")
    return 0
  end
  return 1
end

function skill_resurrection_cast(caster, target, skill)
  caster:ConsumeMana(1000)
  target:AsPlayer():SetQuery("res: ('accept', 'release')", caster:GetRoom(), ResQuery)
end
