

#include <vector>
#include <bitset>

using std::vector;
using std::bitset;

/*---------- ”z—ñ ----------*/

template <typename T>
void show_vec(const vector<T>& vec) {
	for (auto& e : vec) std::cout << bitset<64>(e) << ",";
	std::cout << std::endl;
}