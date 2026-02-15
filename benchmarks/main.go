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

func resume(w io.Writer, pietRes []time.Duration, wsRes []time.Duration) {
	fmt.Fprintln(w)
	fmt.Fprintln(w, "Resume:")
	fmt.Fprintf(w, "%-12s", "Whitespace: ")
	for i := range len(wsRes) {
		fmt.Fprintf(w, "%.10v\t", wsRes[i])
	}
	fmt.Fprintln(w)

	fmt.Fprintf(w, "%-12s", "Piet: ")
	for i := range pietRes {
		fmt.Fprintf(w, "%.10v\t", pietRes[i])
	}
	fmt.Fprintln(w)
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

	wsSum := []time.Duration{}
	pietSum := []time.Duration{}

	runBench := func(name, exe, arg string, useInput bool) {
		fmt.Printf("\n%s test:\n", name)

		var total time.Duration
		results := make([]time.Duration, 0, iterations)

		for i := range iterations {
			cmd := exec.Command(exe, arg)
			if useInput {
				cmd.Stdin = strings.NewReader(inputData)
			}

			start := time.Now()
			if err := cmd.Run(); err != nil {
				log.Fatalf("Error at %d: %v", i, err)
			}
			elapsed := time.Since(start)

			total += elapsed
			if verbose {
				results = append(results, elapsed)
			}
		}

		avg := total / time.Duration(iterations)

		if verbose {
			fmt.Printf("Full measurements: %v\n", results)
		}
		switch name {
		case "Whitespace":
			wsSum = append(wsSum, avg)
		case "Piet":
			pietSum = append(pietSum, avg)
		}
		fmt.Printf("Avg: %v\n", avg)
	}

	fmt.Printf("Benchmark start: %s\n", time.Now().Format("15:04:05"))
	runBench("Piet", pietPath, pietArg1, false)
	runBench("Whitespace", wsPath, wsArg1, true)
	runBench("Piet", pietPath, pietArg2, false)
	runBench("Whitespace", wsPath, wsArg2, false)
	resume(multi, pietSum, wsSum)
	fmt.Printf("\nBenchmark end: %s\n", time.Now().Format("15:04:05"))
}
