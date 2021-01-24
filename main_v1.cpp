/**************************************************
	> File Name:  fuck.cpp
	> Author:     Leuckart
	> Time:       2019-10-22 02:31
**************************************************/

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <ctime>
#include <string>
#include "My_Range_Coder.h"

template <typename T>
void Generate_Random(std::vector<T> &vec, const T mean, const T scale, bool bePositive = false)
{
	std::knuth_b eng(time(0));
	std::normal_distribution<double_t> dist(mean, scale);
	if (bePositive)
		std::for_each(vec.begin(), vec.end(), [&dist, &eng](T &it) { it = abs(dist(eng)); });
	else
		std::for_each(vec.begin(), vec.end(), [&dist, &eng](T &it) { it = dist(eng); });
}

void Main_Encode(const std::vector<int32_t> &data,
				 const std::vector<double_t> &mean,
				 const std::vector<double_t> &scale,
				 const int32_t data_min,
				 const int32_t data_max,
				 const std::string filename = "./compressed.bin")
{
	std::cout << " Data  Info ===>  Min: " << data_min << " ;  Max: " << data_max << std::endl;
	const uint32_t data_size = data.size();
	const uint8_t range_size = 1 + data_max - data_min;

	RangeEncoder arithmetic_encoder;
	arithmetic_encoder.Init(filename);
	const uint16_t factor = (1 << 16) - range_size;

	for (size_t i = 0; i < data_size; ++i)
	{
		const std::uint8_t cur_data = data[i] - data_min;
		const double_t cur_mean = mean[i] - data_min, cur_scale = scale[i];

		const uint32_t x_0 = normalCDF((cur_data - 0.5 - cur_mean) / cur_scale) * factor + cur_data;
		const uint32_t x_1 = normalCDF((cur_data + 0.5 - cur_mean) / cur_scale) * factor + cur_data + 1;
		arithmetic_encoder.Encode(x_0, x_1);
	}
	arithmetic_encoder.Finalize();
}

void Main_Decode(std::vector<int32_t> &recons,
				 const std::vector<double_t> &mean,
				 const std::vector<double_t> &scale,
				 const int32_t data_min,
				 const int32_t data_max,
				 const std::string filename = "./compressed.bin")
{
	const uint32_t data_size = mean.size();

	RangeDecoder arithmetic_decoder;
	arithmetic_decoder.Init(filename, data_min, data_max);
	for (size_t i = 0; i < data_size; ++i)
		recons.push_back(arithmetic_decoder.Decode(mean[i], scale[i]));
}

void Hyper_Encode(const std::vector<int32_t> &data,
				  const std::vector<std::vector<uint32_t>> &cdf,
				  const int32_t data_min,
				  const int32_t data_max,
				  const std::string filename = "./compressed.bin")
{
	std::cout << " Data  Info ===>  Min: " << data_min << " ;  Max: " << data_max << std::endl;
	const uint32_t data_size = data.size();

	RangeEncoder arithmetic_encoder;
	arithmetic_encoder.Init(filename);

	for (size_t i = 0; i < data_size; ++i)
		arithmetic_encoder.Encode_Cdf(data[i] - data_min, cdf[i]);
	arithmetic_encoder.Finalize();
}

void Hyper_Decode(std::vector<int32_t> &recons,
				  const std::vector<std::vector<uint32_t>> &cdf,
				  const int32_t data_min,
				  const int32_t data_max,
				  const std::string filename = "./compressed.bin")
{
	const uint32_t data_size = cdf.size();
	//const uint32_t range_size = data_max - data_min + 1;

	RangeDecoder arithmetic_decoder;
	arithmetic_decoder.Init(filename, data_min, data_max);
	for (size_t i = 0; i < data_size; ++i)
		recons.push_back(arithmetic_decoder.Decode_Cdf(cdf[i]));
}

int main()
{
	/*============================== Setting ==============================*/
	const bool encode_main = 0;
	const uint32_t data_size = 525000;
	const double _mean = 22.5, _scale = 0.5;

	/*============================== Generating ==============================*/
	std::vector<double_t> Means(data_size);
	Generate_Random<double_t>(Means, _mean, _scale);
	std::cout << " Mean  Info ===>  Min: " << *min_element(Means.cbegin(), Means.cend()) << " ;  Max: " << *max_element(Means.cbegin(), Means.cend()) << std::endl;

	std::vector<double_t> Scales(data_size);
	Generate_Random<double_t>(Scales, _scale, _scale, true);
	std::cout << " Scale Info ===>  Min: " << *min_element(Scales.cbegin(), Scales.cend()) << " ;  Max: " << *max_element(Scales.cbegin(), Scales.cend()) << std::endl;

	std::vector<std::int32_t> Datas(data_size);
	for (size_t i = 0; i < data_size; ++i)
	{
		std::knuth_b Eng_D(time(0));
		std::normal_distribution<double_t> Dist_D(Means[i], Scales[i]);
		Datas[i] = round(Dist_D(Eng_D));
	}

	/*============================== Information ==============================*/
	const int32_t data_min = *min_element(Datas.cbegin(), Datas.cend());
	const int32_t data_max = *max_element(Datas.cbegin(), Datas.cend());
	const uint8_t range_size = 1 + data_max - data_min;

	std::vector<std::vector<uint32_t>> Cdf;
	for (size_t i = 0; i < data_size; ++i)
	{
		std::vector<uint32_t> _cdf(range_size + 1);
		for (size_t j = 0; j <= range_size; ++j)
			_cdf[j] = j + normalCDF((j - 0.5 + data_min - Means[i]) / Scales[i]) * ((1 << 16) - range_size);
		Cdf.push_back(_cdf);
	}

	/*============================== Encoding ==============================*/
	const std::clock_t enc_beg = clock();
	if (encode_main)
		Main_Encode(Datas, Means, Scales, data_min, data_max, "./compressed.bin");
	else
		Hyper_Encode(Datas, Cdf, data_min, data_max, "./compressed.bin");
	const std::clock_t enc_end = clock();

	/*============================== Decoding ==============================*/
	const std::clock_t dec_beg = clock();
	std::vector<int32_t> recons;
	if (encode_main)
		Main_Decode(recons, Means, Scales, data_min, data_max, "./compressed.bin");
	else
		Hyper_Decode(recons, Cdf, data_min, data_max, "./compressed.bin");
	const std::clock_t dec_end = clock();

	/*============================== Checking ==============================*/
	int error_number = 0;
	for (size_t i = 0; i < data_size; ++i)
		if (Datas[i] != recons[i])
			error_number++;

	std::cout << "Error Number: " << error_number << std::endl;
	std::cout << "Encoding Time: " << (double)(enc_end - enc_beg) / CLOCKS_PER_SEC << std::endl;
	std::cout << "Decoding Time: " << (double)(dec_end - dec_beg) / CLOCKS_PER_SEC << std::endl;
	std::cout << "Total Time: " << (double)(enc_end + dec_end - enc_beg - dec_beg) / CLOCKS_PER_SEC << std::endl;
	return 0;
}
