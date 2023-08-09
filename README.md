# win_port_scan
This repository contains two versions of a minimal port scanner for *Windows*:

- A single threaded version, just for testing purposes.
- A multi-threaded version, more efficient and with more debug information.

Usage of the multi-threaded version:
```
.\mt_port_scan.exe
Usage: .\mt_port_scan.exe start_port end_port ip num_threads
```

The scanner uses the standard *winsock* interface, therefore it implements a *Connect* scan and should work on all *Windows* versions, even the oldest ones. Please open an issue if you find a version for which it does not work. For *x64* architecture, you can test with the pre-compiled binaries, without even re-compiling them. For *x86*, I suggest to setup a *Windows 7 x86* machine and compile the code there.

> Q: Why use such a slow scanner?

> A: For me, it was useful during penetration test engagements, encompassing old *Windows 7* machines for which I wasn't able to make *nmap* (and similar) builds work, and I just needed a simple port scanner **which worked**. This is slow compared to *nmap*, but still faster than performing scans with *nmap* tunneling through multiple proxies. In fact, a *Connect* scan performed in local network with multiple threads is still faster than scanning with *proxychains* through multiple proxies. This scanner just works and is fast enough, being able to scan all 65535 ports of a host in local network in about 20 minutes, using 50 threads.