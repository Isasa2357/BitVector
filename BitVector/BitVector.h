#pragma once

#include <vector>
#include <climits>
#include <string>
#include <tuple>
#include <utility>
#include <bitset>
#include <typeinfo>

/*------------------------------- unit util ------------------------------*/

namespace Unit_util {

	/*--------------------- �\���n --------------------*/

	void show_bin_uint8(uint8_t x) {
		std::cout << std::bitset<8>(x);
	}

	void show_bin_uint16(uint16_t x) {
		std::cout << std::bitset<16>(x);
	}

	void show_bin_uint32(uint32_t x) {
		std::cout << std::bitset<32>(x);
	}

	void show_bin_uint64(uint64_t x) {
		std::cout << std::bitset<64>(x);
	}

	template <typename Unit>
	void show_bin(Unit x, std::string end = "\n") {
		size_t unit_size = sizeof(Unit) * CHAR_BIT;
		if (unit_size == 8) {
			show_bin_uint8(x);
		} else if (unit_size == 16) {
			show_bin_uint16(x);
		} else if (unit_size == 32) {
			show_bin_uint32(x);
		} else if (unit_size == 64) {
			show_bin_uint64(x);
		}
		std::cout << end;
	}

	/*-------------------- �J�E���g�n --------------------*/

	int popcount(uint64_t x) {
		int c = 0;
		while (x) {
			x &= (x - 1);  // 1�r�b�g����
			++c;
		}
		return c;
	}

	/*-------------------- �����n --------------------*/

	template <typename Unit>
	Unit get_single_bit(const size_t pos) {
		// pos�̈ʒu�̂�1�� Unit �𐶐�

		return Unit(1) << pos;
	}

	template <typename Unit>
	Unit make_seqmask(const size_t begin, const size_t end) {
		// [begin, end) ��1�ł���ȊO��0�̃r�b�g��𐶐�

		size_t size = sizeof(Unit) * CHAR_BIT;
		Unit mask = std::numeric_limits<Unit>::max();
		mask <<= size - end;
		mask >>= ((size - end - 1) + begin + 1);
		mask <<= begin;

		return mask;
	}

	template <typename Unit>
	Unit make_Unit(std::string x) {
		// �r�b�g�̕�����\���� Unit�ɕϊ�

		Unit ret = Unit(0);

		for (auto& bit : x) {
			ret <<= 1;
			if (bit == '1') ret += 1;
		}

		return ret;
	}


	/*-------------------- ����n --------------------*/

	template <typename Unit>
	Unit bitfield(Unit x, int begin, int end) {
		// x �� begin����end�̂т��Ƃ𒊏o���C�͈͊O�� 0 ���߂������̂𐶐�

		// �}�X�N�쐬
		Unit mask = make_seqmask<Unit>(begin, end);

		// ���o
		return x & mask;
	}

	template <typename Unit>
	std::pair<Unit, Unit> push_left(Unit unit, std::string push_val = '0') {
		// unit �� push_val ��left push shift�������̂ƁC��ꂽ���̂�Ԃ�

		// aft_push: push������unit
		// overwhelm: ��ꂽ�l
		Unit aft_push(unit);
		Unit overwhelm(unit);

		// aft_push�v�Z
		aft_push <<= push_val.size();
		aft_push |= make_Unit<Unit>(push_val);

		// overwhelm�v�Z
		size_t unit_size = sizeof(Unit) * CHAR_BIT;
		overwhelm >>= (unit_size - push_val.size());

		return std::make_pair(aft_push, overwhelm);
	}

	
}

template <typename Unit=uint64_t>
class BitVector {

	/*------------------------------ �R���X�g���N�^ -------------------------------*/
public:
	// �f�t�H���g�R���X�g���N�^
	BitVector() 
		: size_(0)
	{
		this->bitvec_.resize(0);
	}

	// �R�s�[�R���X�g���N�^
	BitVector(BitVector& bv)
		: bitvec_(bv.bitvec_), size_(bv.size_)
	{}

	BitVector(const size_t size) 
		: size_(size)
	{
		this->bitvec_.resize(get_byteidx_from_seqidx(size - 1) + 1, 0);
	}

	BitVector(const std::string str_bit) 
		: BitVector(str_bit.size())
	{
		// ���[�ȊO��Unit�̑��
		auto i = 0;
		for (i = 0; i < this->bitvec_.size() - 1; ++i) {
			this->bitvec_[i] = Unit_util::make_Unit<Unit>(str_bit.substr(str_bit.size() - (unit_size_ * (i + 1)), unit_size_));
		}

		// ���[��Unit�̑��
		std::cout << str_bit.substr(0, str_bit.size() - (unit_size_ * i) + 1) << std::endl;
		this->bitvec_[i] = Unit_util::make_Unit<Unit>(str_bit.substr(0, str_bit.size() - (unit_size_ * i)));
	}

	/*------------------------------- index�ϊ� ------------------------------*/

	// index�̎��
	// seqidx: �r�b�g��S�̂�idx�De.g �E����65�Ԗڂł����65�ƂȂ�
	// byteidx, bitidx: 
	//     byteidx * unit_size + bitidx�ƂȂ�
	//     �܂�Cbyteidx �� bitvec �� idx �ŁCbitidx�͂��̈ʒu�� unit �̉��r�b�g�ڂ�������
	//     e.g �E����65�Ԗڂł���΁Cunit_size=68 �Ƃ���ƁCbyteidx=1, bitidx=4 �ƂȂ�
private:
	static int conv_vecidx2bitidx(const int vecidx) { return vecidx * unit_size_; }
	static int conv_bitidx2vecidx(const int bitidx) { return int(bitidx / unit_size_); }

	static int get_byteidx_from_seqidx(const int seqidx) { return int(seqidx / unit_size_); }
	static int get_bitidx_from_seqidx(const int seqidx) { return int(seqidx % unit_size_); }

	static std::pair<int, int> conv_seqidx2bytebitidx(const int seqidx) {
		return std::make_pair<int, int>(get_byteidx_from_seqidx(seqidx), get_bitidx_from_seqidx(seqidx));
	}

	/*------------------------------ ���Z�q -------------------------------*/
public:
	BitVector<Unit>& operator&=(const BitVector<Unit>& obv) {
		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() &= (obv.bitvec_.back() & (std::numeric_limits<Unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<Unit>& operator|=(const BitVector<Unit>& obv) {
		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] ^= obv.bitvec_[i];

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() |= (obv.bitvec_.back() & (std::numeric_limits<Unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<Unit>& operator^=(const BitVector<Unit>& obv) {
		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() ^= (obv.bitvec_.back() & (std::numeric_limits<Unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<Unit>& operator<<=(const size_t pos) {
		// ����` push_left�����ȍ~
	}

	BitVector<Unit>& operator>>=(const size_t pos) {
		// ������ push_right�����ȍ~
	}

	BitVector<Unit> operator~() {
		BitVector<Unit> not_bv(*this);

		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) not_bv.bitvec_[i] = ~(this->bitvec_[i]);

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		not_bv.bitvec_.back() = ~(this->bitvec_.back() | (std::numeric_limits<Unit>::max() << (bitidx + 1)));

		return not_bv;
	}

	bool operator==(const BitVector<Unit>& obv) const {
		if (this->bitvec_.size() != this->bitvec_.size()) return false;

		for (auto i = 0; i < this->bitvec_.size(); ++i) {
			if (this->bitvec_[i] != obv.bitvec_[i]) return false;
		}
		return true;
	}

	bool operator!=(const BitVector<Unit>& obv) const {
		return !(*this == obv);
	}


	/*--------------- �V�t�g ---------------*/
	// �V�t�g�ɂ͈ȉ���3��ނ�����
	// shift: �ʏ�V�t�g�D�r�b�g��̒������ێ����C��ꂽ����0�Ŗ��߂�
	// cycle_shift: �z�V�t�g�D�r�b�g��̒������ێ����C�z�V�t�g�D
	// push_shift: ���V�t�g�ł́C�V�t�g�������r�b�g��̒�����������D���������͐ݒ肵���l�Ŗ��߂�D
	//             e.g. bv(101).push_shift_left(010) -> bv(101010)
	//                  bv(101).push_shift_right(01) -> bv(011)
	// �V�t�g���Z�q�͒ʏ�V�t�g�ɑΉ�������D

	/*---------- �ʏ�V�t�g ----------*/

	BitVector<Unit> shl(const size_t sh_pos) const {
		// sh_pos�� ���V�t�g

		return shl(std::string(sh_pos, '0'));
	}

	BitVector<Unit> shl(std::string push_val=0) const {
		// ���V�t�g��C�󂢂������� push_val �Ŗ��߂�
		BitVector<Unit> sh_bv(*this);

		// unit_shift_count: Unit���x���̃V�t�g��
		// bit_shift_count: �r�b�g���x���̃V�t�g��
		const int shift_count = push_val.size();
		const int unit_shift_count = shift_count / unit_size_;
		const int bit_shift_count = shift_count % unit_size_;

		//----- Unit���x���̃V�t�g
		for (auto i = this->bitvec_.size() - unit_shift_count - 1; i >= 0; --i) {
			this->bitvec_[i + unit_shift_count] = this->bitvec_[i];
		}

		//----- bit ���x���̃V�t�g
		// ���[�̉��Z
		auto [pushed, overwhelm] = Unit_util::push_left(this->bitvec_.back());
		this->bitvec_.back() = pushed;

		// ���[�ȊO�̉��Z
		for (auto i = this->bitvec_.size() - 2; i >= unit_shift_count; --i) {
			auto [pushed, overwhelm] = Unit_util::push_left(this->bitvec_[i]);
			this->bitvec_[i] = pushed;
			this->bitvec_[i] |= overwhelm;
		}

		// push_val �Ŗ��߂�


	}

	BitVector<Unit> shr(const size_t sh_pos) const {
		return shr(std::string(sh_pos, '0'));

	}

	BitVector<Unit> shr(std::string push_val = "0") const {
		return NULL;
	}

	BitVector<Unit> operator<<(const size_t pos) const {
		// ������ <<=�����ȍ~
	}

	BitVector<Unit> operator>>(const size_t pos) const {
		// ������ >>= �����ȍ~
	}

	/*---------- �z�V�t�g ----------*/
	BitVector<Unit> cshl(const size_t sh_pos) const {

	}

	BitVector<Unit> cshr(const size_t sh_pos) const {

	}

	/*---------- �����o���V�t�g ----------*/
	BitVector<Unit> pshl(const size_t sh_pos) const {

	}

	BitVector<Unit> pshr(const size_t sh_pos) const {

	}

	/*------------------------------ ���� ------------------------------*/
public:
	BitVector<Unit>& set(const size_t pos, const bool val=1) {
		if (pos > size_ - 1) return *this;

		auto [byteidx, bitidx] = conv_seqidx2bytebitidx(pos);

		this->bitvec_[byteidx] |= (Unit(1) << bitidx);

		return *this;
	}

	BitVector<Unit>& reset() {
		this->bitvec_.clear();
		return *this;
	}

	BitVector<Unit>& flip() {
		*this = ~(*this);
		return *this;
	}

	int count() const {
		int count = 0;
		// ���[�ȊO���J�E���g
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end() - 1; ++unit) {
			count += Unit_util::popcount(*unit);
		}

		// ���[���J�E���g
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = Unit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		count += Unit_util::popcount(left_unit);
		return count;
	}

	bool test(const size_t pos) const {
		auto [byteidx, bitidx] = conv_seqidx2bytebitidx(pos);

		Unit mask = Unit_util::get_single_bit<uint64_t>(bitidx);
		return this->bitvec_[byteidx] & mask;
	}

	bool all() const {
		// ���[�ȊO������
		Unit fill = std::numeric_limits<Unit>::max();
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end() - 1; ++unit) {
			if (*unit != fill) return false;
		}

		// ���[������
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = Unit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		fill = Unit_util::make_seqmask<Unit>(0, bitidx + 1);

		return left_unit == fill;
	}

	bool any() const {
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end(); ++unit) {
			if (*unit != 0) return true;
		}
		return false;
	}

	/*------------------------------ �^�ϊ� ------------------------------*/

	std::string to_string() const {
		std::string str = "";

		// ���[������
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = Unit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		for (auto i = bitidx; i >= 0; --i) {
			if (left_unit & Unit_util::get_single_bit<Unit>(i)) str += "1";
			else str += "0";
		}

		// ���[�ȊO������
		for (auto unit = this->bitvec_.rbegin() + 1; unit < this->bitvec_.rend(); ++unit) {
			str += std::bitset<unit_size_>(*unit).to_string();
		}

		return str;
	}


	/*------------------------------ �����o�ϐ� -------------------------------*/
protected:
	// �r�b�g��̖{��
	std::vector<Unit> bitvec_;

	// unit�����r�b�g������
	static constexpr size_t unit_size_ = sizeof(Unit) * CHAR_BIT;

	// �L���r�b�g����
	int size_;

	/*------------------------------ �e�X�g�p ------------------------------*/
public:
	std::vector<Unit> bitvec() const { return this->bitvec_; }

	void show(std::string end="\n") const {
		for (auto unit = this->bitvec_.rbegin(); unit != this->bitvec_.rend(); ++unit) {
			Unit_util::show_bin(*unit, end);
			std::cout << end;
		}
		std::cout << std::endl;
	}

	void show_val() const {
		for (auto unit = this->bitvec_.rbegin(); unit != this->bitvec_.rend(); ++unit) {
			std::cout << *unit << ",";
		}
		std::cout << std::endl;
	}
};

template<typename Unit>
BitVector<Unit> operator&(BitVector<Unit> lbv, const BitVector<Unit>& rbv) {
	return lbv &= rbv;
}

template<typename Unit>
BitVector<Unit> operator|(BitVector<Unit> lbv, const BitVector<Unit>& rbv) {
	return lbv |= rbv;
}

template<typename unit>
BitVector<unit> operator^(BitVector<unit> lbv, const BitVector<unit>& rbv) {
	return lbv ^= rbv;
}