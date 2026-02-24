//go:build windows

package main

import (
	"os/exec"
	"syscall"
	"unsafe"
)

var (
	modkernel32              = syscall.NewLazyDLL("kernel32.dll")
	modpsapi                 = syscall.NewLazyDLL("psapi.dll")
	procGetProcessMemoryInfo = modpsapi.NewProc("GetProcessMemoryInfo")
	procOpenProcess          = modkernel32.NewProc("OpenProcess")
	procCloseHandle          = modkernel32.NewProc("CloseHandle")
)

const (
	PROCESS_QUERY_INFORMATION = 0x0400
	PROCESS_VM_READ           = 0x0010
)

type PROCESS_MEMORY_COUNTERS struct {
	CB                         uint32
	PageFaultCount             uint32
	PeakWorkingSetSize         uintptr
	WorkingSetSize             uintptr
	QuotaPeakPagedPoolUsage    uintptr
	QuotaPagedPoolUsage        uintptr
	QuotaPeakNonPagedPoolUsage uintptr
	QuotaNonPagedPoolUsage     uintptr
	PagefileUsage              uintptr
	PeakPagefileUsage          uintptr
}

func getMaxMemory(cmd *exec.Cmd) int64 {
	if cmd.Process == nil {
		return 0
	}

	h, _, _ := procOpenProcess.Call(
		uintptr(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ),
		0,
		uintptr(cmd.Process.Pid),
	)
	if h == 0 {
		return 0
	}
	defer procCloseHandle.Call(h)

	var mem PROCESS_MEMORY_COUNTERS
	mem.CB = uint32(unsafe.Sizeof(mem))

	ret, _, _ := procGetProcessMemoryInfo.Call(
		h,
		uintptr(unsafe.Pointer(&mem)),
		uintptr(mem.CB),
	)

	if ret == 0 {
		return 0
	}

	return int64(mem.PeakWorkingSetSize / 1024)
}
