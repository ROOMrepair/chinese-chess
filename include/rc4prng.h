#pragma once

#include <cstdint>
#include <chrono>
#include <cstdint>
#include <random>

struct RC4Struct {
    uint8_t s[256];
    int x, y;

    void Init(void* lpKey, int nKeyLen) {
        int i, j;
        x = y = j = 0;
        for (i = 0; i < 256; i++) {
            s[i] = i;
        }
        for (i = 0; i < 256; i++) {
            j = (j + s[i] + ((uint8_t*)lpKey)[i % nKeyLen]) & 255;
            std::swap(s[i], s[j]);
        }
    }

    void InitZero() {
        uint32_t dwKey = 0;
        Init(&dwKey, 4);
    }

    void InitRand() {
        union {
            uint32_t dw[2];
            uint64_t qw;
        } Seed;

        // 使用系统时间 + 高精度时钟作为种子
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        uint64_t time_part = epoch.count();

        auto hrc_now = std::chrono::high_resolution_clock::now();
        uint64_t hrc_part = hrc_now.time_since_epoch().count();

        Seed.qw = time_part ^ hrc_part;
        Init(&Seed, 8);
    }

    uint8_t NextByte() {
        x = (x + 1) & 255;
        y = (y + s[x]) & 255;
        std::swap(s[x], s[y]);
        return s[(s[x] + s[y]) & 255];
    }

    uint32_t NextLong() {
        uint32_t ret;
        ret = (NextByte() << 24) | (NextByte() << 16) | (NextByte() << 8) | NextByte();
        return ret;
    }
};