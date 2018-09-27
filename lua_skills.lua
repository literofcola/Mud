function item_basicfood_cost(caster, target, skill)
    if(caster:InCombat()) then
        caster:Send("You can't do that while in combat.\n\r")
        return 0
    end
    return 1
end
function item_basicfood_cast(caster, target, sk)
    local totalheal = math.ceil((20*caster:GetLevel())+50)
    local healpersecond = math.ceil(totalheal / 30)
    target:AddSpellAffect(0, caster, "Food", false, false, 10, 30, AFFECT_NONE, sk, "Restores " .. healpersecond .. " health per second.")
end
function item_basicfood_apply(caster, target, affect)
    local totalheal = math.ceil((20*caster:GetLevel())+50)
    affect:SaveDataInt("totalheal", totalheal)
    affect:ApplyAura(AURA_EATING, 1)

end
function item_basicfood_tick(caster, target, affect)
    if(target:GetHealth() >= target:GetMaxHealth()) then
      return
    end
    local totalheal = affect:GetDataInt("totalheal")
    local healpertick = math.ceil(totalheal / 10)
    if(target:GetHealth() > (target:GetMaxHealth() - healpertick)) then
      target:Send("|WYou gain " .. (target:GetMaxHealth() - healpertick) .. " health.|X\n\r")
    else
      target:Send("|WYou gain " .. healpertick .. " health.|X\n\r")
    end
    target:AdjustHealth(caster, healpertick)
end
function item_basicfood_remove(caster, target, affect)
    
end
function item_basicdrink_cost(caster, target, skill)
    if(caster:InCombat()) then
        caster:Send("You can't do that while in combat.\n\r")
        return 0
    end
    return 1
end
function item_basicdrink_cast(caster, target, sk)
    local totalmana = math.ceil((30*caster:GetLevel())+50)
    local manapersecond = math.ceil(totalmana / 30)
    target:AddSpellAffect(0, caster, "Drink", false, false, 10, 30, AFFECT_NONE, sk, "Restores " .. manapersecond .. " mana per second.")
end
function item_basicdrink_apply(caster, target, affect)
    local totalmana = math.ceil((30*caster:GetLevel())+50)
    affect:SaveDataInt("totalmana", totalmana)
    affect:ApplyAura(AURA_EATING, 1)
end
function item_basicdrink_tick(caster, target, affect)
    if(target:GetMana() >= target:GetMaxMana()) then
      return
    end
    local totalmana = affect:GetDataInt("totalmana")
    local manapertick = math.ceil(totalmana / 10)
    if(target:GetMana() > (target:GetMaxMana() - manapertick)) then
      target:Send("|WYou gain " .. (target:GetMaxMana() - manapertick) .. " mana.|X\n\r")
    else
      target:Send("|WYou gain " .. manapertick .. " mana.|X\n\r")
    end
    target:AdjustMana(caster, manapertick)
end
function item_basicdrink_remove(caster, target, affect)
    
end
