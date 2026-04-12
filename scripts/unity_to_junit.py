import subprocess
import sys
import os

REPORTS = "reports"
UNITY_OUT = os.path.join(REPORTS, "unity_output.txt")
JUNIT_OUT = os.path.join(REPORTS, "junit.xml")

def main():
    # Run tests
    subprocess.run(["build/test_core.exe"], stdout=open(UNITY_OUT, "w"))

    # Convert to JUnit
    subprocess.run([
        "python", "scripts/unity_to_junit.py",
        UNITY_OUT,
        JUNIT_OUT
    ])

if __name__ == "__main__":
    main()
