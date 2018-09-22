function skill_renew_1_cost(caster, target, skill)
  if(not target:IsAlive()) then
    caster:Send("That target is already dead!\n\r")
    return 0
  end
  if(caster:IsFighting(target)) then
    caster:Send("You can't heal the target you're attacking.\n\r")
    return 0
  end
  if(not caster:GetCharacterAdjacentRoom(target)) then
	caster:Send("That target is out of range.\n\r")
	return 0
  end
  if(not caster:HasResource(RESOURCE_MANA, 30)) then
    caster:Send("You don't have enough mana.\n\r")
    return 0
  end
  return 1
end

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

function skill_power_word_shield_cost(caster, target, skill)
	if(not target:IsAlive()) then
		caster:Send("That target is already dead!\n\r")
		return 0
	end
	if(caster:IsFighting(target)) then
		caster:Send("You can't shield the target you're attacking.\n\r")
		return 0
	end
	if(not caster:GetCharacterAdjacentRoom(target)) then
		caster:Send("That target is out of range.\n\r")
		return 0
	end
	if(not caster:HasResource(RESOURCE_MANA, 400)) then
		caster:Send("You don't have enough mana.\n\r")
		return 0
	end
	if(target:HasSpellAffect("Weakened Soul")) then
	  caster:Send("You can't shield that target again yet.\n\r")
	  return 0
	end
	return 1
end

function skill_power_word_shield_cast(caster, target, sk)
  caster:ConsumeMana(400)
  local absorbvalue = math.ceil(caster:GetIntellect() * 1.75)
  if(caster ~= target) then
      caster:Send("|WYou cast Power Word: Shield on " .. target:GetName() .. ".|X\n\r")
      target:Send("|W" .. caster:GetName() .. " casts Power Word: Shield on you.|X\n\r")
  else
      caster:Send("|WYou cast Power Word: Shield on yourself.|X\n\r")
  end
  caster:Message("|W" .. caster:GetName() .. " casts Power Word: Shield on " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, target)

  target:AddSpellAffect(0, caster, "Power Word: Shield", false, false, 0, 30, AFFECT_MAGIC, sk, "Absorbs " .. absorbvalue .. " Damage")
  target:AddSpellAffect(1, caster, "Weakened Soul", false, false, 0, 15, AFFECT_NONE, sk, "Cannot be affected by Power Word: Shield")
end

function skill_power_word_shield_apply(caster, target, affect)
  if(affect.name == "Power Word: Shield") then
	affect:ApplyAura(AURA_DAMAGE_ABSORB, math.ceil(caster:GetIntellect() * 1.75))
  end
  if(affect.name == "Weakened Soul") then
    target:Send("|WYou are affected by Weakened Soul.|X\n\r")
  end
end

function skill_power_word_shield_remove(caster, target, affect)
  --caster may be nil in tick and remove functions
  if(affect.name == "Power Word: Shield") then
	  if(affect:GetCasterName() == target:GetName()) then
		  target:Send("|WYour Power Word: Shield fades.|X\n\r")
	  else
		  if(caster ~= nil) then
			  caster:Send("|WYour Power Word: Shield fades from " .. target:GetName() .. ".|X\n\r")
		  end
		  target:Send("|W" .. affect:GetCasterName() .. "'s Power Word: Shield fades from you.|X\n\r")
	  end
	  target:Message("|W" .. affect:GetCasterName() .. "'s Power Word: Shield fades from " .. target:GetName() .. ".|X", MSG_ROOM_NOTCHARVICT, caster)
  end
  if(affect.name == "Weakened Soul") then
    target:Send("|WWeakened Soul fades from you.|X\n\r")
  end
end