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

	/*--------------------- 表示系 --------------------*/

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

	/*-------------------- カウント系 --------------------*/

	int popcount(uint64_t x) {
		int c = 0;
		while (x) {
			x &= (x - 1);  // 1ビット消す
			++c;
		}
		return c;
	}

	/*-------------------- 生成系 --------------------*/

	template <typename Unit>
	Unit get_single_bit(const size_t pos) {
		// posの位置のみ1の Unit を生成

		return Unit(1) << pos;
	}

	template <typename Unit>
	Unit make_seqmask(const size_t begin, const size_t end) {
		// [begin, end) が1でそれ以外が0のビット列を生成

		size_t size = sizeof(Unit) * CHAR_BIT;
		Unit mask = std::numeric_limits<Unit>::max();
		mask <<= size - end;
		mask >>= ((size - end - 1) + begin + 1);
		mask <<= begin;

		return mask;
	}

	template <typename Unit>
	Unit make_Unit(std::string x) {
		// ビットの文字列表現を Unitに変換

		Unit ret = Unit(0);

		for (auto& bit : x) {
			ret <<= 1;
			if (bit == '1') ret += 1;
		}

		return ret;
	}


	/*-------------------- 操作系 --------------------*/

	template <typename Unit>
	Unit bitfield(Unit x, int begin, int end) {
		// x の beginからendのびっとを抽出し，範囲外を 0 埋めしたものを生成

		// マスク作成
		Unit mask = make_seqmask<Unit>(begin, end);

		// 抽出
		return x & mask;
	}

	template <typename Unit>
	std::pair<Unit, Unit> push_left(Unit unit, std::string push_val = '0') {
		// unit に push_val をleft push shiftしたものと，溢れたものを返す

		// aft_push: push操作後のunit
		// overwhelm: 溢れた値
		Unit aft_push(unit);
		Unit overwhelm(unit);

		// aft_push計算
		aft_push <<= push_val.size();
		aft_push |= make_Unit<Unit>(push_val);

		// overwhelm計算
		size_t unit_size = sizeof(Unit) * CHAR_BIT;
		overwhelm >>= (unit_size - push_val.size());

		return std::make_pair(aft_push, overwhelm);
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
		// 左端以外のUnitの代入
		auto i = 0;
		for (i = 0; i < this->bitvec_.size() - 1; ++i) {
			this->bitvec_[i] = Unit_util::make_Unit<Unit>(str_bit.substr(str_bit.size() - (unit_size_ * (i + 1)), unit_size_));
		}

		// 左端のUnitの代入
		std::cout << str_bit.substr(0, str_bit.size() - (unit_size_ * i) + 1) << std::endl;
		this->bitvec_[i] = Unit_util::make_Unit<Unit>(str_bit.substr(0, str_bit.size() - (unit_size_ * i)));
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


	/*--------------- シフト ---------------*/
	// シフトには以下の3種類がある
	// shift: 通常シフト．ビット列の長さを維持しつつ，溢れた分は0で埋める
	// cycle_shift: 循環シフト．ビット列の長さを維持しつつ，循環シフト．
	// push_shift: 左シフトでは，シフトした分ビット列の長さが増える．増えた分は設定した値で埋める．
	//             e.g. bv(101).push_shift_left(010) -> bv(101010)
	//                  bv(101).push_shift_right(01) -> bv(011)
	// シフト演算子は通常シフトに対応させる．

	/*---------- 通常シフト ----------*/

	BitVector<Unit> shl(const size_t sh_pos) const {
		// sh_pos回 左シフト

		return shl(std::string(sh_pos, '0'));
	}

	BitVector<Unit> shl(std::string push_val=0) const {
		// 左シフト後，空いた部分を push_val で埋める
		BitVector<Unit> sh_bv(*this);

		// unit_shift_count: Unitレベルのシフト回数
		// bit_shift_count: ビットレベルのシフト回数
		const int shift_count = push_val.size();
		const int unit_shift_count = shift_count / unit_size_;
		const int bit_shift_count = shift_count % unit_size_;

		//----- Unitレベルのシフト
		for (auto i = this->bitvec_.size() - unit_shift_count - 1; i >= 0; --i) {
			this->bitvec_[i + unit_shift_count] = this->bitvec_[i];
		}

		//----- bit レベルのシフト
		// 左端の演算
		auto [pushed, overwhelm] = Unit_util::push_left(this->bitvec_.back());
		this->bitvec_.back() = pushed;

		// 左端以外の演算
		for (auto i = this->bitvec_.size() - 2; i >= unit_shift_count; --i) {
			auto [pushed, overwhelm] = Unit_util::push_left(this->bitvec_[i]);
			this->bitvec_[i] = pushed;
			this->bitvec_[i] |= overwhelm;
		}

		// push_val で埋める


	}

	BitVector<Unit> shr(const size_t sh_pos) const {
		return shr(std::string(sh_pos, '0'));

	}

	BitVector<Unit> shr(std::string push_val = "0") const {
		return NULL;
	}

	BitVector<Unit> operator<<(const size_t pos) const {
		// 未実装 <<=実装以降
	}

	BitVector<Unit> operator>>(const size_t pos) const {
		// 未実装 >>= 実装以降
	}

	/*---------- 循環シフト ----------*/
	BitVector<Unit> cshl(const size_t sh_pos) const {

	}

	BitVector<Unit> cshr(const size_t sh_pos) const {

	}

	/*---------- 押し出しシフト ----------*/
	BitVector<Unit> pshl(const size_t sh_pos) const {

	}

	BitVector<Unit> pshr(const size_t sh_pos) const {

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
			count += Unit_util::popcount(*unit);
		}

		// 左端をカウント
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
		// 左端以外を検証
		Unit fill = std::numeric_limits<Unit>::max();
		for (auto unit = this->bitvec_.begin(); unit < this->bitvec_.end() - 1; ++unit) {
			if (*unit != fill) return false;
		}

		// 左端を検証
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

	/*------------------------------ 型変換 ------------------------------*/

	std::string to_string() const {
		std::string str = "";

		// 左端を処理
		int bitidx = get_bitidx_from_seqidx(size_ - 1);
		Unit left_unit = Unit_util::bitfield(this->bitvec_.back(), 0, bitidx + 1);
		for (auto i = bitidx; i >= 0; --i) {
			if (left_unit & Unit_util::get_single_bit<Unit>(i)) str += "1";
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