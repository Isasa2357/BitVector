#pragma once

#include <vector>
#include <climits>
#include <string>

template <typename unit=uint64_t>
class BitVector {

	/*------------------------------ コンストラクタ -------------------------------*/
public:
	// デフォルトコンストラクタ
	BitVector() 
		: size_(0)
	{
		this->bitvec_.resize(0);
	}

	// コピーコンストラクタ
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
		// 配列にpush
	}

	/*------------------------------- index変換 ------------------------------*/
private:
	int conv_vecidx2bitidx(const int vecidx) { return vecidx * unit_size_; }
	int conv_bitidx2vecidx(const int bitidx) { return int(bitidx / unit_size_); }

	/*------------------------------ 演算子 -------------------------------*/
public:
	BitVector<unit>& operator&=(const BitVector<unit>& obv) {
		// サイズ調整
		int iter_count = std::min(this->bitvec_.size(), obv.bitvec_.size());
		if (this->bitvec_.size() == iter_count) this->bitvec_.resize(obv.bitvec_.size());

		// 演算を実行
		for (auto i = 0; i < iter_count; ++i) {
			this->bitvec_[i] &= obv.bitvec_[i];
		}

		return *this;
	}

	BitVector<unit>& operator|=(const BitVector<unit>& obv) {
		// サイズ調整
		int iter_count = std::min(this->bitvec_.size(), obv.bitvec_.size());
		if (this->bitvec_.size() == iter_count) this->bitvec_.resize(obv.bitvec_.size());

		// 演算を実行
		for (auto i = 0; i < iter_count; ++i) {
			this->bitvec_[i] |= obv.bitvec_[i];
		}

		return *this;
	}

	BitVector<unit>& operator^=(const BitVector<unit>& obv) {
		// サイズ調整
		int iter_count = std::min(this->bitvec_.size(), obv.bitvec_.size());
		if (this->bitvec_.size() == iter_count) this->bitvec_.resize(obv.bitvec_.size());

		// 演算を実行
		for (auto i = 0; i < iter_count; ++i) {
			this->bitvec_[i] ^= obv.bitvec_[i];
		}

		return *this;
	}

	BitVector<unit>& operator<<=(const size_t pos) {
		// 未定義 push_left実装以降
	}

	BitVector<unit>& operator>>=(const size_t pos) {
		// 未実装 push_right実装以降
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
		// 未実装 <<=実装以降
	}

	BitVector<unit>& operator>>(const size_t pos) const {
		// 未実装 >>= 実装以降
	}



	/*------------------------------ 操作 ------------------------------*/
public:
	void set(unit x) {
		this->bitvec_[0] = x;
	}


	/*------------------------------ メンバ変数 -------------------------------*/
protected:
	// ビット列の本体
	std::vector<unit> bitvec_;

	// unitが何ビット整数か
	static constexpr size_t unit_size_ = sizeof(uint64_t) * CHAR_BIT;

	// 有効ビット長さ
	int size_;

	/*------------------------------ テスト用 ------------------------------*/
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