# A64Dbg

### Description

An arm/arm64/x86/x86_x64 assembly level debugger for macOS/iOS/Linux/Android like OllyDbg & X64Dbg.

An arm/arm64/x86/x86_x64 virtualization debugger(uvmdbg) based on UraniumVM for macOS/iOS/Android.

Running desktop platform supports Windows/Linux/macOS.

The debugee platform supports macOS/Linux/iOS/Android.

||Local|Remote Android|Remote iOS|
|-|-|-|-|
|Windows|No|Yes|No|
|Intel Linux|Yes|Yes|No|
|ARM Linux|Yes|Yes|No|
|Intel macOS|Yes|Yes|Yes|
|ARM macOS|Yes|Yes|Yes|

 * GUI Runtime is based on [Qt](https://www.qt.io/); 
 * GUI Controls is based on [X64Dbg](https://github.com/vpand/X64Dbg/);
 * Debug Engine is based on [LLDB](http://lldb.llvm.org/);
 * UVMDbg Engine is developed by [VPAND](https://vpand.com/);
 * Assembler/Disassembler is based on [LLVM](http://llvm.org/);
 * Analyze Engine is developed by [VPAND](https://vpand.com/);
 * Script is based on [Python](https://www.python.org/);

 ```
macOS user: use the Preference menu to configurate the remote debugging.
Windows/Linux user: use the MainMenu/Options/Preference to configurate the remote debugging.

iOS user: only support the real iDevice IP configuration.
Android user: only support the 127.0.0.1 IP configuration.
```

Debugger Server:

 * iOS user: install [a64dbg-server.deb](https://github.com/vpand/A64Dbg/blob/master/a64dbg-server.deb) to iDevice
```
scp A64Dbg/a64dbg-server.deb root@ip:/tmp/
ssh root@ip dpkg -i --force-overwrite /tmp/a64dbg-server.deb
```
 * iOS uvmdbg user: install the extra [a64dbg-server.uvm.deb](https://github.com/vpand/A64Dbg/blob/master/a64dbg-server.uvm.deb) to iDevice
```
scp A64Dbg/a64dbg-server.uvm.deb root@ip:/tmp/
ssh root@ip dpkg -i --force-overwrite /tmp/a64dbg-server.uvm.deb
```
 * Android user: push [a64dbg-server-arm64](https://github.com/vpand/A64Dbg/tree/master/a64dbg-server-arm64) to Android Device
```
adb push A64Dbg/a64dbg-server-arm64 /data/local/tmp/
adb shell chmod -R 755 /data/local/tmp/a64dbg-server-arm64/
cd /data/local/tmp/a64dbg-server-arm64; ./a64dbg-server&
```
```
adb forward tcp:30333 tcp:30333
```
 * Android uvmdbg user: push the extra [a64dbg-server-arm64.uvm](https://github.com/vpand/A64Dbg/tree/master/a64dbg-server-arm64.uvm) to Android Device and turn off SELinux
```
adb push A64Dbg/a64dbg-server-arm64.uvm /data/local/tmp/
setenforce 0
```
 * On Android, if you want to launch APK using uvmdbg, you should have Magisk root, and install [Riru ADZygote](https://github.com/vpand/A64Dbg/tree/master/launch-apk)

### User Manual

For more details, see [A64Dbg User Manual](https://github.com/vpand/adusrmanual/).
