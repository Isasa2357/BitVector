#pragma once

#include <vector>
#include <climits>
#include <string>
#include <tuple>
#include <utility>
#include <bitset>

namespace bit_util {
	int popcount(uint64_t x) {
		int c = 0;
		while (x) {
			x &= (x - 1);  // 1ビット消す
			++c;
		}
		return c;
	}

	template <typename T>
	T bitfield(T x, int begin, int end) {
		// マスク作成
		size_t size = sizeof(T) * CHAR_BIT;
		T mask = std::numeric_limits<T>::max();
		mask <<= size - end;
		mask >>= ((size - end - 1) + begin + 1);
		mask <<= begin;

		// 抽出
		return x & mask;
	}

	template <typename T>
	T get_single_bit(const size_t pos) {
		return T(1) << pos;
	}

	template <typename T>
	T get_fill_bit(const size_t begin, const size_t end) {
		// マスク作成
		size_t size = sizeof(T) * CHAR_BIT;
		T mask = std::numeric_limits<T>::max();
		mask <<= size - end;
		mask >>= ((size - end - 1) + begin + 1);
		mask <<= begin;

		return mask;
	}

	void show_bin_int64(int64_t x) {
		std::cout << std::bitset<64>(x) << std::endl;
	}
}

template <typename Unit=uint64_t>
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
	BitVector<Unit>& operator&=(const BitVector<Unit>& obv) {
		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// 左端の unit を演算
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() &= (obv.bitvec_.back() & (std::numeric_limits<Unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<Unit>& operator|=(const BitVector<Unit>& obv) {
		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] ^= obv.bitvec_[i];

		// 左端の unit を演算
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() |= (obv.bitvec_.back() & (std::numeric_limits<Unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<Unit>& operator^=(const BitVector<Unit>& obv) {
		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) this->bitvec_[i] &= obv.bitvec_[i];

		// 左端の unit を演算
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		this->bitvec_.back() ^= (obv.bitvec_.back() & (std::numeric_limits<Unit>::max() >> (unit_size_ - 1 - bitidx)));

		return *this;
	}

	BitVector<Unit>& operator<<=(const size_t pos) {
		// 未定義 push_left実装以降
	}

	BitVector<Unit>& operator>>=(const size_t pos) {
		// 未実装 push_right実装以降
	}

	BitVector<Unit> operator~() {
		BitVector<Unit> not_bv(*this);

		// 左端の unit 以外を演算
		for (auto i = 0; i < this->bitvec_.size() - 1; ++i) not_bv.bitvec_[i] = ~(this->bitvec_[i]);

		// 左端の unit を演算
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

	BitVector<Unit>& operator<<(const size_t pos) const {
		// 未実装 <<=実装以降
	}

	BitVector<Unit>& operator>>(const size_t pos) const {
		// 未実装 >>= 実装以降
	}

	/*------------------------------ 操作 ------------------------------*/
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
		// 左端以外をカウント
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end() - 1; ++unit) {
			count += bit_util::popcount(*unit);
		}

		// 左端をカウント
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = bit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		count += bit_util::popcount(left_unit);
		return count;
	}

	bool test(const size_t pos) const {
		auto [byteidx, bitidx] = conv_seqidx2bytebitidx(pos);

		Unit mask = bit_util::get_single_bit<uint64_t>(bitidx);
		return this->bitvec_[byteidx] & mask;
	}

	bool all() const {
		// 左端以外を検証
		Unit fill = std::numeric_limits<Unit>::max();
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end() - 1; ++unit) {
			if (*unit != fill) return false;
		}

		// 左端を検証
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = bit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		fill = bit_util::get_fill_bit<Unit>(0, bitidx + 1);

		return left_unit == fill;
	}

	bool any() const {
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end(); ++unit) {
			if (*unit != 0) return true;
		}
		return false;
	}

	/*------------------------------ 型変換 ------------------------------*/

	std::string to_string() const {
		std::string str = "";

		// 左端を処理
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = bit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		for (auto i = bitidx; i >= 0; --i) {
			if (left_unit & bit_util::get_single_bit<Unit>(i)) str += "1";
			else str += "0";
		}

		// 左端以外を処理
		for (auto unit = this->bitvec_.rbegin() + 1; unit < this->bitvec_.rend(); ++unit) {
			str += std::bitset<unit_size_>(*unit).to_string();
		}

		return str;
	}


	/*------------------------------ メンバ変数 -------------------------------*/
protected:
	// ビット列の本体
	std::vector<Unit> bitvec_;

	// unitが何ビット整数か
	static constexpr size_t unit_size_ = sizeof(Unit) * CHAR_BIT;

	// 有効ビット長さ
	int size_;

	/*------------------------------ テスト用 ------------------------------*/
public:
	std::vector<Unit> bitvec() const { return this->bitvec_; }

	void show() const {
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end(); ++unit) bit_util::show_bin_int64(*unit);
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