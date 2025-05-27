TARGETS = copier mtcopier
COPIER = ./copier_files/main.cpp ./copier_files/reader.cpp ./copier_files/writer.cpp
MTCOPIER = ./mtcopier_files/main.cpp ./mtcopier_files/reader.cpp ./mtcopier_files/writer.cpp

CXXFLAGS = -Wall -Werror -std=c++20 -O -o

all: $(TARGETS)

copier:    $(COPIER)
	g++ $(CXXFLAGS) copier $(COPIER)

mtcopier:    $(MTCOPIER)
	g++ $(CXXFLAGS) mtcopier $(MTCOPIER) -pthread

clean:
	rm copier mtcopier