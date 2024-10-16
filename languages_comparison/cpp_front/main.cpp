

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "main.cpp2"

#line 12 "main.cpp2"
class WorkerInterface;
    

#line 21 "main.cpp2"
template<typename T> class Producer;
    

#line 61 "main.cpp2"
template<typename T> class Consumer;
    

#line 96 "main.cpp2"
class WorkerType;
    

//=== Cpp2 type definitions and function declarations ===========================

#line 1 "main.cpp2"
#include <shared_mutex>        // needed for std::shared_mutex
#include <chrono>              // needed for time measurement
#include <future>              // needed for std::async
#include <condition_variable>  // needed for std::condition_variable_any
// mutex for shared resource access synchronization
static std::shared_mutex mtx;
// condition variable for threads execution flow synchronization
static std::condition_variable_any cv;
// atomic variable to share a flag between the threads
std::atomic<bool> is_finished = false;

#line 12 "main.cpp2"
class WorkerInterface {
    public: virtual ~WorkerInterface() noexcept;
    public: virtual auto Run() -> void = 0;
    public: WorkerInterface() = default;
    public: WorkerInterface(WorkerInterface const&) = delete; /* No 'that' constructor, suppress copy */
    public: auto operator=(WorkerInterface const&) -> void = delete;

#line 15 "main.cpp2"
};
// alias for a type
using WorkerPtr = std::shared_ptr<WorkerInterface>;
// templated alias for a type
template<typename T> using SharedContainerPtr = std::shared_ptr<std::vector<T>>;
// template class definition
template<typename T> class Producer: public WorkerInterface {
    // inheritance

    // class constructor and members initialization
    // together with passing arguments by const reference and non-const reference
    public: explicit Producer(std::array<T,3> const& source_data, SharedContainerPtr<T> shared_container);

#line 35 "main.cpp2"
    public: auto Run() -> void override;

#line 56 "main.cpp2"
    // private class members
    private: std::array<T,3> source_data_; 
    private: SharedContainerPtr<T> shared_container_; 
    public: Producer(Producer const&) = delete; /* No 'that' constructor, suppress copy */
    public: auto operator=(Producer const&) -> void = delete;

#line 59 "main.cpp2"
};

template<typename T> class Consumer: public WorkerInterface {

    public: explicit Consumer(cpp2::impl::in<uint> id, SharedContainerPtr<T> shared_container);

#line 73 "main.cpp2"
    public: auto Run() -> void override;

#line 91 "main.cpp2"
    private: uint id_; 
    private: std::size_t last_size_; 
    private: SharedContainerPtr<T> shared_container_; 
    public: Consumer(Consumer const&) = delete; /* No 'that' constructor, suppress copy */
    public: auto operator=(Consumer const&) -> void = delete;

#line 94 "main.cpp2"
};
// enum definition
class WorkerType {
private: cpp2::u8 _value; private: constexpr WorkerType(cpp2::impl::in<cpp2::i64> _val);

private: constexpr auto operator=(cpp2::impl::in<cpp2::i64> _val) -> WorkerType& ;
public: static const WorkerType kProducer;
public: static const WorkerType kConsumer;
public: [[nodiscard]] constexpr auto get_raw_value() const& -> cpp2::u8;
public: constexpr explicit WorkerType();
public: constexpr WorkerType(WorkerType const& that);
public: constexpr auto operator=(WorkerType const& that) -> WorkerType& ;
public: constexpr WorkerType(WorkerType&& that) noexcept;
public: constexpr auto operator=(WorkerType&& that) noexcept -> WorkerType& ;
public: [[nodiscard]] auto operator<=>(WorkerType const& that) const& -> std::strong_ordering = default;
public: [[nodiscard]] auto to_string_impl(cpp2::impl::in<std::string_view> prefix) const& -> std::string;
public: [[nodiscard]] auto to_string() const& -> std::string;
public: [[nodiscard]] auto to_code() const& -> std::string;
public: [[nodiscard]] static auto from_string(cpp2::impl::in<std::string_view> s) -> WorkerType;
public: [[nodiscard]] static auto from_code(cpp2::impl::in<std::string_view> s) -> WorkerType;

#line 99 "main.cpp2"
};
// template function with a compile time argument and variadic arguments
template<cpp2::u8 worker_type, typename ...Args> [[nodiscard]] auto CreateWorker(Args const& ...args) -> WorkerPtr;

#line 116 "main.cpp2"
// application entry point
auto main() -> int;

//=== Cpp2 function definitions =================================================

#line 1 "main.cpp2"

#line 11 "main.cpp2"
// interface class

#line 13 "main.cpp2"
    WorkerInterface::~WorkerInterface() noexcept{}

#line 26 "main.cpp2"
    template <typename T> Producer<T>::Producer(std::array<T,3> const& source_data, SharedContainerPtr<T> shared_container)
        : WorkerInterface{  }
        , source_data_{ source_data }
        , shared_container_{ cpp2::move(shared_container) }{

#line 30 "main.cpp2"
        // if statement
#line 31 "main.cpp2"
        if (!(shared_container_)) {
            std::cout << "Given shared_container is a nullptr!" << std::endl;
        }
    }
#line 35 "main.cpp2"
    template <typename T> auto Producer<T>::Run() -> void{
        // indexed for loop
        for ( auto const& i : cpp2::range(0,10000U) ) {
            // range based for loop
            for ( auto const& element : source_data_ ) {
                // print statement
                std::cout << "Putting " + cpp2::to_string(element) + " to the shared container" << std::endl;
                {
                    // blocking reading and writing from/to shared container
                    std::unique_lock<std::shared_mutex> lock {mtx}; 
                    // appending element to a variable-length array
                    CPP2_UFCS(push_back)((*cpp2::impl::assert_not_null(shared_container_)), element);
                }
                // unblocking all waiting threads
                CPP2_UFCS(notify_all)(cv);
            }
        }
        is_finished = true;
        CPP2_UFCS(notify_all)(cv);
        std::cout << "Producer done" << std::endl;
    }

#line 63 "main.cpp2"
    template <typename T> Consumer<T>::Consumer(cpp2::impl::in<uint> id, SharedContainerPtr<T> shared_container)
        : WorkerInterface{  }
        , id_{ id }
        , last_size_{ CPP2_UFCS_NONLOCAL(size)((*cpp2::impl::assert_not_null(shared_container))) }
        , shared_container_{ cpp2::move(shared_container) }{

#line 69 "main.cpp2"
        if (!(shared_container_)) {
            std::cout << "Given shared_container is a nullptr!" << std::endl;
        }
    }
#line 73 "main.cpp2"
    template <typename T> auto Consumer<T>::Run() -> void{
        // infinite while loop
        while( true ) {
            // blocking writing to the shared container
            std::shared_lock<std::shared_mutex> lock {mtx}; 
            // waiting for the input from another thread
            CPP2_UFCS(wait)(cv, lock, [&]() -> bool{return cpp2::impl::cmp_greater(CPP2_UFCS(size)((*cpp2::impl::assert_not_null(shared_container_))),last_size_) || is_finished; });
            static_cast<void>(cpp2::move(lock));
            if (is_finished) {
                break;
            }
            std::cout << "Consumer " + cpp2::to_string(id_) + " noticed new element: " + cpp2::to_string(CPP2_UFCS(back)((*cpp2::impl::assert_not_null(shared_container_)))) + "" << std::endl;
            last_size_ = CPP2_UFCS(size)((*cpp2::impl::assert_not_null(shared_container_)));
        }

        std::cout << "Consumer " + cpp2::to_string(id_) + " done" << std::endl;
    }


    constexpr WorkerType::WorkerType(cpp2::impl::in<cpp2::i64> _val)
                                                          : _value{ cpp2::unchecked_narrow<cpp2::u8>(_val) } {  }

constexpr auto WorkerType::operator=(cpp2::impl::in<cpp2::i64> _val) -> WorkerType&  { 
                                                          _value = cpp2::unchecked_narrow<cpp2::u8>(_val);
                                                          return *this; }
inline CPP2_CONSTEXPR WorkerType WorkerType::kProducer{ 0U };

inline CPP2_CONSTEXPR WorkerType WorkerType::kConsumer{ 1U };

[[nodiscard]] constexpr auto WorkerType::get_raw_value() const& -> cpp2::u8 { return _value; }
constexpr WorkerType::WorkerType()
                                        : _value{ kProducer._value }{}
constexpr WorkerType::WorkerType(WorkerType const& that)
                                              : _value{ that._value }{}
constexpr auto WorkerType::operator=(WorkerType const& that) -> WorkerType& {
                                              _value = that._value;
                                              return *this;}
constexpr WorkerType::WorkerType(WorkerType&& that) noexcept
                                              : _value{ std::move(that)._value }{}
constexpr auto WorkerType::operator=(WorkerType&& that) noexcept -> WorkerType& {
                                              _value = std::move(that)._value;
                                              return *this;}
[[nodiscard]] auto WorkerType::to_string_impl(cpp2::impl::in<std::string_view> prefix) const& -> std::string{

    auto pref {cpp2::to_string(prefix)}; 
    if ((*this) == kProducer) {return pref + "kProducer"; }
    if ((*this) == kConsumer) {return cpp2::move(pref) + "kConsumer"; }
    return "invalid WorkerType value"; 
    }

    [[nodiscard]] auto WorkerType::to_string() const& -> std::string { return to_string_impl(""); }
[[nodiscard]] auto WorkerType::to_code() const& -> std::string { return to_string_impl("WorkerType::"); }
[[nodiscard]] auto WorkerType::from_string(cpp2::impl::in<std::string_view> s) -> WorkerType{

    auto x {s}; 
    if ("kProducer" == x) {return kProducer; }
    else {if ("kConsumer" == cpp2::move(x)) {return kConsumer; }
#line 1 "main.cpp2"
}
CPP2_UFCS(report_violation)(cpp2::type_safety, CPP2_UFCS(c_str)(("can't convert string '" + cpp2::to_string(s) + "' to enum of type WorkerType")));
return kProducer; 
}

[[nodiscard]] auto WorkerType::from_code(cpp2::impl::in<std::string_view> s) -> WorkerType{
std::string str {s}; return from_string(cpp2::string_util::replace_all(cpp2::move(str), "WorkerType::", "")); }

#line 101 "main.cpp2"
template<cpp2::u8 worker_type, typename ...Args> [[nodiscard]] auto CreateWorker(Args const& ...args) -> WorkerPtr{
    // compile time if statement excluding constructors of the classes to which
    // current arguments don't fit - an ordinary runtime if would cause a compilation error
    // because Producer's and Consumer's constructors have different signatures
    if constexpr (worker_type == 0U) {
        // dynamic memory allocation of the polymorphic type
        return CPP2_UFCS_TEMPLATE(cpp2_new<Producer<std::string>>)(cpp2::shared, args...); 
    }
    else {if constexpr (worker_type == 1U) {
        return CPP2_UFCS_TEMPLATE(cpp2_new<Consumer<std::string>>)(cpp2::shared, args...); 
    }
    else {
        std::cout << "Unsupported worker type: " + cpp2::to_string(worker_type) + "" << std::endl;
    }}
}

#line 117 "main.cpp2"
auto main() -> int{
    // start measuring execution time
    auto const start {std::chrono::high_resolution_clock::now()}; 
    {
        // fixed-length array
        std::array<std::string,3> const source_data {"Hello", "world", "!!!"}; 
        // variable-length array
        std::vector<std::string> message_container {}; 
        SharedContainerPtr<std::string> shared_container {CPP2_UFCS_TEMPLATE(cpp2_new<std::vector<std::string>>)(cpp2::shared, cpp2::move(message_container))}; 
        // creation of the polymorphic types
        WorkerPtr producer {CreateWorker<0U>(cpp2::move(source_data), shared_container)}; 
        WorkerPtr first_consumer {CreateWorker<1U>(0U, shared_container)}; 
        WorkerPtr second_consumer {CreateWorker<1U>(1U, shared_container)}; 
        // start 3 threads
        auto producer_future {std::async([_0 = cpp2::move(producer)]() mutable -> void{CPP2_UFCS(Run)((*cpp2::impl::assert_not_null((_0)))); })}; 
        auto first_consumer_future {std::async([_0 = cpp2::move(first_consumer)]() mutable -> void{CPP2_UFCS(Run)((*cpp2::impl::assert_not_null((_0)))); })}; 
        auto second_consumer_future {std::async([_0 = cpp2::move(second_consumer)]() mutable -> void{CPP2_UFCS(Run)((*cpp2::impl::assert_not_null((_0)))); })}; 
        std::cout << CPP2_UFCS(size)((*cpp2::impl::assert_not_null(cpp2::move(shared_container)))) << std::endl;
    }
    // measure execution time and display the result
    auto const end {std::chrono::high_resolution_clock::now()}; 
    std::cout << "Time elapsed: " << CPP2_UFCS(count)(std::chrono::duration_cast<std::chrono::milliseconds>(cpp2::move(end) - cpp2::move(start))) << "ms" << std::endl;
}

