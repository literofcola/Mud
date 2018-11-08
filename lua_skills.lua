function item_burst_of_knowledge_apply(caster, target, affect)
  caster:Send("|WYou are affected by Burst of Knowledge.|X\r\n");
  caster:ApplyAura(AURA_MANA_COST, -100)
end