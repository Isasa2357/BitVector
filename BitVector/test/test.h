#include <iostream>
#include <random>

#include "test_usil.h"

#include "../BitVector.h"

using namespace std;

int popcount(uint64_t x) {
	int c = 0;
	while (x) {
		x &= (x - 1);  // 1ビット消す
		++c;
	}
	return c;
}

void test_popcount() {
	for (uint64_t i = 0; i < std::numeric_limits<uint64_t>::max(); ++i) {
		int bs_count = std::bitset<64>(i).count();
		int pop_count = popcount(i);

		if (bs_count != pop_count) std::cout << "miss\n";
	}
}

void test_bv_basic() {
	BitVector<> bv1(5), bv2(64), bv3(64);

	bv1.set(3);
	bv2.set(5).set(3).set(63);
	bv3.set(3);

	bv1.show();
	bv2.show();
	bv3.show();


	std::cout << "comp\n";
	std::cout << (bv1 == bv2) << ", " << (bv1 == bv3) << std::endl;
	std::cout << (bv1 != bv2) << ", " << (bv1 != bv3) << std::endl;

	std::cout << "not" << std::endl;
	(~bv1).show();

	std::cout << "count" << std::endl;
	std::cout << bv1.count() << ", " << bv2.count() << ", " << bv3.count() << std::endl;
}

void test_shift() {
	uint64_t x = 1;
	for (uint64_t i = 0; i < 64; ++i) {
		std::cout << i << ", " << std::bitset<64>(x) << std::endl;
		x <<= 1;
	}
}

void test_bv_test() {
	BitVector<> bv1(5), bv2(68), bv3(64);
	bv2.set(5).set(3).set(63).set(67);

	for (int i = 0; i < 68; ++i) {
		std::cout << i << "," << bv2.test(i) << "\n";
	}
}

void test_bv_all() {
	BitVector<> bv1(5), bv2(68), bv3(64);

	// bv1
	for (int i = 0; i < 64; ++i) {
		bv1.set(i);
	}
	bv1.show();
	assert(bv1.all());

	// bv2
	for (int i = 0; i < 68; ++i) {
		bv2.set(i);
	}
	bv2.show();
	assert(bv2.all());

	// bv1
	for (int i = 0; i < 64; ++i) {
		bv3.set(i);
	}
	bv3.show();
	assert(bv3.all());

	std::cout << "Done" << "\n";
}

void test_bv_to_string() {
	BitVector<> bv1(5), bv2(64), bv3(64);

	bv1.set(3);
	bv2.set(5).set(3).set(63);
	bv3.set(3);

	std::cout << bv1.to_string() << "\n";
	std::cout << bv2.to_string() << "\n";
	std::cout << bv3.to_string() << "\n";

	std::cout << "Done" << std::endl;
}

/*------------------------------ BitVector ------------------------------*/

void test_bv_constructor_str() {
	std::string str = "101010101111111111110000";
	BitVector<uint8_t> bv(str);
	std::bitset<24> bs(str);

	for (int i = 0; i < bv.bitvec().size(); ++i) {
		std::cout << unsigned long(bv.bitvec()[i]) << std::endl;
	}
	bv.show("");
	std::cout << bv.to_string() << std::endl;
	std::cout << bs << std::endl;
}

/*--------------------------- Unit util テスト ---------------------------*/

using namespace Unit_util;

void test_uu_single_bit() {
	for (int i = 0; i < 16; ++i) {
		assert(get_single_bit<uint16_t>(i) == (1 << i));
	}
}

void test_uu_make_seqmask() {
	const int size = 16;
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = i; j < size; ++j) {
			// 正解作成
			string str(size, '0');

			for (int k = 0; k < size; ++k) {
				if (i <= k && k < j) str[size - k - 1] = '1';
			}

			// 比較
			assert(make_seqmask<uint16_t>(i, j) == make_Unit<uint16_t>(str));
		}
	}
}

void test_uu_bitfield() {
	const int size = 8;
	for (uint8_t i = 0; i < numeric_limits<uint8_t>::max(); ++i) {

		for (auto j = 0; j <= size; ++j) {
			for (auto k = j; k <= size; ++k) {
				string str = bitset<size>(i).to_string();
				// 正解作成
				for (int l = 0; l < size; ++l) {
					if (!(j <= l && l < k)) str[size - l - 1] = '0';
				}

				// 比較
				assert(bitfield<uint8_t>(i, j, k) == make_Unit<uint8_t>(str));
			}
		}
	}
}

void test_uu_push_left() {
	const int size = 8;
	for (uint8_t i = 0; i < numeric_limits<uint8_t>::max(); ++i) {
		// 操作1
		string str_i = bitset<size>(i).to_string();
		auto [pushed, overwhelm] = push_left(i, "010");
		
		uint8_t pushed_ans = ((i << 3) | make_Unit<uint8_t>("010"));
		uint8_t overwhelm_ans = make_Unit<uint8_t>(str_i.substr(0, 3));
		assert(pushed == pushed_ans);
		assert(overwhelm == overwhelm_ans);

		// 操作2
		str_i = bitset<size>(i).to_string();
		auto [pushed2, overwhelm2] = push_left(i, "00000000");

		assert(pushed2 == make_Unit<uint8_t>("0"));
		assert(overwhelm2 == make_Unit<uint8_t>(str_i));
	}
}

void test_uu() {
	test_uu_single_bit();
	test_uu_make_seqmask();
	test_uu_bitfield();
	test_uu_push_left();
}

void test_uu_make_Unit() {
	std::string str = "01101";
	std::cout << Unit_util::make_Unit<int64_t>(str) << std::endl;
	std::cout << std::bitset<64>(str).to_ulong() << std::endl;
}
