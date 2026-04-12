# =====================================================================
# Jenkins Agent Bootstrap Script (Windows)
# Installs: Git, MinGW-w64 (gcc/make), Python3, Cppcheck, lcov
# Ensures PATH is updated for SYSTEM account (Jenkins)
# =====================================================================

Write-Host "=== Jenkins Agent Bootstrap Script ==="

# ---------------------------------------------------------------------
# Helper: Install Chocolatey if missing
# ---------------------------------------------------------------------
if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Chocolatey..."
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = "tls12"
    Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
} else {
    Write-Host "Chocolatey already installed."
}

# ---------------------------------------------------------------------
# Install required tools
# ---------------------------------------------------------------------
$packages = @(
    "git",
    "mingw",
    "python",
    "cppcheck",
    "lcov"
)

foreach ($pkg in $packages) {
    Write-Host "Installing $pkg..."
    choco install $pkg -y --no-progress
}

# ---------------------------------------------------------------------
# Add MinGW, Python, and Cppcheck to PATH (SYSTEM environment)
# ---------------------------------------------------------------------
$pathsToAdd = @(
    "C:\Program Files\Cppcheck",
    "C:\Program Files\Python312",
    "C:\Program Files\Python312\Scripts",
    "C:\ProgramData\chocolatey\bin",
    "C:\mingw64\bin"
)

foreach ($p in $pathsToAdd) {
    if (-not ($env:Path -split ";" | Where-Object { $_ -eq $p })) {
        Write-Host "Adding to PATH: $p"
        setx /M PATH "$($env:Path);$p" | Out-Null
    }
}

# ---------------------------------------------------------------------
# Verify installations
# ---------------------------------------------------------------------
Write-Host "`n=== Verifying Tools ==="
$tools = @("git", "gcc", "make", "python", "cppcheck", "lcov")

foreach ($t in $tools) {
    Write-Host "Checking $t..."
    if (Get-Command $t -ErrorAction SilentlyContinue) {
        Write-Host "✔ $t found"
    } else {
        Write-Host "❌ $t NOT FOUND"
    }
}

# ---------------------------------------------------------------------
# Restart Jenkins service so PATH updates
# ---------------------------------------------------------------------
Write-Host "`nRestarting Jenkins service..."
Restart-Service jenkins

Write-Host "`n=== Bootstrap Complete ==="
Write-Host "Your Jenkins agent is now ready for CI builds."
