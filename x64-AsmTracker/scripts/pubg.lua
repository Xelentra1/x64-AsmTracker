if(not FileLoaded()) then LoadFile('E:\\Steam\\steamapps\\common\\PUBG\\TslGame\\Binaries\\Win64\\TslGame_dump 24-07.exe') end

local r = DoScan('48 8B 0D ?? ?? ?? ?? 48 85 C9 74 1E 48 8B 01 FF 90')
Log(string.format('#define ENGINE_OFFSET 0x%x',GetRelativeAddress(GetBase()+r)))

local r = 0x339f74E--DoScans('48 83 C7 08 49 83 EE 01 0F 85 ?? FF FF FF')[3]+0x12
--Log(string.format('Scan Result:%x',r))
SetRva(r)
StepOver(16)