package main

import (
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"strings"
	"time"
)

type BenchResult struct {
	Name    string
	AvgDur  time.Duration
	CPUTime time.Duration
	MaxRAM  int64
}

func resume(w io.Writer, results []BenchResult) {
	fmt.Fprintln(w)
	fmt.Fprintln(w, "Resume:")
	fmt.Fprintf(w, "%-15s | %-15s | %-15s | %-10s\n", "Language", "Avg Duration", "CPU Time", "Max RAM")
	fmt.Fprintln(w, strings.Repeat("-", 65))
	for i, r := range results {
		fmt.Fprintf(w, "%d %.13s | %-15.10v | %-15.10v | %d KB\n",
			i, r.Name, r.AvgDur, r.CPUTime, r.MaxRAM)
	}
}

func main() {
	const (
		verbose    = true
		iterations = 100
		logFile    = "benchmark.log"

		pietPath = "./../Piet-interp/cmake-build-debug/Piet_interp.exe"
		wsPath   = "./../Whitespace-interp/cmake-build-debug/Whitespace_interp.exe"

		pietArg1 = "../Piet-interp/examples/fib.png"
		pietArg2 = "../Piet-interp/examples/Countdown.png"

		wsArg1 = "../Whitespace-interp/examples/fibonacci.ws"
		wsArg2 = "../Whitespace-interp/examples/count.ws"

		inputData = "14\n"
	)

	f, err := os.OpenFile(logFile, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatalf("Error opening log file: %v", err)
	}
	defer f.Close()

	multi := io.MultiWriter(os.Stdout, f)
	var finalResults []BenchResult

	runBench := func(name, exe, arg string, useInput bool) {
		fmt.Printf("\n%s test (%s):\n", name, arg)

		var totalDur, totalCPU time.Duration
		var maxMem int64
		results := make([]time.Duration, 0, iterations)

		for i := 0; i < iterations; i++ {
			cmd := exec.Command(exe, arg)
			if useInput {
				cmd.Stdin = strings.NewReader(inputData)
			}

			start := time.Now()
			if err := cmd.Run(); err != nil {
				log.Fatalf("Error at %d: %v", i, err)
			}
			elapsed := time.Since(start)

			state := cmd.ProcessState
			totalDur += elapsed
			totalCPU += state.UserTime() + state.SystemTime()

			currentMem := getMaxMemory(cmd)
			if currentMem > maxMem {
				maxMem = currentMem
			}

			if verbose {
				results = append(results, elapsed)
			}
		}

		avgDur := totalDur / time.Duration(iterations)
		avgCPU := totalCPU / time.Duration(iterations)

		finalResults = append(finalResults, BenchResult{
			Name:    name + " (" + arg + ")",
			AvgDur:  avgDur,
			CPUTime: avgCPU,
			MaxRAM:  maxMem,
		})
		fmt.Printf("Avg: %v | CPU: %v | Max RAM: %d KB\n", avgDur, avgCPU, maxMem)
	}

	fmt.Printf("Benchmark start: %s\n", time.Now().Format("15:04:05"))
	runBench("Piet", pietPath, pietArg1, false)
	runBench("Whitespace", wsPath, wsArg1, true)
	runBench("Piet", pietPath, pietArg2, false)
	runBench("Whitespace", wsPath, wsArg2, false)

	resume(multi, finalResults)
	fmt.Printf("\nBenchmark end: %s\n", time.Now().Format("15:04:05"))
}
