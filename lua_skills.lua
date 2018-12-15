function skill_summon_spawn_of_baelgar_cost(caster, target, skill)
  return 1
end
function skill_summon_spawn_of_baelgar_cast(caster, target, skill)
  Game:GetGame():LoadNPCRoom(34, caster:GetRoom())
  caster:Message("|WSpawn of Bael'Gar has arrived.|X", MSG_ROOM, nil)
  for index, ch in pairs(caster:GetRoom().characters) do
    if(ch:IsPlayer()) then
      local damage = 500
      local absorbed = ch:HandleDamageAbsorb(damage)
      damage = damage - absorbed
      ch:Message("|WThe summoning of " .. caster:GetName() .. "'s spawn hits " .. ch:GetName() .. " for " .. damage .. " damage.|X", MSG_ROOM_NOTCHARVICT, caster) 
      ch:Send("|WThe summoning of " .. caster:GetName() .. "'s spawn hits you for " .. damage .. " damage") 
      if(absorbed > 0) then
        ch:Send(" (" .. absorbed .. " absorbed)")
      end
      ch:Send(".|X\r\n")
      caster:EnterCombat(ch)
      ch:EnterCombat(caster)
      caster:OneHit(ch, damage)
    end
  end
end
function skill_magma_splash_cost(caster, target, skill)
  return 1
end
function skill_magma_splash_cast(caster, target, skill)
  caster:Message("|W" .. caster:GetName() .. " casts Magma Splash on " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHAR, nil)
  target:AddSpellAffect(SPELLAFFECT_BUFF, caster, "Magma Splash", SPELLAFFECT_VISIBLE, 1, 10, 30, AFFECT_MAGIC, skill, "Deals Fire damage to the current target and additional Fire damage every 3 sec for 30 sec")
  if(target:GetTarget() ~= nil) then
    local damage = 125
    local absorbed = target:GetTarget():HandleDamageAbsorb(damage)

    target:Send("|WYour Magma Splash hits " .. target:GetTarget():GetName() .. " for " .. (damage - absorbed) .. " damage")
    if(absorbed > 0) then
      target:Send(" (" .. absorbed .. " absorbed)")
    end
    target:Send(".|X\r\n")
    target:GetTarget():Send("|W" .. target:GetName() .. "'s Magma Splash hits you for " .. (damage - absorbed) .. " damage")
    if(absorbed > 0) then
      target:GetTarget():Send(" (" .. absorbed .. " absorbed)")
    end
    target:GetTarget():Send(".|X\r\n")

    target:Message("|W" .. caster:GetName() .. "'s Magma Splash hits " .. target:GetTarget():GetName() .. " for " .. (damage - absorbed) .. " damage.|X", MSG_ROOM_NOTCHARVICT, target:GetTarget())
    damage = damage - absorbed
    target:EnterCombat(target:GetTarget())
    target:GetTarget():EnterCombat(target)
    target:OneHit(target:GetTarget(), damage)
  end
end
function skill_magma_splash_apply(caster, target, affect)
  caster:Message("|W" .. caster:GetName() .. " is affected by Magma Splash.|X", MSG_ROOM_NOTCHAR, nil)
end
function skill_magma_splash_tick(caster, target, affect)
  if(target:GetTarget() == nil) then --This skill sits on the target and damages their target
    return
  end

  local damage = 100
  local absorbed = target:GetTarget():HandleDamageAbsorb(damage)
  damage = damage - absorbed
  target:GetTarget():Send("|W" .. affect:GetCasterName() .. "'s Magma Splash damages you for " .. damage .. " health") 
  if(absorbed > 0) then
    target:GetTarget():Send(" (" .. absorbed .. " absorbed)")
  end
  target:GetTarget():Send(".|X\r\n")

  target:GetTarget():Message("|W" .. affect:GetCasterName() .. "'s Magma Splash damages " .. target:GetTarget():GetName() .. " for " .. damage .. " health.|X", MSG_ROOM_NOTCHARVICT, target) 
  target:Send("|WYour Magma Splash damages " .. target:GetTarget():GetName() .. " for " .. damage .. " health") 
  if(absorbed > 0) then
    target:Send(" (" .. absorbed .. " absorbed)")
  end
  target:Send(".|X\r\n")
  target:OneHit(target:GetTarget(), damage) 
end
function skill_magma_splash_remove(caster, target, affect)
  if(caster ~= nil) then
    caster:Send("|WYour Magma Splash fades from " .. target:GetName() .. ".|X\n\r")
  end
  target:Send("|W" .. affect:GetCasterName() .. "'s Magma Splash fades from you.|X\n\r")
  target:Message("|W" .. affect:GetCasterName() .. "'s Magma Splash fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, caster)
end
