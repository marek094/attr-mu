CXXFLAGS = -Wall -std=c++1y

.PHONY: clean

clean:
	find . | sed -e 's/\.cpp//' | sort | uniq -d | tee /dev/stderr | xargs rm

