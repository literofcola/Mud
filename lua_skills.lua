function skill_charge_cost(caster, target, skill)
  if(not caster:CanAttack(target)) then
    caster:Send("You can't attack that target.\n\r")
    return 0
  end
  if(not target:IsAlive()) then
    caster:Send("That target is already dead!\n\r")
    return 0
  end
  if(caster:GetCharacterRoom(target)) then
    caster:Send("That target is too close!\n\r")
    return 0
  end
  if(not caster:GetCharacterAdjacentRoom(target)) then
    caster:Send("That target is out of range.\n\r")
    return 0
  end
end

function skill_charge_cast(caster, target, skill)
  --find which direction target is
  --print charging cast/exit messages
  --move
  --print charge enter/stun messages
  target:CancelAutoAttack()
  target:AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Charge Stun", SPELLAFFECT_VISIBLE, 1, 0, 2, AFFECT_NONE, skill, "Stunned")
  caster:EnterCombat(target)
  target:EnterCombat(caster)
  caster:AdjustRage(caster, 10)
end

function skill_charge_apply(caster, target, affect)
  affect:ApplyAura(AURA_STUN, 1)
end

function skill_charge_remove(caster, target, affect)
  if(affect.name == "Charge Stun") then
    target:Send("|WCharge Stun fades from you.|X\r\n")
    if(caster ~= nil) then
      caster:Send("|WYour Charge Stun fades from " .. target:GetName() .. ".|X\r\n")
      target:Message("|WCharge Stun fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, caster)
    else
      target:Message("|WCharge Stun fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHAR, nil)
    end
  end
end