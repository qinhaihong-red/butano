/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef FIXED_TESTS_H
#define FIXED_TESTS_H

#include <cmath>
#include "bn_math.h"
#include "bn_fixed.h"
#include "bn_string.h"
#include "tests.h"

class fixed_tests : public tests
{

public:
    fixed_tests() :
        tests("fixed")
    {
        BN_ASSERT(bn::fixed(4) + 4 == 8);
        BN_ASSERT(4 + bn::fixed(4) == 8);

        BN_ASSERT(bn::abs(bn::fixed(-4)) == 4);

        BN_ASSERT(bn::fixed_t<4>(bn::fixed(1)).data() == 16);

        BN_ASSERT(bn::fixed_t<5>(bn::fixed(1)).data() == 32);

        BN_ASSERT(bn::fixed_t<7>(bn::fixed(-1) + 1).data() == 0);
        BN_ASSERT(bn::fixed_t<7>(bn::fixed(0) + 1).data() == 128);
        BN_ASSERT(bn::fixed_t<7>(bn::fixed(1) + 1).data() == 256);

        BN_ASSERT(bn::fixed_t<8>(bn::fixed(1)).data() == 256);

        BN_ASSERT(bn::fixed_t<10>(bn::fixed(1)).data() == 1024);
        BN_ASSERT(bn::fixed_t<10>(bn::fixed(64)).data() == 65536);

        BN_ASSERT(bn::to_string<8>(bn::fixed(1)) == bn::string_view("1"),
                   "Fixed to string conversion failed: ", bn::to_string<8>(bn::fixed(1)));

        BN_ASSERT(bn::to_string<8>(bn::fixed(0.5)) == bn::string_view("0.50000"),
                   "Fixed to string conversion failed: ", bn::to_string<8>(bn::fixed(0.5)));

        BN_ASSERT(bn::to_string<8>(bn::fixed(0.25)) == bn::string_view("0.25000"),
                   "Fixed to string conversion failed: ", bn::to_string<8>(bn::fixed(0.25)));

        BN_ASSERT(bn::to_string<8>(bn::fixed(0.125)) == bn::string_view("0.12500"),
                   "Fixed to string conversion failed: ", bn::to_string<8>(bn::fixed(0.125)));

        BN_ASSERT(bn::to_string<8>(bn::fixed(0.0625)) == bn::string_view("0.06250"),
                   "Fixed to string conversion failed: ", bn::to_string<8>(bn::fixed(0.0625)));

        BN_ASSERT(bn::to_string<8>(bn::fixed(0.03125)) == bn::string_view("0.03125"),
                   "Fixed to string conversion failed: ", bn::to_string<8>(bn::fixed(0.03125)));

        _truncate_and_round_test(-2.5);
        _truncate_and_round_test(-2);
        _truncate_and_round_test(-1.5);
        _truncate_and_round_test(-1);
        _truncate_and_round_test(-0.5);
        _truncate_and_round_test(0);
        _truncate_and_round_test(0.5);
        _truncate_and_round_test(1);
        _truncate_and_round_test(1.5);
        _truncate_and_round_test(2);
        _truncate_and_round_test(2.5);
    }

private:
    static void _truncate_and_round_test(float value)
    {
        BN_ASSERT(bn::fixed(value).integer() == int(value), "Fixed truncate failed: ",
                   bn::fixed(value), " - ", bn::fixed(value).integer(), " - ", int(value));

        BN_ASSERT(bn::fixed(value).round_integer() == int(std::round(value)), "Fixed round failed: ",
                   bn::fixed(value), " - ", bn::fixed(value).round_integer(), " - ", int(std::round(value)));
    }
};

#endif
