#include <cstdint>
namespace zpods {
    enum class Status : uint8_t {
        OK,
        ERROR
    };

    enum class FileType : uint8_t {
        DIRECTORY,
        REGULAR_FILE,
        SYMLINK,
        PIPE,
        SOCKET,
        CHARACTER_DEVICE,
        BLOCK_DEVICE,
        UNKNOWN
    };

}