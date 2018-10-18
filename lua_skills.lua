function skill_sunder_armor_cost(caster, target, skill)
  if(not caster:CanAttack(target)) then
    caster:Send("You can't attack that target.\n\r")
    return 0
  end
  if(not target:IsAlive()) then
    caster:Send("That target is already dead!\n\r")
    return 0
  end
  if(not caster:GetCharacterRoom(target)) then
    caster:Send("That target is out of range.\n\r")
    return 0
  end
  if(not caster:HasResource(RESOURCE_RAGE, 15)) then
    caster:Send("You don't have enough rage.\n\r")
    return 0
  end
  return 1
end
function skill_sunder_armor_cast(caster, target, skill)
  caster:ConsumeRage(15)
  caster:EnterCombat(target)
  target:EnterCombat(caster)
  local threat = caster:GetMainhandDamageRandomHit()
  local attack = caster:DoAttackRoll(target, SCHOOL_PHYSICAL)
  if(attack == ATTACK_MISS) then
    caster:Send("|WYour Sunder Armor misses " .. target:GetName() .. "|X\n\r")
    target:Send("|W" .. caster:GetName() .. "'s Sunder Armor misses you.|X\n\r")
    caster:Message("|W" .. caster:GetName() .. "'s Sunder Armor misses " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, target)
    return
  elseif (attack == ATTACK_DODGE) then
    caster:Send("|W" .. target:GetName() .. " dodges your Sunder Armor.|X\n\r")
    target:Send("|WYou dodge " .. caster:GetName() .. "'s Sunder Armor.|X\n\r")
    caster:Message("|W" .. target:GetName() .. " dodges " .. caster:GetName() .. "'s Sunder Armor.|X", MSG_ROOM_NOTCHARVICT, target)
    return
  elseif (attack == ATTACK_PARRY) then
    caster:Send("|W" .. target:GetName() .. " parries your Sunder Armor.|X\n\r")
    target:Send("|WYou parry " .. caster:GetName() .. "'s Sunder Armor.|X\n\r")
    caster:Message("|W" .. target:GetName() .. " parries " .. caster:GetName() .. "'s Sunder Armor.|X", MSG_ROOM_NOTCHARVICT, target)
    return
  end
  target:AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Sunder Armor", false, true, 0, 30, AFFECT_NONE, skill, "Armor decreased by 4% per stack")
  --need to change how stackable affects work
  target:UpdateThreat(caster, threat, THREAT_OTHER)
end
function skill_sunder_armor_apply(caster, target, affect)
  --apply / update aura armor reduction
end
function skill_sunder_armor_remove(caster, target, affect)

end
