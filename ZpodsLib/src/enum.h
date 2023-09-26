namespace zpods {
    enum class Status : std::uint8_t {
        OK,
        ERROR
    };

    enum class FileType : std::uint8_t {
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
