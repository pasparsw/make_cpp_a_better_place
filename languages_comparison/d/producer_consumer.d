import std.stdio;                          // needed for writeln
import std.datetime.stopwatch : StopWatch; // needed for time measurement
import core.atomic;                        // needed for atomic operations
import core.sync.mutex;                    // needed for Mutex
import core.thread;                        // needed for Thread
// mutex for shared resource access synchronization
shared Mutex mtx;
// flag shared between the threads
shared bool is_finished = false;
// initialization of global variables at runtime
shared static this() {
    mtx = new shared Mutex();
}
// interface class
interface WorkerInterface
{
    // public class members
    void Run() shared;
}
// templated alias for a type
alias SharedContainerPtr(T) = T[]*;
// template class definition + inheritance
class Producer(T) : WorkerInterface
{
    // class constructor and members initialization
    // together with passing arguments by const reference and non-const reference
    this(const T[3] source_data, SharedContainerPtr!T shared_container)
    {
        source_data_ = source_data;
        shared_container_ = shared_container;
        // if statement
        if (!shared_container_) {
            // error reporting by throwing an exception
            throw new Exception("Given shared_container is null!");
        }
    }
    override shared void Run()
    {
        // indexed for loop
        for (uint i=0U; i<10_000U; i++) {
            // range based for loop
            foreach (const ref element; source_data_) {
                // print statement
                writeln("Putting ", element, " into the shared container");
                // blocking reading and writing from/to the shared container
                synchronized (mtx) {
                    *shared_container_ ~= element;
                }
            }
        }
        is_finished.atomicStore(true);
        writeln("Producer done");
    }
// private class members
private:
    // fixed-length array with a generic type
    T[3] source_data_;
    SharedContainerPtr!T shared_container_;
}

class Consumer(T) : WorkerInterface
{
    this(const uint id, SharedContainerPtr!T shared_container)
    {
        id_ = id;
        last_size_ = shared_container.length;
        shared_container_ = shared_container;

        if (!shared_container_) {
            throw new Exception("Given shared_container is null!");
        }
    }
    override shared void Run()
    {
        // infinite while loop
        while (true) {
            // waiting for the input from another thread
            if (!(shared_container_.length > last_size_ || is_finished.atomicLoad())) {
                continue;
            }
            if (is_finished.atomicLoad()) {
                break;
            }
            // blocking writing to the shared container
            synchronized (mtx) {
                writeln("Consumer ", id_, " noticed new element: ", (*shared_container_)[$-1]);
                last_size_ = shared_container_.length;
            }
        }
        writeln("Consumer ", id_, " done");
    }

private:
    uint id_;
    size_t last_size_;
    SharedContainerPtr!T shared_container_;
}
// enum definition
enum WorkerType
{
    kProducer,
    kConsumer
}
// template function with a compile time argument and variadic arguments
WorkerInterface CreateWorker(WorkerType worker_type, Args...)(Args args)
{
    // compile time if statement excluding constructors of the classes to which
    // current arguments don't fit - an ordinary runtime if would cause a compilation error
    // because Producer's and Consumer's constructors have different signatures
    static if (worker_type == WorkerType.kProducer) {
        // dynamic memory allocation of the polymorphic type
        return new Producer!string(args);
    } else static if (worker_type == WorkerType.kConsumer) {
        return new Consumer!string(args);
    } else {
        throw new Exception("Unsupported worker type!");
    }
}
// application entry point
void main()
{
    // start measuring execution time
    auto sw = StopWatch();
    sw.start();
    try {
        // fixed-length array
        string[3] source_data = ["Hello", "world", "!!!"];
        // variable-length array
        string[] shared_container;
        // creation of the polymorphic types
        WorkerInterface producer = CreateWorker!(WorkerType.kProducer)(source_data, &shared_container);
        WorkerInterface first_consumer = CreateWorker!(WorkerType.kConsumer)(0, &shared_container);
        WorkerInterface second_consumer = CreateWorker!(WorkerType.kConsumer)(1, &shared_container);
        // start 3 threads
        auto producer_thread = new Thread(() => (cast(shared)producer).Run());
        auto first_consumer_thread = new Thread(() => (cast(shared)first_consumer).Run());
        auto second_consumer_thread = new Thread(() => (cast(shared)second_consumer).Run());

        producer_thread.start();
        first_consumer_thread.start();
        second_consumer_thread.start();

        producer_thread.join();
        first_consumer_thread.join();
        second_consumer_thread.join();
    }
    // error handling
    catch (Exception e) {
        writeln("Error: ", e.msg);
    }
    // measure execution time and display the result
    sw.stop();
    writeln("Time elapsed: ", sw.peek.total!"msecs", "ms");
}
