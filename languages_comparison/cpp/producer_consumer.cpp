#include <iostream>            // needed for std::cout
#include <memory>              // needed for std::shared_ptr
#include <shared_mutex>        // needed for std::shared_mutex
#include <chrono>              // needed for time measurement
#include <mutex>               // needed for std::unique_lock
#include <vector>              // needed for std::vector
#include <condition_variable>  // needed for std::condition_variable_any
#include <array>               // needed for std::array
#include <future>              // needed for std::async
#include <atomic>              // needed for std::atomic
// mutex for shared resource access synchronization
static std::shared_mutex mtx;
// condition variable for threads execution flow synchronization
static std::condition_variable_any cv;
// atomic variable to share a flag between the threads
static std::atomic<bool> is_finished = false;
// interface class
class WorkerInterface
{
// public class members
public:
    virtual ~WorkerInterface() = default;
    virtual void Run() = 0;
};
// alias for a type
using WorkerPtr = std::shared_ptr<WorkerInterface>;
// templated alias for a type
template <typename T>
using SharedContainerPtr = std::shared_ptr<std::vector<T>>;
// template class definition + inheritance
template <typename T>
class Producer : public WorkerInterface
{
public:
    // class constructor and members initialization
    // together with passing arguments by const reference and non-const reference
    Producer(const std::array<T, 3> &source_data, SharedContainerPtr<T> shared_container)
    : source_data_{source_data},
      shared_container_{shared_container}
    {
        // if statement
        if (!shared_container_) {
            // error reporting by throwing an exception
            throw std::runtime_error("Given shared_container is a nullptr!");
        }
    }
    void Run() override
    {
        // indexed for loop
        for (unsigned int i=0U; i<10000U; i++) {
            // range based for loop
            for (const auto &element : source_data_) {
                // print statement
                std::cout << "Putting " << element << " to the shared container" << std::endl;
                {
                    // blocking reading and writing from/to the shared container
                    std::unique_lock<std::shared_mutex> lock(mtx);
                    // appending element to a variable-length array
                    shared_container_->push_back(element);
                }
                // unblocking all waiting threads
                cv.notify_all();
            }
        }
        is_finished = true;
        cv.notify_all();
        std::cout << "Producer done" << std::endl;
    }
// private class members
private:
    // fixed-length array with a generic type
    std::array<T, 3> source_data_;
    SharedContainerPtr<T> shared_container_;
};

template<typename T>
class Consumer : public WorkerInterface
{
public:
    Consumer(const unsigned int id, SharedContainerPtr<T> shared_container)
    : id_{id},
      last_size_{shared_container->size()},
      shared_container_{shared_container}
    {
        if (!shared_container_) {
            throw std::runtime_error("Given shared_container is a nullptr!");
        }
    }
    void Run() override
    {
        // infinite while loop
        while (true) {
            // blocking writing to the shared container
            std::shared_lock<std::shared_mutex> lock(mtx);
            // waiting for the input from another thread
            cv.wait(lock, [this]{ return shared_container_->size() > last_size_ || is_finished; });
            if (is_finished) {
                break;
            }
            std::cout << "Consumer " << static_cast<unsigned>(id_) << " noticed new element: " << shared_container_->back() << std::endl;
            last_size_ = shared_container_->size();
        }
        std::cout << "Consumer " << static_cast<unsigned>(id_) << " done" << std::endl;
    }

private:
    unsigned int id_;
    std::size_t last_size_;
    SharedContainerPtr<T> shared_container_;
};
// enum definition
enum class WorkerType
{
    kProducer,
    kConsumer
};
// template function with a compile time argument and variadic arguments
template <WorkerType worker_type, typename ... Args>
[[nodiscard]] WorkerPtr CreateWorker(Args&&... args)
{
    // compile time if statement excluding constructors of the classes to which
    // current arguments don't fit - an ordinary runtime if would cause a compilation error
    // because Producer's and Consumer's constructors have different signatures
    if constexpr (worker_type == WorkerType::kProducer) {
        // dynamic memory allocation of the polymorphic type
        return std::make_shared<Producer<std::string>>(std::forward<Args>(args)...);
    }
    else if constexpr (worker_type == WorkerType::kConsumer) {
        return std::make_shared<Consumer<std::string>>(std::forward<Args>(args)...);
    }
    else {
        throw std::runtime_error("Unsupported worker type: " + std::to_string(static_cast<unsigned int>(worker_type)));
    }
}
// application entry point
int main(int argc, char** argv)
{
    // start measuring execution time
    const auto start = std::chrono::high_resolution_clock::now();
    try {
        // fixed-length array
        const std::array<std::string, 3> source_data {"Hello", "world", "!!!"};
        // variable-length array
        std::vector<std::string> message_container;
        SharedContainerPtr<std::string> shared_container = std::make_shared<std::vector<std::string>>(message_container);
        // creation of the polymorphic types
        WorkerPtr producer = CreateWorker<WorkerType::kProducer>(source_data, shared_container);
        WorkerPtr first_consumer = CreateWorker<WorkerType::kConsumer>(0U, shared_container);
        WorkerPtr second_consumer = CreateWorker<WorkerType::kConsumer>(1U, shared_container);
        // start 3 threads
        auto producer_future = std::async([producer](){ producer->Run(); });
        auto first_consumer_future = std::async([first_consumer](){ first_consumer->Run(); });
        auto second_consumer_future = std::async([second_consumer](){ second_consumer->Run(); });
    }
    // error handling
    catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    // measure execution time and display the result
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}
