import os
import sys
from time import sleep
import subprocess

def run_cmd(cmd: str) -> int:
    print(f"Running: {cmd}")
    process = subprocess.Popen(
        cmd, shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    # Print output in real time
    for line in process.stdout:
        print(line, end='')

    for line in process.stderr:
        print(line, end='')

    process.wait()
    return process.returncode


GDBSERVER_PATH = "/data/local/tmp/gdbserver-7.7.1-armhf-eabi5-v1-sysv"
PORT = 5555

run_cmd('adb shell su -c "pkill -f gdb*"')
#sleep(0.5)
run_cmd('adb shell su -c "pkill -f sakeloader*"')
#sleep(0.5)

gdbserver_starter = f'{GDBSERVER_PATH} 127.0.0.1:{PORT} /data/local/tmp/sakeloader/sakeloader debug' # --break main
run_cmd(f'adb shell su -c "{gdbserver_starter}"', background=True)
#sleep(0.5)
run_cmd(f'adb forward tcp:{PORT} tcp:{PORT}')
#sleep(0.5)


gdb_com = f'gdb-multiarch -q -ex "set architecture armv7" -ex "set debuginfod enable off" -ex "set pagination off" -ex "target remote localhost:{PORT}" -ex "continue" -ex "sharedlibrary" -ex "info sharedlibrary"'
print(gdb_com)

input("\n\n***PRESS ENTER TO ATTACH MANUALLY!*** \n\n")
os.system(gdb_com)