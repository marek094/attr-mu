CXXFLAGS = -Wall -std=c++14

.PHONY: clean

clean:
	find . | sed -e 's/\.cpp//' | sort | uniq -d | tee /dev/stderr | xargs rm
