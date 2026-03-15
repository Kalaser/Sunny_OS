param(
    [string]$Generator = "MinGW Makefiles",
    [int]$Jobs = 10,
    [switch]$Detach
)

$ErrorActionPreference = "Stop"

& (Join-Path $PSScriptRoot "build.ps1") -Generator $Generator -Jobs $Jobs

$buildDir = Join-Path $PSScriptRoot "Simulator\\build"
$exe = Join-Path $buildDir "SunnyOsSimulator.exe"

if ($Detach) {
    Start-Process -FilePath $exe -WorkingDirectory $buildDir | Out-Null
    return
}

Push-Location $buildDir
try {
    & $exe
} finally {
    Pop-Location
}
