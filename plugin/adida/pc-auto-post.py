# -*- coding: utf-8 -*-

# https://github.com/vpand/A64Dbg/blob/master/python3/adpdef.py
# 引入adp基础定义，比如error/event定义
from adpdef import *

# https://github.com/vpand/A64Dbg/blob/master/python3/adp.py
# 引入adp所有python api
from adp import *

import socket
import struct

dopost = False

udpsock = None
lastregs = None
lastpage = 0
lastbase = 0

def log_msg(text):
    if 0:
        print(text)

def post_pc_rva(pc):
    global lastpage
    global lastbase
    if not dopost:
        return
    # 使用当前页地址与历史页地址进行比较，减少模块获取次数
    curpage = pc & ~(0x1000 - 1)
    if lastpage != curpage:
        lastpage = curpage
        module = addrmod(pc)
        log_msg('Sync module %s' % str(module))
        lastbase = module['start']
    # 发送当前PC的RVA值
    rvabuf = struct.pack('@I', pc - lastbase)
    log_msg('Post pc %lx' % pc)
    udpsock.sendto(rvabuf, ('127.0.0.1', 20232))

# a64dbg插件入口
def adp_on_event(args):
    global dopost
    global udpsock
    global lastregs
    # 获取当前的事件码
    event = args[adp_inkey_type]
    # 用户执行了插件主菜单命令
    if event == adp_event_main_menu:
        dopost = not dopost
        flag = 'off'
        if dopost:
            flag = 'on'
        print('Turn %s pc auto post plugin.' % (flag))
    # 调试启动初始化完成
    elif event == adp_event_debug_initialized:
        udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        arch = curArch()
        if arch == adp_arch_arm:
            lastregs = arm
        elif arch == adp_arch_arm64:
            lastregs = arm64
        elif arch == adp_arch_x86:
            lastregs = x86
        else:
            lastregs = x64
        log_msg('Create udp socket %s, curregs %s' % (udpsock, lastregs))
        return success()
    # 暂停执行
    elif event == adp_event_debug_paused:
        post_pc_rva(lastregs.pc)
        return success()
    # 结束调试
    elif event == adp_event_debug_terminated:
        udpsock.close()
        log_msg('Closed udp socket')
        return success()
    # 插件框架向插件询问主菜单名称
    elif event == adp_event_menuname:
        return success('pc-auto-post')
    # 插件框架向插件询问插件版本和简介
    elif event == adp_event_adpinfo:
        return success(('0.1.0', "使用UDP协议自动将当前PC RVA值发送出去。"))
    return failed(adp_err_unimpl)
