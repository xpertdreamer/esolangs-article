package main

import (
	"fmt"
	"log"
	"os/exec"
	"strings"
	"time"
)

func main() {
	const (
		verbose    = true
		iterations = 100

		pietPath = "./../Piet-interp/cmake-build-debug/Piet_interp.exe"
		wsPath   = "./../Whitespace-interp/cmake-build-debug/Whitespace_interp.exe"

		pietArg1 = "../Piet-interp/examples/fib.png"
		pietArg2 = "../Piet-interp/examples/Countdown.png"

		wsArg1 = "../Whitespace-interp/examples/fibonacci.ws"
		wsArg2 = "../Whitespace-interp/examples/count.ws"

		inputData = "14\n"
	)

	runBench := func(name, exe, arg string, useInput bool) {
		fmt.Printf("\n%s test:\n", name)

		var total time.Duration
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

			total += elapsed
			if verbose {
				results = append(results, elapsed)
			}
		}

		if verbose {
			fmt.Printf("Full measurements: %v\n", results)
		}
		fmt.Printf("Avg: %v\n", total/time.Duration(iterations))
	}

	fmt.Printf("Benchmark start: %s\n", time.Now().Format("15:04:05"))

	runBench("Piet", pietPath, pietArg1, false)
	runBench("Whitespace", wsPath, wsArg1, true)
	runBench("Piet", pietPath, pietArg2, false)
	runBench("Whitespace", wsPath, wsArg2, false)
}
