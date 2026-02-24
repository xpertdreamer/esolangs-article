//go:build !windows

package main

import (
	"os/exec"
	"runtime"
	"syscall"
)

func getMaxMemory(cmd *exec.Cmd) int64 {
	if usage, ok := cmd.ProcessState.SysUsage().(*syscall.Rusage); ok {
		mem := int64(usage.Maxrss)
		if runtime.GOOS == "darwin" {
			mem /= 1024
		}
		return mem
	}
	return 0
}
