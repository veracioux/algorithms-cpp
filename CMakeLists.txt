cmake_minimum_required(VERSION 3.13)
project(AlgorithmsCPP)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp numeric/Numeric.cpp test/McCluskeyTest.cpp test/McCluskeyTest.h logic/Implicant.cpp logic/Implicant.h logic/McCluskey.cpp logic/McCluskey.h numeric/NumericGeneral.cpp numeric/NumericGeneral.h symbols.h logic/symbols.h programs/McCluskeyProg.h programs/McCluskeyProg.cpp logic/VeitchDiagram.cpp logic/VeitchDiagram.h test/VeitchDiagramTest.h test/VeitchDiagramTest.cpp programs/Program.h programs/Program.cpp)
add_executable(AlgorithmsCPP ${SOURCE_FILES})