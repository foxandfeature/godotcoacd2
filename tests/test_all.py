import subprocess
from pathlib import Path
import threading

SCRIPT_DIR = Path(__file__).resolve().parent
WORKING_DIR = SCRIPT_DIR.parent

def run_command(command, cwd=None):
    process = subprocess.Popen(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        shell=isinstance(command, str)  # shell=True if command is a string
    )

    stdout_lines = []
    stderr_lines = []

    # Function to read a stream line by line in real-time
    def read_stream(stream, collector):
        for line in iter(stream.readline, ''):
            print(line, end="")
            collector.append(line)
        stream.close()

    # Start threads for stdout and stderr
    stdout_thread = threading.Thread(target=read_stream, args=(process.stdout, stdout_lines))
    stderr_thread = threading.Thread(target=read_stream, args=(process.stderr, stderr_lines))
    stdout_thread.start()
    stderr_thread.start()

    # Wait for process to finish
    process.wait()
    stdout_thread.join()
    stderr_thread.join()

    success = process.returncode == 0
    return success, "".join(stderr_lines)
  


def test_all(commands):
    passed = []
    failed = []

    for cmd in commands:
        run_command(["scons", "--clean"], cwd=WORKING_DIR)
        print(f"\n=== Running command: {cmd} ===")

        success, stderr = run_command(cmd)
        if success:
            passed.append(cmd)
        else:
            failed.append((cmd, stderr))

    print("\n===== TEST RESULTS =====")

    print(f"\n✅ Passed ({len(passed)}):")
    for cmd in passed:
        print(f"  - {cmd}")

    print(f"\n❌ Failed ({len(failed)}):")
    for cmd, error in failed:
        print(f"  - {cmd}")
        print(f"    Error: {error}")

    print("\n========================")

    return passed, failed