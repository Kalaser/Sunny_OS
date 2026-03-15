# Minimal Makefile wrapper around the existing CMake build.
# Works best with GNU Make on Windows (MSYS2/MinGW) or on Linux/macOS hosts.
#
# Examples:
#   make configure
#   make build
#   make run
#   make clean
#
# Override generator if needed:
#   make GEN="Ninja" build

GEN ?= MinGW Makefiles
BUILD_DIR ?= Simulator/build
JOBS ?= 10

.PHONY: help configure build run clean distclean

help:
	@echo Targets: configure build run clean distclean
	@echo Vars: GEN="$(GEN)" BUILD_DIR="$(BUILD_DIR)" JOBS="$(JOBS)"

configure:
	cmake -S Simulator -B "$(BUILD_DIR)" -G "$(GEN)"

build: configure
	cmake --build "$(BUILD_DIR)" -j "$(JOBS)"

run: build
	cd "$(BUILD_DIR)" && ./SunnyOsSimulator.exe

clean:
	cmake --build "$(BUILD_DIR)" --target clean

distclean:
	powershell -NoProfile -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"
