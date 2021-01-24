
# C++ 接口

    需要文件：Makefile, main_v0.cpp, main_v1.cpp, My_Range_Coder.h, My_Range_Encoder.cpp, My_Range_Decoder.cpp.
    更改Makefile中的Main_Name可更改主文件
    执行方式：
        make
        ./main

# Python 接口

    需要文件：Test.py, Test_Cdf.py, AE.cpp, setup.py, My_Range_Coder.h, My_Range_Encoder.cpp, My_Range_Decoder.cpp.
    注意事项：需要将AE.cpp中的Python.h路径更改为本地主机上的路径，如/usr/include/python3.6m/Python.h等
    执行方式：
        python setup.py build
        python setup.py install
        python Test.py
        python Test_Cdf.py

# 实际应用示例

    需要文件：encode.py, decode.py...
    注意事项：需要在编译Python接口成功后进行，需要更改两个文件中的模型读取路径、图片读取路径、二进制文件存储路径
    验证方式：
        python l-one-sim.py 得到理论bpp、MS-SSIM
        python encode.py 进行压缩
        python decode.py 进行解压，若MS-SSIM及生成的*.bin文件大小符合l-one-sim.py的结果，证明算术解码成功
