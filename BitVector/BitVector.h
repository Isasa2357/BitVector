#pragma once

#include <vector>
#include <climits>
#include <string>

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
		this->bitvec_.resize(int(size / unit_size_) + 1, 0);
	}

	BitVector(const std::string str_bit) 
		: BitVector(str_bit.size())
	{
		// �z���push
	}

	/*------------------------------- index�ϊ� ------------------------------*/
private:
	int conv_vecidx2bitidx(const int vecidx) { return vecidx * unit_size_; }
	int conv_bitidx2vecidx(const int bitidx) { return int(bitidx / unit_size_); }

	/*------------------------------ ���Z�q -------------------------------*/
public:
	BitVector<unit>& operator&=(const BitVector<unit>& obv) {
		// �T�C�Y����
		int iter_count = std::min(this->bitvec_.size(), obv.bitvec_.size());
		if (this->bitvec_.size() == iter_count) this->bitvec_.resize(obv.bitvec_.size());

		// ���Z�����s
		for (auto i = 0; i < iter_count; ++i) {
			this->bitvec_[i] &= obv.bitvec_[i];
		}

		return *this;
	}

	BitVector<unit>& operator|=(const BitVector<unit>& obv) {
		// �T�C�Y����
		int iter_count = std::min(this->bitvec_.size(), obv.bitvec_.size());
		if (this->bitvec_.size() == iter_count) this->bitvec_.resize(obv.bitvec_.size());

		// ���Z�����s
		for (auto i = 0; i < iter_count; ++i) {
			this->bitvec_[i] |= obv.bitvec_[i];
		}

		return *this;
	}

	BitVector<unit>& operator^=(const BitVector<unit>& obv) {
		// �T�C�Y����
		int iter_count = std::min(this->bitvec_.size(), obv.bitvec_.size());
		if (this->bitvec_.size() == iter_count) this->bitvec_.resize(obv.bitvec_.size());

		// ���Z�����s
		for (auto i = 0; i < iter_count; ++i) {
			this->bitvec_[i] ^= obv.bitvec_[i];
		}

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
	void set(unit x) {
		this->bitvec_[0] = x;
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