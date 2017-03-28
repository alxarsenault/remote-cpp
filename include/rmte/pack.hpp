#pragma once

#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <msgpack.hpp>

namespace rmte {
    
//    std::vector<std::uint8_t> buffer(
//                                     (std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());
    
    std::vector<std::uint8_t> sstream_to_vector(std::stringstream&& ss) {
        ss.seekg(0, std::ios::end);
        std::size_t size = ss.tellg();
        ss.seekg(0, std::ios::beg);
        
        std::vector<std::uint8_t> data(size);
        ss.read((char*)&data[0], size);
        return data;
    }
} // rmte
