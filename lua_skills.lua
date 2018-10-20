function skill_blind_cost(caster, target, skill)
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
  if(not caster:HasResource(RESOURCE_RAGE, 30)) then
    caster:Send("You don't have enough rage.\n\r")
    return 0
  end
  return 1
end
function skill_blind_cast(caster, target, skill)
  caster:ConsumeEnergy(30)
  target:AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Blind", SPELLAFFECT_VISIBLE, 1, 0, 30, AFFECT_NONE, skill, "Blind and disoriented.")
  caster:EnterCombat(target)
  target:EnterCombat(caster)
  target:CancelAutoAttack()
  if(caster:GetTarget() == target) then
    caster:CancelAutoAttack()
  end
  --Messages & AttackRoll physical for blind
end
function skill_blind_apply(caster, target, affect)
  if(affect.name == "Blind") then
    affect:ApplyAura(AURA_INCAPACITATE, 1)
    affect:SaveDataString("cmd_look_cc", "|W(Blinded by " .. caster:GetName() .. ")|X") --used in cmd_look
  end
end
function skill_blind_remove(caster, target, affect)

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
  caster:Send("|WYour Sunder Armor hits " .. target:GetName() .. ".|X\r\n")
  target:Send("|W" .. caster:GetName() .. "'s Sunder Armor hits you.|X\r\n")
  caster:Message("|W" .. caster:GetName() .. "'s Sunder Armor hits " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, target)
  target:AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Sunder Armor", SPELLAFFECT_VISIBLE, 5, 0, 30, AFFECT_NONE, skill, "Armor decreased by " .. math.ceil(caster:GetStrength() * 0.30) .. " per stack")
  target:UpdateThreat(caster, threat, THREAT_OTHER)
end
function skill_sunder_armor_apply(caster, target, affect)
  local mod = 0
  if(affect:HasAura(AURA_MODIFY_ARMOR)) then
    mod = affect:GetAuraModifier(AURA_MODIFY_ARMOR)
  end
  affect:RemoveAura(AURA_MODIFY_ARMOR)
  affect:ApplyAura(AURA_MODIFY_ARMOR, mod - math.ceil(caster:GetStrength() * 0.30))
end
function skill_sunder_armor_remove(caster, target, affect)
  if(caster ~= nil) then
    caster:Send("|WYour Sunder Armor fades from " .. target:GetName() .. ".|X\r\n")
  end
  target:Send("|WSunder Armor fades from you.|X\r\n")
end
function skill_polymorph_4_cost(caster, target, skill)
  if(not caster:CanAttack(target)) then
    caster:Send("You can't attack that target.\n\r")
    return 0
  end
  if(not target:IsAlive()) then
    caster:Send("That target is already dead!\n\r")
    return 0
  end
  if(not caster:GetCharacterAdjacentRoom(target)) then
    caster:Send("That target is out of range.\n\r")
    return 0
  end
  if(not caster:HasResource(RESOURCE_MANA, 150)) then
    caster:Send("You don't have enough mana.\n\r")
    return 0
  end
  if(target:IsTapped() and not caster:HasTap(target)) then
    caster:Send("That target is already tapped.\r\n")
    return 0
  end
  return 1
end
function skill_polymorph_4_cast(caster, target, skill)
  caster:ConsumeMana(150)
  --if caster has spell affect mypolymorphtarget, find that affect's target (via affect caster field) and unpolymorph them
  local polytarget_affect = caster:HasSpellAffect("MyPolymorphTarget")
  if(polytarget_affect) then
    local polytarget_ch = polytarget_affect:GetCaster()
    caster:RemoveSpellAffect(true, "MyPolymorphTarget")
    if(polytarget_ch) then
        polytarget_ch:RemoveSpellAffect(true, "Polymorph")
    end
  end
  target:AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Polymorph", SPELLAFFECT_VISIBLE, 1, 25, 50, AFFECT_MAGIC, skill, "Incapacitated. Cannot attack or cast spells. Increased health regeneration.")
  caster:AddSpellAffect(SPELLAFFECT_DEBUFF, target, "MyPolymorphTarget", SPELLAFFECT_HIDDEN, 1, 0, 50, AFFECT_NONE, skill, target:GetName() .. " is my Polymorph target")
  caster:EnterCombat(target)
  target:EnterCombat(caster)
  if(caster:GetTarget() == target) then
    caster:CancelAutoAttack()
  end
  target:CancelAutoAttack()
  caster:Send("|W" .. target:GetName() .. " is affected by your Polymorph.|X\r\n")
  target:Send("|WYou are affected by " ..  caster:GetName() .. "'s Polymorph.|X\r\n")
  caster:Message("|W" .. target:GetName() .. " is affected by " .. caster:GetName() .. "'s Polymorph.|X", MSG_ROOM_NOTCHARVICT, target)
  if(caster:GetRoom() ~= target:GetRoom()) then
    target:Message("|W" .. target:GetName() .. " is affected by " .. caster:GetName() .. "'s Polymorph.|X", MSG_ROOM_NOTCHARVICT, caster)
  end
end
function skill_polymorph_4_apply(caster, target, affect)
  if(affect.name == "Polymorph") then
    affect:ApplyAura(AURA_INCAPACITATE, 1)
    affect:SaveDataString("cmd_look_cc", "|W(Polymorphed by " .. caster:GetName() .. ")|X") --used in cmd_look
  end
end
function skill_polymorph_4_tick(caster, target, affect)
  if(affect.name == "Polymorph") then
    target:AdjustHealth(target, target:GetMaxHealth())
  end
end
function skill_polymorph_4_remove(caster, target, affect)
  if(affect.name == "Polymorph") then
    target:Send("|WPolymorph fades from you.|X\r\n")
    if(caster ~= nil) then
      caster:Send("|WYour Polymorph fades from " .. target:GetName() .. ".|X\r\n")
      target:Message("|WPolymorph fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, caster)
    else
      target:Message("|WPolymorph fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHAR, nil)
    end
  end
  if(affect.name == "MyPolymorphTarget") then

  end
end
