if(not FileLoaded()) then CreateProcess('C:\\Games\\Call of Duty Modern Warfare\\ModernWarfare.exe') Sleep(2000) local p = FindProcess('modernwarfare.exe') if p > 0 then Attach(p) Log('pid '..p) else Log('bad pid') return end
else ResetDbg() end
local r = DoScan('0F B6 4C 24 40 48 C1 C9 0C 83 E1 0F 48 83 F9 0E')
local iSkip = 0
Decode(GetBase()+r,function(rip,it)
	iSkip=iSkip+1 r = rip return iSkip<10
end)
Log(string.format('Scan Result: %x',r))
SetRva(r-GetBase())
local tc = GetTickCount()
SetAlias(RAX,'ret')
StepOver(220)
Log('tc: '..(GetTickCount()-tc)/1000)
local c = GetContext()
Log(string.format('Hi! %x',c.rip))
tc = GetTickCount()
local t = Track(RAX)
Log('tc2: '..(GetTickCount()-tc)/1000)
Log(string.format('RAX Track! %x',t.rva))
Log('Dump {') Log(t:dump()) Log('}')