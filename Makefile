INCLUDE_DIR = ./include
SOURCE_DIR = ./source
OBJECT_DIR = ./object

CC = g++
CFLAGS=-g -I$(INCLUDE_DIR)
OUT = cluster

_DEPS = CosineSimilarity.hpp ErrorCodes.hpp HashTable.hpp HypercubeEuclidean.hpp LSHEuclidean.hpp Metric.hpp myvector.hpp utility.hpp WriteOutput.hpp ParsingCSV.hpp ReadInput.hpp
#pattern matching from  _DEPS to include directory
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = CosineSimilarity.o HashTable.o HypercubeEuclidean.o LSHEuclidean.o main.o Metric.o myvector.o utility.o WriteOutput.o ReadInput.o
#same pattern matching principe
OBJ = $(patsubst %,$(OBJECT_DIR)/%,$(_OBJ))

.PHONY: clean

############goals#######################

#general rule for all object files
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#default goal
$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

count:
	wc $(SOURCE_DIR)/*.cpp $(DEPS)

clean:
	rm -f $(OBJECT_DIR)/*.o ./cluster
