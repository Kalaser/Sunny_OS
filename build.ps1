param(
    [string]$Generator = "MinGW Makefiles",
    [int]$Jobs = 10
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $root "Simulator\\build"

cmake -S (Join-Path $root "Simulator") -B $buildDir -G $Generator
cmake --build $buildDir -j $Jobs

