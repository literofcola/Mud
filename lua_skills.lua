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
  target:AddSpellAffect(1, caster, "Polymorph", false, false, 25, 50, AFFECT_MAGIC, skill, "Incapacitated. Cannot attack or cast spells. Increased health regeneration.")
  caster:AddSpellAffect(1, target, "MyPolymorphTarget", true, false, 0, 50, AFFECT_NONE, skill, target:GetName() .. " is my Polymorph target")
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
