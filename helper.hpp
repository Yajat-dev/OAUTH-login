#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define outchar(data) dec << static_cast<uint16_t>(data) << 'x' << hex << uppercase << static_cast<uint16_t>(data) << dec
#define outvar(data) dec << data << 'x' << hex << uppercase << data << dec
#define outtime(time) time << hex << 'x' << uint64_t(time) << dec
#define outpair(first, second) dec << first << '/' << second
#define outpaix(first, second) hex << uppercase << 'x' << first << "/x" << second << dec
#define clean "\033[2K\r"
#define tab '\t'
#define kB (1 << 10)
#define MB (1 << 20)

using LBA = uint64_t;

bool ldump(const void*, uint, uint = 0);
bool pdump(const void*, const void*);
bool dump(LBA, const std::vector<char>&);
void confirm(std::string&& info = std::string());
std::string& lower(std::string&);
