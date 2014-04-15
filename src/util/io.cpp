#include <llmr/util/io.hpp>

#include <cstdio>

void llmr::util::write_file(const std::string &filename, const std::string &data) {
    FILE *fd = fopen(filename.c_str(), "wb");
    if (fd) {
        fwrite(data.data(), sizeof(std::string::value_type), data.size(), fd);
        fclose(fd);
    } else {
        fprintf(stderr, "Failed to open file\n");
    }
}
