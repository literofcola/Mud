function skill_fire_storm_cost(caster, target, skill)
  return 1
end
function skill_fire_storm_cast(caster, target, skill)

    caster:Message("|W" .. caster:GetName() .. "'s Fire Storm hits " .. target:GetName() .. " for " .. damage .. " damage.|X", MSG_ROOM_NOTCHARVICT, target)
    target:Message("|W" .. caster:GetName() .. " casts Fire Storm on you.|X", MSG_CHAR, nil)

    if(caster:GetRoom() ~= target:GetRoom()) then
    target:Message("|W" .. caster:GetName() .. " casts Fire Storm on " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHAR, nil)
    end

    target:GetRoom():AddSpellAffect(SPELLAFFECT_DEBUFF, caster, "Fire Storm", SPELLAFFECT_VISIBLE, 1, 10, 30, AFFECT_MAGIC, skill, "Deals Fire damage to the target and additional Fire damage to everyone in the room every 3 sec for 30 sec")
  
    local damage = 324
    local absorbed = target:HandleDamageAbsorb(damage)

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
function skill_fire_storm_apply(caster, room, affect)
  target:Message("|WThe room is engulfed by a Fire Storm!|X", MSG_ROOM, nil)
end
function skill_fire_storm_tick(caster, room, affect)

end
function skill_fire_storm_remove(caster, room, affect)

end
