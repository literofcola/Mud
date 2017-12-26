MSG_CHAR = 0;
MSG_CHAR_VICT = 1;
MSG_ROOM = 2;
MSG_ROOM_NOTCHAR = 3;
MSG_ROOM_NOTVICT = 4;
MSG_ROOM_NOTCHARVICT = 5;

function getlevel(character)
    return character:GetLevel();
end

function setlevel(character, level)
    character:SetLevel(level);
end

function add(first, second)
    return first + second
end
