#pragma once

#include <vector>
#include <climits>
#include <string>
#include <tuple>
#include <utility>

template <typename unit=uint64_t>
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
		// �z���push
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
	BitVector<unit>& operator&=(const BitVector<unit>& obv) {
		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() &= (obv.bitvec_.back() & (std::numeric_limits<unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<unit>& operator|=(const BitVector<unit>& obv) {
		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] ^= obv.bitvec_[i];

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() |= (obv.bitvec_.back() & (std::numeric_limits<unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<unit>& operator^=(const BitVector<unit>& obv) {
		// ���[�� unit �ȊO�����Z
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// ���[�� unit �����Z
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() ^= (obv.bitvec_.back() & (std::numeric_limits<unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<unit>& operator<<=(const size_t pos) {
		// ����` push_left�����ȍ~
	}

	BitVector<unit>& operator>>=(const size_t pos) {
		// ������ push_right�����ȍ~
	}

	BitVector<unit> operator~() {
		BitVector<unit> not_bv(*this);
		for (int i = 0; i < this->bitvec_.size(); ++i) not_bv.bitvec_[i] = ~(this->bitvec_[i]);
		return not_bv;
	}

	bool operator==(const BitVector<unit>& obv) const {
		if (this->bitvec_.size() != this->bitvec_.size()) return false;

		for (auto i = 0; i < this->bitvec_.size(); ++i) {
			if (this->bitvec_[i] != obv.bitvec_[i]) return false;
		}
		return true;
	}

	bool operator!=(const BitVector<unit>& obv) const {
		return !(*this == obv);
	}

	BitVector<unit>& operator<<(const size_t pos) const {
		// ������ <<=�����ȍ~
	}

	BitVector<unit>& operator>>(const size_t pos) const {
		// ������ >>= �����ȍ~
	}



	/*------------------------------ ���� ------------------------------*/
public:
	BitVector<unit>& set(const size_t pos, const bool val=1) {
		auto [byteidx, bitidx] = conv_seqidx2bytebitidx(pos);

		this->bitvec_[byteidx] |= unit(1 << bitidx);

		return *this;
	}

	void reset() {
		this->bitvec_.clear();
	}


	/*------------------------------ �����o�ϐ� -------------------------------*/
protected:
	// �r�b�g��̖{��
	std::vector<unit> bitvec_;

	// unit�����r�b�g������
	static constexpr size_t unit_size_ = sizeof(uint64_t) * CHAR_BIT;

	// �L���r�b�g����
	int size_;

	/*------------------------------ �e�X�g�p ------------------------------*/
public:
	std::vector<unit> bitvec() const { return this->bitvec_; }
};

template<typename unit>
BitVector<unit> operator&(BitVector<unit> lbv, const BitVector<unit>& rbv) {

}

template<typename unit>
BitVector<unit>& operator|(BitVector<unit> lbv, const BitVector<unit>& rbv) {

}

template<typename unit>
BitVector<unit>& operator^(BitVector<unit> lbv, const BitVector<unit>& rbv) {

}