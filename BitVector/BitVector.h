#pragma once

#include <vector>
#include <climits>
#include <string>
#include <tuple>
#include <utility>

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
		this->bitvec_.resize(get_byteidx_from_seqidx(size - 1) + 1, 0);
	}

	BitVector(const std::string str_bit) 
		: BitVector(str_bit.size())
	{
		// 配列にpush
	}

	/*------------------------------- index変換 ------------------------------*/

	// indexの種類
	// seqidx: ビット列全体のidx．e.g 右から65番目であれば65となる
	// byteidx, bitidx: 
	//     byteidx * unit_size + bitidxとなる
	//     つまり，byteidx が bitvec の idx で，bitidxはその位置の unit の何ビット目かを示す
	//     e.g 右から65番目であれば，unit_size=68 とすると，byteidx=1, bitidx=4 となる
private:
	static int conv_vecidx2bitidx(const int vecidx) { return vecidx * unit_size_; }
	static int conv_bitidx2vecidx(const int bitidx) { return int(bitidx / unit_size_); }

	static int get_byteidx_from_seqidx(const int seqidx) { return int(seqidx / unit_size_); }
	static int get_bitidx_from_seqidx(const int seqidx) { return int(seqidx % unit_size_); }

	static std::pair<int, int> conv_seqidx2bytebitidx(const int seqidx) {
		return std::make_pair<int, int>(get_byteidx_from_seqidx(seqidx), get_bitidx_from_seqidx(seqidx));
	}

	/*------------------------------ 演算子 -------------------------------*/
public:
	BitVector<unit>& operator&=(const BitVector<unit>& obv) {
		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// 左端の unit を演算
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() &= (obv.bitvec_.back() & (std::numeric_limits<unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<unit>& operator|=(const BitVector<unit>& obv) {
		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] ^= obv.bitvec_[i];

		// 左端の unit を演算
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() |= (obv.bitvec_.back() & (std::numeric_limits<unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<unit>& operator^=(const BitVector<unit>& obv) {
		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// 左端の unit を演算
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() ^= (obv.bitvec_.back() & (std::numeric_limits<unit>::max() >> (unit_size_ - 1 - bitidx)));

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
	BitVector<unit>& set(const size_t pos, const bool val=1) {
		auto [byteidx, bitidx] = conv_seqidx2bytebitidx(pos);

		this->bitvec_[byteidx] |= unit(1 << bitidx);

		return *this;
	}

	void reset() {
		this->bitvec_.clear();
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