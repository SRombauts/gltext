/**
 * @file    Exception.h
 * @brief   Custom exception based on std::exception.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <stdexcept>
#include <string>

namespace gltext {

/**
 * @brief gltext Exception derived from std::exception
 */
class Exception : public std::runtime_error {
public:
    /**
     * @brief Construct a new exception with the provided string message
     *
     * @param[in] aWhat String message explaining the error
     */
    explicit Exception(const std::string& aWhat) : std::runtime_error(aWhat) {}
};


} // namespace gltext
