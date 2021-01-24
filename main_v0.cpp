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
#include "My_Range_Coder.h"

int main()
{
	const double_t Mean = 22.5, Scale = 0.5;

	// ================================================== //
	std::knuth_b Eng(time(0));
	std::normal_distribution<double_t> Dist(Mean, Scale);
	std::vector<int32_t> Data(525000);
	std::for_each(Data.begin(), Data.end(), [&Dist, &Eng](int32_t &it) { it = round(Dist(Eng)); });
	Data[1000] = 10;
	const std::vector<int32_t> Data_Label = Data;

	// ================================================== //
	const int32_t data_min = *min_element(Data_Label.cbegin(), Data_Label.cend());
	const int32_t data_max = *max_element(Data_Label.cbegin(), Data_Label.cend());
	const uint8_t range_size = 1 + data_max - data_min;
	std::cout << " ===>  Min: " << data_min << " ;  Max: " << data_max << std::endl;

	std::for_each(Data.begin(), Data.end(), [data_min](int32_t &it) { it -= data_min; });

	// ================================================== //
	std::clock_t enc_beg = clock();
	RangeEncoder arithmetic_encoder;
	arithmetic_encoder.Init("./compressed.bin");

	uint16_t factor = (1 << 16) - range_size;
	for (const std::uint8_t _tmp : Data)
	{
		uint32_t x_0 = normalCDF((_tmp - 0.5 - Mean + data_min) / Scale) * factor + _tmp;
		uint32_t x_1 = normalCDF((_tmp + 0.5 - Mean + data_min) / Scale) * factor + _tmp + 1;
		arithmetic_encoder.Encode(x_0, x_1);
	}
	arithmetic_encoder.Finalize();
	std::clock_t enc_end = clock();

	// ================================================== //
	std::clock_t dec_beg = clock();
	RangeDecoder arithmetic_decoder;
	arithmetic_decoder.Init("./compressed.bin", data_min, data_max);

	std::vector<uint8_t> recons;
	for (size_t i = 0; i < Data.size(); ++i)
	{
		recons.push_back(arithmetic_decoder.Decode(Mean, Scale));
	}
	std::clock_t dec_end = clock();

	// ================================================== //
	int error_number = 0;
	for (size_t i = 0; i < Data_Label.size(); ++i)
		if (Data_Label[i] != recons[i])
			error_number++;
	std::cout << "Error Number: " << error_number << std::endl;

	std::cout << "Encoding Time: " << (double)(enc_end - enc_beg) / CLOCKS_PER_SEC << std::endl;
	std::cout << "Decoding Time: " << (double)(dec_end - dec_beg) / CLOCKS_PER_SEC << std::endl;
	std::cout << "Total Time: " << (double)(enc_end + dec_end - enc_beg - dec_beg) / CLOCKS_PER_SEC << std::endl;
	return 0;
}
