all:
	./build.sh
	./run.sh ./tests/input1.txt ./tests/output_1.txt
	./run.sh ./tests/input2.txt ./tests/output_2.txt
	./run.sh ./tests/input3.txt ./tests/output_3.txt
	diff ./tests/output1.txt ./tests/output_1.txt
	diff ./tests/output2.txt ./tests/output_2.txt
	diff ./tests/output3.txt ./tests/output_3.txt

clean:
	rm -rf *.class
	rm ./tests/output_1.txt
	rm ./tests/output_2.txt
	rm ./tests/output_3.txt
