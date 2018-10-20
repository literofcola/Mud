function skill_kidney_shot_cost(caster, target, skill)
  if(not caster:CanAttack(target)) then
    caster:Send("You can't attack that target.\n\r")
    return 0
  end
  if(not caster:GetCharacterRoom(target)) then
    caster:Send("That target is out of range.\n\r")
    return 0
  end
  if(not caster:HasResource(RESOURCE_ENERGY, 25)) then
      caster:Send("You don't have enough energy.\n\r")
      return 0
  end
  if(not caster:HasResource(RESOURCE_COMBO, 1)) then
      caster:Send("You don't have any combo points.\n\r")
      return 0
  end
  return 1
end
function skill_kidney_shot_cast(caster, target, skill)
  local combos = caster:SpendComboPoints(target)
  if(combos == 0) then
      caster:Send("You don't have combo points on that target.\n\r")
      return
  end
  caster:EnterCombat(target)
  target:EnterCombat(caster)
  caster:ConsumeEnergy(35)
  local attack = caster:DoAttackRoll(target, SCHOOL_PHYSICAL)
  if(attack == ATTACK_MISS) then
    caster:Send("|WYour Kidney Shot misses " .. target:GetName() .. "|X\n\r")
    target:Send("|W" .. caster:GetName() .. "'s Kidney Shot misses you.|X\n\r")
    caster:Message("|W" .. caster:GetName() .. "'s Kidney Shot misses " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, target)
    return
  elseif (attack == ATTACK_DODGE) then
    caster:Send("|W" .. target:GetName() .. " dodges your Kidney Shot.|X\n\r")
    target:Send("|WYou dodge " .. caster:GetName() .. "'s Kidney Shot.|X\n\r")
    caster:Message("|W" .. target:GetName() .. " dodges " .. caster:GetName() .. "'s Kidney Shot.|X", MSG_ROOM_NOTCHARVICT, target)
    return
  elseif (attack == ATTACK_PARRY) then
    caster:Send("|W" .. target:GetName() .. " parries your Kidney Shot.|X\n\r")
    target:Send("|WYou parry " .. caster:GetName() .. "'s Kidney Shot.|X\n\r")
    caster:Message("|W" .. target:GetName() .. " parries " .. caster:GetName() .. "'s Kidney Shot.|X", MSG_ROOM_NOTCHARVICT, target)
    return
  else
    caster:Send("|WYour Kidney Shot hits " .. target:GetName() .. ".|X\r\n")
    target:Send("|W" .. caster:GetName() .. "'s Kidney Shot hits you.|X\r\n")
    caster:Message("|W" .. caster:GetName() .. "'s Kidney Shot hits " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, target)
    target:CancelAutoAttack()
    target:AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Kidney Shot", SPELLAFFECT_VISIBLE, 1, 0, combos+1, AFFECT_NONE, skill, "Stunned")
  end
end
function skill_kidney_shot_apply(caster, target, affect)
  caster:Send(target:GetName() .. " is stunned by your Kidney Shot.|X\r\n")
  target:Send("|WYou have been stunned by " .. caster:GetName() .. "'s Kidney Shot.|X\r\n")
  caster:Message("|W" .. target:GetName() .. " has been stunned by " .. caster:GetName() .. "'s Kidney Shot.|X", MSG_ROOM_NOTCHARVICT, target)
  affect:ApplyAura(AURA_STUN, 1)
end
function skill_kidney_shot_remove(caster, target, affect)
  if(affect.name == "Kidney Shot") then
    target:Send("|WKidney Shot fades from you.|X\r\n")
    if(caster ~= nil) then
      caster:Send("|WYour Kidney Shot fades from " .. target:GetName() .. ".|X\r\n")
      target:Message("|WKidney Shot fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, caster)
    else
      target:Message("|WKidney Shot fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHAR, nil)
    end
  end
end
