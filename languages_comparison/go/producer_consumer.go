package main

import (
	"fmt"          // needed for printing
	"sync"         // needed for Mutex
	"sync/atomic"  // needed for atomic variable
	"time"         // needed for time measuring
)
// global variables
var (
	// mutex for shared resource access synchronization
	mtx sync.Mutex
	// allows to wait for all active go routines before main thead exits
	wait_group sync.WaitGroup
	// atomic variable to share a flag between the threads
	is_finished atomic.Bool
)
// interface class
type WorkerInterface interface {
	Run()
}
// template type definition
type Producer[T any] struct {
	// public members
	source_data [3]T
	shared_container *[]T
}
// class constructor and members initialization
func NewProducer[T any](source_data [3]T, shared_container *[]T) Producer[T] {
	// if statement
	if shared_container == nil {
		// error reporting by exception-like panic
		panic("Given shared_container is nil!")
	}
	return Producer[T]{source_data: source_data, shared_container: shared_container}
}

func (producer Producer[T]) Run() {
	// indexed for loop
	for i := 0; i < 10000; i++ {
		// range based for loop
		for _, element := range producer.source_data {
			// print statement
			fmt.Println("Putting", element, "to the shared container")
			// blocking reading and writing from/to the shared container
			mtx.Lock()
			// appending element to a variable-length array
			*producer.shared_container = append(*producer.shared_container, element)
			mtx.Unlock()
		}
	}
	is_finished.Store(true)
	fmt.Println("Producer done")
	wait_group.Done()
}

type Consumer[T any] struct {
	id uint
	last_size int
	shared_container *[]T
}

func NewConsumer[T any](id uint, shared_container *[]T) Consumer[T] {
	if shared_container == nil {
		panic("Given shared_container is nil!")
	}
	return Consumer[T]{id: id, shared_container: shared_container}
}

func (consumer Consumer[T]) Run() {
	// infinite loop
	for {
		// waiting for the input from another thread
		if (!(len(*consumer.shared_container) > consumer.last_size || is_finished.Load())) {
			continue;
		}
		if (is_finished.Load()) {
			break;
		}
		mtx.Lock()
		fmt.Println("Consumer", consumer.id, "noticed new element:", (*consumer.shared_container)[len(*consumer.shared_container)-1])
		consumer.last_size = len(*consumer.shared_container)
		mtx.Unlock()
	}

	fmt.Println("Consumer", consumer.id, "done")
	wait_group.Done()
}
// enum definition
type WorkerType int
const (
	kProducer WorkerType = iota
	kConsumer
)
// template function
func CreateWorker(worker_type WorkerType,
				  source_data [3]string,
				  shared_container *[]string,
				  id uint) WorkerInterface {
	if worker_type == kProducer {
		return NewProducer[string](source_data, shared_container)
	} else if worker_type == kConsumer {
		return NewConsumer[string](id, shared_container)
	} else {
		panic(fmt.Sprintf("Unsupported worker type: %d", worker_type))
	}
}
// application entry point
func main() {
	// error handling
	defer func() {
        if err := recover(); err != nil {
            fmt.Println("Error:", err)
        }
    }()
	// start measuring execution time
	start := time.Now()
	// measure execution time and display the result at the end
	defer func() {
		duration := time.Since(start)
		fmt.Println("Time elapsed:", duration)
	}()
	// initialize is_finished global atomic variable
	is_finished.Store(false)
	// fixed-length array
	source_data := [...]string {"Hello", "world", "!!!"}
	// variable-length array
	message_container := make([]string, 0)
	shared_container := &message_container
	// creation of the polymorphic types
	producer := CreateWorker(kProducer, source_data, shared_container, 0)
	first_consumer := CreateWorker(kConsumer, source_data, shared_container, 0)
	second_consumer := CreateWorker(kConsumer, source_data, shared_container, 1)

	wait_group.Add(3)
	// start 3 threads
	go producer.Run()
	go first_consumer.Run()
	go second_consumer.Run()
	// wait for threads to finish
	wait_group.Wait()
}
