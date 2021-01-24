#TO BE CONTINUE

CXX=g++
CXXFLAGS=-Wall -std=c++11 -O2 -march=native

all:main

SRC=./
Main_Name=main_v1.cpp

main:$(SRC)/My_Range_Encoder.o $(SRC)/My_Range_Decoder.o $(SRC)/main.o
	@$(CXX) $(CXXFLAGS) -o main $(SRC)/My_Range_Encoder.o $(SRC)/My_Range_Decoder.o $(SRC)/main.o
	@-rm $(SRC)/*.o

$(SRC)/My_Range_Encoder.o:$(SRC)/My_Range_Encoder.cpp $(SRC)/My_Range_Coder.h
	@$(CXX) $(CXXFLAGS) -c $(SRC)/My_Range_Encoder.cpp -o $(SRC)/My_Range_Encoder.o

$(SRC)/My_Range_Decoder.o:$(SRC)/My_Range_Decoder.cpp $(SRC)/My_Range_Coder.h
	@$(CXX) $(CXXFLAGS) -c $(SRC)/My_Range_Decoder.cpp -o $(SRC)/My_Range_Decoder.o

$(SRC)/main.o:$(SRC)/$(Main_Name) $(SRC)/My_Range_Coder.h
	@$(CXX) $(CXXFLAGS) -c $(SRC)/$(Main_Name) -o $(SRC)/main.o

clean:
	-@rm main
