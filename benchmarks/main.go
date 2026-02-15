package main

import (
	"context"
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
		iterations = 1000
		timeout    = 10 * time.Second
		logFile    = "benchmark.log"

		pietPath = "./../Piet-interp/cmake-build-debug/Piet_interp.exe"
		wsPath   = "./../Whitespace-interp/cmake-build-debug/Whitespace_interp.exe"

		pietArg1 = "../Piet-interp/examples/fib.png"
		pietArg2 = "../Piet-interp/examples/Countdown.png"
		pietArg3 = "../Piet-interp/examples/p.png"
		pietArg4 = "../Piet-interp/examples/piet_factorial_big.png"

		wsArg1 = "../Whitespace-interp/examples/fibonacci.ws"
		wsArg2 = "../Whitespace-interp/examples/count.ws"
		wsArg3 = "../Whitespace-interp/examples/h.ws"
		wsArg4 = "../Whitespace-interp/examples/fact.ws"

		inputData = "14\n"
	)

	f, err := os.OpenFile(logFile, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatalf("Error: %v", err)
	}
	defer f.Close()

	multi := io.MultiWriter(os.Stdout, f)
	var finalResults []BenchResult

	runBench := func(name, exe, arg string, useInput bool) {
		fmt.Printf("\n%s test (%s):\n", name, arg)

		var totalDur, totalCPU time.Duration
		var maxMem int64
		var successCount int64

		for i := 0; i < iterations; i++ {
			ctx, cancel := context.WithTimeout(context.Background(), timeout)
			cmd := exec.CommandContext(ctx, exe, arg)
			if useInput {
				cmd.Stdin = strings.NewReader(inputData)
			}

			start := time.Now()
			err := cmd.Run()
			elapsed := time.Since(start)
			cancel()

			if err != nil {
				continue
			}

			state := cmd.ProcessState
			totalDur += elapsed
			totalCPU += state.UserTime() + state.SystemTime()
			successCount++

			currentMem := getMaxMemory(cmd)
			if currentMem > maxMem {
				maxMem = currentMem
			}
		}

		if successCount > 0 {
			avgDur := totalDur / time.Duration(successCount)
			avgCPU := totalCPU / time.Duration(successCount)

			finalResults = append(finalResults, BenchResult{
				Name:    name + " (" + arg + ")",
				AvgDur:  avgDur,
				CPUTime: avgCPU,
				MaxRAM:  maxMem,
			})
			fmt.Printf("Avg: %v | CPU: %v | Max RAM: %d KB (Success: %d/%d)\n", avgDur, avgCPU, maxMem, successCount, iterations)
		}
	}

	fmt.Printf("Benchmark start: %s\n", time.Now().Format("15:04:05"))
	runBench("Piet", pietPath, pietArg1, false)
	runBench("Whitespace", wsPath, wsArg1, true)
	runBench("Piet", pietPath, pietArg2, false)
	runBench("Whitespace", wsPath, wsArg2, false)
	runBench("Piet", pietPath, pietArg3, false)
	runBench("Whitespace", wsPath, wsArg3, false)
	runBench("Piet", pietPath, pietArg4, true)
	runBench("Whitespace", wsPath, wsArg4, true)

	resume(multi, finalResults)
	fmt.Printf("\nBenchmark end: %s\n", time.Now().Format("15:04:05"))
}
