#include "sdl/sdl_storage.h"

#include "gmock/gmock.h"
#include <expected>

class mock_storage : public sdl::istorage
{
  public:
    MOCK_METHOD(bool, is_ready, (), (const, noexcept, override));
    MOCK_METHOD(void, wait_until_ready, (), (const, noexcept, override));
    MOCK_METHOD((std::expected<uint64_t, std::string>), get_file_size, (const std::string),
                (const, noexcept, override));
    MOCK_METHOD(std::optional<std::vector<std::byte>>, read_file, (const std::string), (const, noexcept, override));
};
