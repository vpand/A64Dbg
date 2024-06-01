# -*- coding: utf-8 -*-

import threading
import socket
import struct
import traceback
import idaapi
import ida_nalt
import ida_kernwin
import idc

s = None
lastrva = 0
newrva = 0

def log_msg(text):
    if 0:
        print(text)

def gui_update():
    global lastrva
    global newrva
    try:
        idc.set_color(lastrva + ida_nalt.get_imagebase(), idc.CIC_ITEM, idc.DEFCOLOR)
        lastrva = newrva
        ea = lastrva + ida_nalt.get_imagebase()
        log_msg('Recv rva %x, new ea %lx' % (newrva, ea))
        idc.set_color(ea, idc.CIC_ITEM, 0x0000CC)
        ida_kernwin.jumpto(ea)
    except Exception as exc:
        log_msg(str(exc))
        traceback.print_exc()
    return False

def cursor_update():
    global s
    global lastrva
    global newrva
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('127.0.0.1', 20232))
    print("Running cursor auto update server...")
    while True:
        try:
            data, c_addr = s.recvfrom(4)
            newrva = struct.unpack('@I', data)[0]
            ida_kernwin.execute_ui_requests([gui_update])
        except Exception as exc:
            print(str(exc))
            traceback.print_exc()
            break
    print("Exited cursor auto updator")

class CursorUpdatePlugin(idaapi.plugin_t):
    flags = 0
    comment = "Auto update cursor from a64dbg incoming rva"
    help = comment
    wanted_name = "Cursor Auto Updator with A64Dbg"
    wanted_hotkey = ""
    
    def init(self):
        return idaapi.PLUGIN_OK 

    def run(self, arg):
        global s
        if s is None:
            print("%s plugin init" % self.comment)
            threading.Thread(target=cursor_update).start()

    def term(self):
        global s
        if s:
            print("%s plugin term" % self.comment)
            s.close()
            s = None

def PLUGIN_ENTRY():
    return CursorUpdatePlugin()
