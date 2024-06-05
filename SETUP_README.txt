Inside launch.json:

"configFiles": [
    "interface/cmsis-dap.cfg",
    "target/rp2040.cfg"
],

also change GDB path:
            "gdbPath": "/usr/bin/gdb-multiarch",

gdb-multiarch can be installed from the software center