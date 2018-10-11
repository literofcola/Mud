
function skill_whirlwind_cost(caster, target, skill)
  if(not caster:HasResource(RESOURCE_RAGE, 25)) then
    caster:Send("You don't have enough rage.\n\r")
    return 0
  end
  return 1
end


