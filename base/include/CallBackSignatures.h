#include <functional>

using WriteCallBack = std::function<size_t(char*, size_t, size_t, void*)>;
using WriteCallBackPointer = size_t (*)(char*, size_t, size_t, void*);
