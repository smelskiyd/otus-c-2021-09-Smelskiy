//
// Created by Danya Smelskiy on 07.11.2021.
//

#include "utf8.h"

const int kUTF8BOM = 15711167;

int GetNumberOfOctets(int c) {
    if (0 <= c && c <= 0x0000007F) {
        return 1;
    } else if (0x00000080 <=c && c <= 0x000007FF) {
        return 2;
    } else if (0x00000800 <= c && c <= 0x0000FFFF) {
        return 3;
    } else if (0x00010000 <= c && c <= 0x0010FFFF) {
        return 4;
    }

    // Undefined code
    return -1;
}

int ConvertUnicodeToUTF8(int uni_c) {
    int octets = GetNumberOfOctets(uni_c);
    if (octets == -1) {
        return -1;
    }
    int result = 0;
    if (octets > 1) {
        // Get the highest octet
        int highest_bits = (uni_c >> ((octets - 1) * 6));

        result = highest_bits;
        // Store the number of octets in the highest octet
        for (int i = 1; i <= octets; ++i) {
            result |= (1 << (8 - i));
        }

        uni_c ^= (highest_bits << ((octets - 1) * 6));

        for (int i = 1; i < octets; ++i) {
            // Get the bits in the next octet
            highest_bits = (uni_c >> ((octets - i - 1) * 6));
            // Remove them from uni_c
            uni_c ^= (highest_bits << ((octets - i - 1) * 6));
            // Add 10 prefix to the new octet
            int cur_byte = highest_bits | (1 << 7);
            // Push new octet to the result
            result <<= 8;
            result |= cur_byte;
        }
    } else {
        result = uni_c;
    }

    return result;
}
