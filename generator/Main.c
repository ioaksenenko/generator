#include <stdio.h>
#include <Windows.h>
#include <locale.h>


typedef struct {
	unsigned long long int number;
	unsigned long long int count;
	float probability;
} element_t;


typedef struct {
	element_t* elements;
	unsigned long long int length;
} sequence_t;


unsigned long long int lcg(unsigned long long int x, unsigned long long int a, unsigned long long int c, unsigned long long int m) {
	return (x * a + c) % m;
}


sequence_t generate(unsigned long long int x, unsigned long long int a, unsigned long long int c, unsigned long long int m) {
	sequence_t res = (sequence_t){
		.elements = (element_t*)malloc((m + 1) * sizeof(element_t)),
		.length = 0
	};
	res.elements[res.length] = (element_t){
		.number = x,
		.count = 0,
		.probability = 0
	};
	do {
		res.elements[++res.length] = (element_t){
			.number = lcg(res.elements[res.length].number, a, c, m),
			.count = 0,
			.probability = 0
		};
	} while (res.elements[res.length].number != res.elements[0].number);
	return res;
}


unsigned long long int period(sequence_t seq) {
	return seq.length;
}


unsigned long long int index(unsigned long long int number, sequence_t sequence) {
	for (unsigned long long int i = 0; i < sequence.length; i++) {
		if (sequence.elements[i].number == number) {
			return i;
		}
	}
	return -1;
}


sequence_t calc_probabilities(sequence_t seq) {
	sequence_t res = (sequence_t){
		.elements = (element_t*)malloc(seq.length * sizeof(element_t)),
		.length = 0
	};
	for (unsigned long long int i = 0; i < seq.length; i++) {
		unsigned long long int idx = index(seq.elements[i].number, res);
		if (idx != -1) {
			res.elements[idx].count++;
		} else {
			res.elements[res.length++] = (element_t){
				.number = seq.elements[i].number,
				.count = 1,
				.probability = 0
			};
		}
	}
	for (unsigned long long int i = 0; i < res.length; i++) {
		res.elements[i].probability = (float)res.elements[i].count / seq.length * 100;
		for (unsigned long long int i = 0; i < seq.length; i++) {
			if (seq.elements[i].number == res.elements[i].number) {
				seq.elements[i].count = res.elements[i].count;
				seq.elements[i].probability = res.elements[i].probability;
			}
		}
	}
	return res;
}


int main() {
	setlocale(LC_ALL, "Russian");

	unsigned long long int a = 16777325;
	unsigned long long int c = 3;
	// unsigned long long int m = 4294967296;
	unsigned long long int m = 1024;
	// 18446744073709551616

	unsigned long long int x = 0;

	sequence_t seq = generate(x, a, c, m);
	/*for (unsigned long long int i = 0; i < seq.length; i++) {
		printf("%llu\n", seq.elements[i].number);
	}*/
	unsigned long long int k = period(seq);
	// printf("Период: %llu\n", k);
	sequence_t uniq = calc_probabilities(seq);
	/*for (unsigned long long int i = 0; i < uniq.length; i++) {
		printf("Число: %llu\nЧастота: %llu\nВероятность: %f %%\n", uniq.elements[i].number, uniq.elements[i].count, uniq.elements[i].probability);
	}*/
	for (unsigned long long int i = 0; i < seq.length; i++) {
		printf("Число: %llu\nЧастота: %llu\nВероятность: %f %%\n", seq.elements[i].number, seq.elements[i].count, seq.elements[i].probability);
	}

	return 0;
}