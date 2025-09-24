#ifndef CHEXT_TEST_UTIL_HAL_HPP_INCLUDED
#define CHEXT_TEST_UTIL_HAL_HPP_INCLUDED

#include <hdlstuff_hal/hal.hpp>

namespace chext_test::util {

namespace hal = hdlstuff::hal;

extern std::shared_ptr<hal::Sleep> halSleep;

}

#endif // CHEXT_TEST_UTIL_HAL_HPP_INCLUDED
