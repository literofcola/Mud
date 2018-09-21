function skill_slam_cost(caster, target, skill)
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
    if(not caster:HasResource(RESOURCE_RAGE, 20)) then
        caster:Send("You don't have enough rage.\n\r")
        return 0
    end
    return 1
  end
  

  function skill_slam_cast(caster, target, sk)
    caster:ConsumeRage(20)
    caster:EnterCombat(target)
    target:EnterCombat(caster)
    local damage = math.ceil(caster:GetMainhandDamagePerSecond() * 1.9 + (caster:GetStrength() * .3))
    local attack = caster:DoAttackRoll(target, SCHOOL_PHYSICAL)
    if(attack == ATTACK_MISS) then
      caster:Send("|WYour Slam misses " .. target:GetName() .. "|X\n\r")
      target:Send("|W" .. caster:GetName() .. "'s Slam misses you.|X\n\r")
      caster:Message("|W" .. caster:GetName() .. "'s Slam misses " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, target)
      return
    elseif (attack == ATTACK_DODGE) then
      caster:Send("|W" .. target:GetName() .. " dodges your Slam.|X\n\r")
      target:Send("|WYou dodge " .. caster:GetName() .. "'s Slam.|X\n\r")
      caster:Message("|W" .. target:GetName() .. " dodges " .. caster:GetName() .. "'s Slam.|X", MSG_ROOM_NOTCHARVICT, target)
      return
    elseif (attack == ATTACK_PARRY) then
      caster:Send("|W" .. target:GetName() .. " parries your Slam.|X\n\r")
      target:Send("|WYou parry " .. caster:GetName() .. "'s Slam.|X\n\r")
      caster:Message("|W" .. target:GetName() .. " parries " .. caster:GetName() .. "'s Slam.|X", MSG_ROOM_NOTCHARVICT, target)
      return
    elseif (attack == ATTACK_CRIT) then
      damage = math.ceil(damage * 1.5)
      damage = math.ceil(damage - (damage * target:CalculateArmorMitigation()))
      caster:Send("|WYour Slam CRITS " .. target:GetName() .. " for " .. damage .. " damage.|X\n\r")
      target:Send("|W" .. caster:GetName() .. "'s Slam CRITS you for " .. damage .. " damage.|X\n\r")
      caster:Message("|W" .. caster:GetName() .. "'s Slam CRITS " .. target:GetName() .. " for " .. damage .. " damage.|X", MSG_ROOM_NOTCHARVICT, target)
    else
      damage = math.ceil(damage - (damage * target:CalculateArmorMitigation()))
      caster:Send("|WYour Slam hits " .. target:GetName() .. " for " .. damage .. " damage.|X\n\r")
      target:Send("|W" .. caster:GetName() .. "'s Slam hits you for " .. damage .. " damage.|X\n\r")
      caster:Message("|W" .. caster:GetName() .. "'s Slam hits " .. target:GetName() .. " for " .. damage .. " damage.|X", MSG_ROOM_NOTCHARVICT, target)
    end
    caster:OneHit(target, damage)
  end