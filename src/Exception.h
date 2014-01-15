/**
 * @file    Exception.h
 * @brief   Custom exception based on std::exception.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <stdexcept>
#include <string>

namespace gltext {

/**
 * @brief gltext Exception derived from std::exception
 */
class Exception : public std::runtime_error {
public:
    Exception(const std::string& aWhat) : std::runtime_error(aWhat) {}
};


} // namespace gltext
