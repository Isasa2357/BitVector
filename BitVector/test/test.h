#include <iostream>

#include "test_usil.h"

#include "../BitVector.h"

int popcount(uint64_t x) {
	int c = 0;
	while (x) {
		x &= (x - 1);  // 1ÉrÉbÉgè¡Ç∑
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