# Listing Network Adapters with C

The program lists the network adapters available in the device.


## Execute

On windows within developer powershell:
```ps1
cl list_adapters_win.c
list_adapters_win.exe
```

On windows with MinGW compiler:
```ps1
gcc list_adapters_win.c -o list_adapters_win.exe -lws2_32  -liphlpapi
list_adapters.exe
```
