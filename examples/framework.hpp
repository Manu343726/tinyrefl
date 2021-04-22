namespace framework
{

template<typename T>
struct ProcessResult
{
    T&   get();
    bool ready() const;
    bool wait(const int timeoutMs);
};

namespace detail
{
int                run(const char* command, const char** args);
ProcessResult<int> runAsync(const char* command, const char** args);
} // namespace detail

class Process
{
public:
    enum class RunMode
    {
        Async,
        Blocking
    };

    Process(const char* command);

    ProcessResult<int> run(const RunMode mode = RunMode::Async);

private:
    const char* _command;
};

ProcessResult<int> runProcess(const char* command);
} // namespace framework
