#include "number.h"

void RotateLeft(uint239_t &value, uint32_t shift) {
    uint8_t bits[245] = {};
    uint32_t normal_shift = shift % 245;
    if (normal_shift == 0) return;

    int index_bit = 0;
    for (int i = 0; i < 35; ++i) {
        uint8_t byte = value.data[i] & 0x7F;  // 0111 1111
        for (int bit = 6; bit >= 0 && index_bit < 245; --bit) {
            bits[index_bit++] = (byte >> bit) & 1;
        }
    }

    uint8_t shifted_bits[245] = {};
    for (int i = 0; i < 245; ++i) {
        shifted_bits[i] = bits[(i + normal_shift) % 245];
    }

    index_bit = 0;
    for (int i = 0; i < 35; ++i) {
        uint8_t new_byte = 0;
        for (int bit = 6; bit >= 0 && index_bit < 245; --bit) {
            new_byte |= (shifted_bits[index_bit++] << bit);
        }
        value.data[i] = (value.data[i] & 0x80) | new_byte;
    }
}

void SetShift(uint239_t &value, uint32_t shift) {
    for (int i = 34; i >= 0; --i) {
        value.data[i] &= 0x7F;
        value.data[i] |= (shift & 1) << 7;
        shift >>= 1;
    }
}

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t result = {};

    for (int i = 34; i >= 0; i--) {
        result.data[i] = value & 0x7F;
        value >>= 7;
        if (value == 0) {
            break;
        }
    }

    SetShift(result, shift);
    RotateLeft(result, shift);

    return result;
}

uint239_t FromString(const char *str, uint32_t shift) {
    uint239_t result = {};

    for (int i = 0; str[i] != '\0'; ++i) {
        uint8_t carry = str[i] - '0';
        for (int j = 34; j >= 0; --j) {
            uint16_t temp = (result.data[j] & 0x7F) * 10 + carry;
            result.data[j] = (result.data[j] & 0x80) | (temp & 0x7F);
            carry = (temp >> 7);
        }
    }

    SetShift(result, shift);
    RotateLeft(result, shift);

    return result;
}

uint239_t Sum(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = {};

    int32_t buff = 0;
    for (int32_t i = 34; i >= 0; --i) {
        int32_t sum = (lhs.data[i] & 0x7F) + (rhs.data[i] & 0x7F) + buff;

        result.data[i] = (sum & 0x7F);
        buff = (sum >> 7);
    }

    return result;
}

void BackShift(uint239_t &num, uint32_t shift_amount) {
    uint8_t bit_array[245] = {};
    uint32_t normalized_shift = shift_amount % 245;

    int bit_index = 0;
    for (int i = 0; i < 35; ++i) {
        uint8_t current_byte = num.data[i] & 0x7F;
        for (int bit = 6; bit >= 0 && bit_index < 245; --bit) {
            bit_array[bit_index++] = (current_byte >> bit) & 1;
        }
    }

    uint8_t shifted_bits[245] = {};
    for (int i = 0; i < 245; ++i) {
        shifted_bits[i] = bit_array[(245 + i - normalized_shift) % 245];
    }

    bit_index = 0;
    for (int i = 0; i < 35; ++i) {
        uint8_t new_byte = 0;
        for (int bit = 6; bit >= 0 && bit_index < 245; --bit) {
            new_byte |= (shifted_bits[bit_index++] << bit);
        }
        num.data[i] = (num.data[i] & 0x80) | new_byte;
    }
}

uint239_t BackShiftNotVoid(const uint239_t &num, uint32_t shift_amount) {
    uint8_t bit_array[245] = {};
    uint239_t result = {};
    uint32_t normalized_shift = shift_amount % 245;

    int bit_index = 0;
    for (int i = 0; i < 35; ++i) {
        uint8_t current_byte = num.data[i] & 0x7F;
        for (int bit = 6; bit >= 0 && bit_index < 245; --bit) {
            bit_array[bit_index++] = (current_byte >> bit) & 1;
        }
    }

    uint8_t shifted_bits[245] = {};
    for (int i = 0; i < 245; ++i) {
        shifted_bits[i] = bit_array[(245 + i - normalized_shift) % 245];
    }

    bit_index = 0;
    for (int i = 0; i < 35; ++i) {
        uint8_t new_byte = 0;
        for (int bit = 6; bit >= 0 && bit_index < 245; --bit) {
            new_byte |= (shifted_bits[bit_index++] << bit);
        }
        result.data[i] = (num.data[i] & 0x80) | new_byte;
    }
    return result;
}
/*uint239_t operator+(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = {};

    int shiftLHS = GetShift(lhs);
    int shiftRHS = GetShift(rhs);
    int total_shift = shiftLHS + shiftRHS;

    result = Sum(BackShiftNotVoid(lhs, shiftLHS),
                 BackShiftNotVoid(rhs, shiftRHS));

    SetShift(result, total_shift);
    RotateLeft(result, total_shift);

    return result;
}*/
/*
uint239_t operator+(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = {};

    int shiftLHS = GetShift(lhs);
    int shiftRHS = GetShift(rhs);
    int total_shift = shiftLHS + shiftRHS;

    BackShift(const_cast<uint239_t &>(lhs), shiftLHS);
    BackShift(const_cast<uint239_t &>(rhs), shiftRHS);

    int32_t carry = 0;
    for (int i = 34; i >= 0; --i) {
        int32_t sum = (lhs.data[i] & 0x7F) + (rhs.data[i] & 0x7F) + carry;
        result.data[i] = (sum & 0x7F);
        carry = (sum >> 7);
    }

    SetShift(result, total_shift);
    return result;
}*/
uint239_t operator+(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = {};

    int shiftLHS = GetShift(lhs);
    int shiftRHS = GetShift(rhs);
    int total_shift = shiftLHS + shiftRHS;

    int32_t carry = 0;
    for (int i = 34; i >= 0; --i) {
        int32_t lhs_value =
            (i - shiftLHS >= 0) ? (lhs.data[i - shiftLHS] & 0x7F) : 0;
        int32_t rhs_value =
            (i - shiftRHS >= 0) ? (rhs.data[i - shiftRHS] & 0x7F) : 0;
        int32_t sum = lhs_value + rhs_value + carry;
        result.data[i] = (sum & 0x7F);
        carry = (sum >> 7);
    }

    SetShift(result, total_shift);

    int shift = total_shift % 245;
    if (shift != 0) {
        for (int i = 0; i < shift; ++i) {
            uint8_t carry_bit = (result.data[0] & 0x40) >> 6;
            for (int j = 0; j < 35; ++j) {
                uint8_t next_carry_bit =
                    (j < 34) ? (result.data[j + 1] & 0x40) >> 6 : 0;
                result.data[j] =
                    ((result.data[j] << 1) & 0x7F) | next_carry_bit;
            }
            result.data[34] = (result.data[34] & 0x7F) | carry_bit;
        }
    }
    
    return result;
}

uint239_t Diff(const uint239_t &minuend, const uint239_t &subtrahend) {
    uint239_t result = {};
    int32_t carry = 0;

    for (int32_t i = 34; i >= 0; --i) {
        int32_t diff =
            (minuend.data[i] & 0x7F) - (subtrahend.data[i] & 0x7F) + carry;

        result.data[i] = (diff & 0x7F);
        carry = (diff >> 7);
    }

    return result;
}

uint239_t operator-(const uint239_t &lhs, const uint239_t &rhs) {
    int32_t shift_LHS = GetShift(lhs);
    int32_t shift_RHS = GetShift(rhs);
    int32_t total_shift = shift_LHS - shift_RHS;

    uint239_t result = {};

    uint239_t lhs_normalize = lhs;
    uint239_t rhs_normalize = rhs;

    BackShift(lhs_normalize, shift_LHS);
    BackShift(rhs_normalize, shift_RHS);

    result = Diff(lhs_normalize, rhs_normalize);

    SetShift(result, total_shift);
    RotateLeft(result, total_shift);

    return result;
}

uint239_t operator*(const uint239_t &lhs, const uint239_t &rhs) {
    int32_t shift_LHS = GetShift(lhs);
    int32_t shift_RHS = GetShift(rhs);

    int32_t total_shift = shift_LHS + shift_RHS;

    uint239_t result = {};

    uint239_t lhs_normalize = lhs;
    uint239_t rhs_normalize = rhs;

    BackShift(lhs_normalize, shift_LHS);
    BackShift(rhs_normalize, shift_RHS);

    int shift = 0;
    uint239_t tmp = {};

    for (int i = 34; i >= 0; --i) {
        for (int j = 0; j <= 6; ++j) {
            uint8_t bitm = ((lhs_normalize.data[i] >> j) & 1);

            if (bitm == 1) {
                tmp = rhs_normalize;
                RotateLeft(tmp, shift);
                result = Sum(result, tmp);
            }
            shift += 1;
        }
    }

    SetShift(result, total_shift);
    RotateLeft(result, total_shift);

    return result;
}

bool operator>=(const uint239_t &lhs, const uint239_t &rhs) {
    for (int i = 0; i < 35; ++i) {
        if ((lhs.data[i] & 0x7f) > (rhs.data[i] & 0x7f)) return true;
        if ((lhs.data[i] & 0x7f) < (rhs.data[i] & 0x7f)) return false;
    }
    return true;
}

uint239_t operator/(const uint239_t &lhs, const uint239_t &rhs) {
    if (rhs == uint239_t()) {
        std::cerr << "Division by zero!" << std::endl;
        return uint239_t();
    }

    int32_t shift_LHS = GetShift(lhs);
    int32_t shift_RHS = GetShift(rhs);
    int32_t result_shift = shift_LHS - shift_RHS;

    uint239_t result = {};

    uint239_t lhs_normalize = lhs;
    uint239_t rhs_normalize = rhs;

    BackShift(lhs_normalize, shift_LHS);
    BackShift(rhs_normalize, shift_RHS);

    uint239_t result_tmp = {};
    for (int i = 0; i < 35; i++) {
        for (int bitCnt = 6; bitCnt >= 0; --bitCnt) {
            uint16_t carry = 0;

            for (int i = 34; i >= 0; --i) {
                uint16_t temp = (result_tmp.data[i] & 0x7F) * 2 + carry;

                result_tmp.data[i] =
                    (result_tmp.data[i] & 0x80) | (temp & 0x7F);
                carry = temp >> 7;
            }

            carry = (lhs_normalize.data[i] >> bitCnt) & 1;

            for (int i = 34; i >= 0 && carry > 0; --i) {
                uint16_t temp = (result_tmp.data[i] & 0x7F) + carry;

                result_tmp.data[i] =
                    (result_tmp.data[i] & 0x80) | (temp & 0x7F);
                carry = temp >> 7;
            }

            if (result_tmp >= rhs_normalize) {
                unsigned shift = (34 - i) * 7 + bitCnt;
                RotateLeft(rhs_normalize, shift);
                lhs_normalize = Diff(lhs_normalize, rhs_normalize);
                BackShift(rhs_normalize, shift);
                result_tmp = Diff(result_tmp, rhs_normalize);
                result.data[i] |= (1 << bitCnt);
            }
        }
    }

    SetShift(result, result_shift);
    RotateLeft(result, result_shift);

    return result;
}

bool operator==(const uint239_t &lhs, const uint239_t &rhs) {
    uint64_t shift_LHS = GetShift(lhs);
    uint64_t shift_RHS = GetShift(rhs);

    for (int bit_index = 244; bit_index >= 0; --bit_index) {
        uint8_t idx_LHS = (245 + bit_index - shift_LHS) % 245;
        uint8_t bit_LHS = (lhs.data[idx_LHS / 7] >> (6 - (idx_LHS % 7))) & 1;

        uint8_t idx_RHS = (245 + bit_index - shift_RHS) % 245;
        uint8_t bit_RHS = (rhs.data[idx_RHS / 7] >> (6 - (idx_RHS % 7))) & 1;

        if (bit_LHS != bit_RHS) return false;
    }
    return true;
}

bool operator!=(const uint239_t &lhs, const uint239_t &rhs) {
    return !(rhs == lhs);
}

std::ostream &operator<<(std::ostream &stream, const uint239_t &value) {
    for (int i = 0; i < 35; ++i) {
        uint8_t byte = value.data[i];

        for (int bit = 7; bit >= 0; --bit) {
            stream << ((byte >> bit) & 1);
        }

        if (i != 34) {
            stream << " ";
        }
    }

    return stream;
}

uint64_t GetShift(const uint239_t &value) {
    uint64_t shift = 0;

    for (int i = 34; i >= 0; --i) {
        uint8_t service_bit = (value.data[i] >> 7) & 1;
        shift |= (service_bit << (34 - i));
    }

    return shift;
}
