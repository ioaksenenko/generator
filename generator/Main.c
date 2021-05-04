#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

const float QUANTILIES[31][7] = {
	{0.01, 0.025, 0.05, 0.95, 0.975, 0.99},
	{6.6, 5.0, 3.8, 0.0039, 0.00098, 0.00016},
	{9.2, 7.4, 6.0, 0.103, 0.051, 0.020},
	{11.3, 9.4, 7.8, 0.352, 0.216, 0.115},
	{13.3, 11.1, 9.5, 0.711, 0.484, 0.297},
	{15.1, 12.8, 11.1, 1.15, 0.831, 0.554},
	{16.8, 14.4, 12.6, 1.64, 1.24, 0.872},
	{18.5, 16.0, 14.1, 2.17, 1.69, 1.24},
	{20.1, 17.5, 15.5, 2.73, 2.18, 1.65},
	{21.7, 19.0, 16.9, 3.33, 2.70, 2.09},
	{23.2, 20.5, 18.3, 3.94, 3.25, 2.56},
	{24.7, 21.9, 19.7, 4.57, 3.82, 3.05},
	{26.2, 23.3, 21.0, 5.23, 4.40, 3.57},
	{27.7, 24.7, 22.4, 5.89, 5.01, 4.11},
	{29.1, 26.1, 23.7, 6.57, 5.63, 4.66},
	{30.6, 27.5, 25.0, 7.26, 6.26, 5.23},
	{32.0, 28.8, 26.3, 7.96, 6.91, 5.81},
	{33.4, 30.2, 27.6, 8.67, 7.56, 6.41},
	{34.8, 31.5, 28.9, 9.39, 8.23, 7.01},
	{36.2, 32.9, 30.1, 10.1, 8.91, 7.63},
	{37.6, 34.2, 31.4, 10.9, 9.59, 8.26},
	{38.9, 35.5, 32.7, 11.6, 10.3, 8.26},
	{40.3, 36.8, 33.9, 12.3, 11.0, 9.54},
	{41.6, 38.1, 35.2, 13.1, 11.7, 10.2},
	{43.0, 39.4, 36.4, 13.8, 12.4, 10.9},
	{44.3, 40.6, 37.7, 14.6, 13.1, 11.5},
	{45.6, 41.9, 38.9, 15.4, 13.8, 12.2},
	{47.0, 43.3, 40.1, 16.2, 14.6, 12.9},
	{48.3, 44.5, 41.3, 16.9, 15.3, 13.6},
	{49.6, 45.7, 42.6, 17.7, 16.0, 14.3},
	{50.9, 47.0, 43.8, 18.5, 16.8, 15.0}
};


typedef struct binary_t {
	int* bits;
	unsigned long long int length;
} binary_t;


typedef struct element_t {
	unsigned long long int number; // число в десятичном виде
	unsigned long long int frequency; // частость числа в последовательности
	float probability; // вероятность числа в последовательности
	binary_t binary_number; // число в бинарном виде
} element_t;


typedef struct sequence_t {
	element_t* elements;
	unsigned long long int length;
} sequence_t;


unsigned long long int lcg(unsigned long long int x, unsigned long long int a, unsigned long long int c, unsigned long long int m) {
	return (x * a + c) % m;
}


binary_t to_binary(unsigned long long int number) {
	unsigned long long int length = ceil(log2(number + 1));
	binary_t res = (binary_t){
		.bits = (int*)malloc((length) * sizeof(int)),
		.length = length
	};
	for (int i = length - 1; i >= 0; i--) {
		res.bits[i] = number % 2;
		number = floor(number / 2);
	}
	return res;
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
			res.elements[idx].frequency++;
		}
		else {
			res.elements[res.length++] = (element_t){
				.number = seq.elements[i].number,
				.frequency = 1,
				.probability = 0,
				.binary_number = seq.elements[i].binary_number
			};
		}
	}
	for (unsigned long long int i = 0; i < res.length; i++) {
		res.elements[i].probability = (float)res.elements[i].frequency / seq.length * 100;
		for (unsigned long long int i = 0; i < seq.length; i++) {
			if (seq.elements[i].number == res.elements[i].number) {
				seq.elements[i].frequency = res.elements[i].frequency;
				seq.elements[i].probability = res.elements[i].probability;
			}
		}
	}
	return res;
}


unsigned long long int period(unsigned long long int x, unsigned long long int a, unsigned long long int c, unsigned long long int m) {
	unsigned long long int res = 0;
	unsigned long long int next = x;
	do {
		next = lcg(next, a, c, m);
		res++;
	} while (next != x);
	return res;
}


unsigned long long int  max_number_length(sequence_t seq) {
	unsigned long long int res = 0;
	for (unsigned long long int i = 0; i < seq.length; i++) {
		if (res < seq.elements[i].binary_number.length) {
			res = seq.elements[i].binary_number.length;
		}
	}
	return res;
}


void aligning_number_length(sequence_t seq) {
	unsigned long long int length = max_number_length(seq);
	for (unsigned long long int i = 0; i < seq.length; i++) {
		binary_t binary_number = (binary_t){
			.bits = (int*)malloc((length) * sizeof(int)),
			.length = length
		};
		for (int j = binary_number.length - 1, k = seq.elements[i].binary_number.length - 1; j >= 0; j--, k--) {
			binary_number.bits[j] = k >= 0 ? seq.elements[i].binary_number.bits[k] : 0;
		}
		seq.elements[i].binary_number = binary_number;
	}
}


sequence_t generate(unsigned long long int x, unsigned long long int a, unsigned long long int c, unsigned long long int m) {
	unsigned long long int length = period(x, a, c, m);
	sequence_t res = (sequence_t){
		.elements = (element_t*)malloc(length * sizeof(element_t)),
		.length = length
	};
	res.elements[0] = (element_t){
		.number = x,
		.frequency = 0,
		.probability = 0,
		.binary_number = to_binary(x)
	};
	for (int i = 1; i < length; i++) {
		unsigned long long int number = lcg(res.elements[i - 1].number, a, c, m);
		res.elements[i] = (element_t){
			.number = number,
			.frequency = 0,
			.probability = 0.0,
			.binary_number = to_binary(number)
		};
	};
	calc_probabilities(res);
	aligning_number_length(res);
	return res;
}


void print_sequence(sequence_t seq) {
	printf("Десятичное число\tДвоичное число\tЧастота\tВероятность\n");
	for (unsigned long long int i = 0; i < seq.length; i++) {
		printf("%llu\t", seq.elements[i].number);
		for (unsigned long long int j = 0; j < seq.elements[i].binary_number.length; j++) {
			printf("%d", seq.elements[i].binary_number.bits[j]);
		}
		printf("\t%llu\t%f\n", seq.elements[i].frequency, seq.elements[i].probability);
	}
}


int test_period(sequence_t seq, unsigned long long int m) {
	return seq.length == m;
}


int test_dispersion(sequence_t seq) {
	for (int i = 1; i < seq.length; i++) {
		if (seq.elements[i].frequency != seq.elements[0].frequency) {
			return 0;
		}
	}
	return 1;
}


int test_lsb(sequence_t seq, float significance) {
	for (int i = 0; i < seq.length; i++) {
		int units_number = 0;
		int zeros_number = 0;
		int bytes_numer = ceil(seq.elements[i].binary_number.length / 8.0);
		int checked_bits_numer = bytes_numer / 2.0 * 8;
		for (int j = seq.elements[i].binary_number.length - checked_bits_numer; j < seq.elements[i].binary_number.length; j++) {
			units_number += seq.elements[i].binary_number.bits[j];
			zeros_number += !seq.elements[i].binary_number.bits[j];
		}
		// printf("Нулей: %d, Единиц: %d, Отклонение: %f\n", zeros_number, units_number, abs(units_number - zeros_number) * 1.0 / checked_bits_numer);
		if (abs(units_number - zeros_number) * 1.0 / checked_bits_numer > significance) {
			return 0;
		}
	}
	return 1;
}


int test_pearson(sequence_t seq, int intervals_number, float significance) {
	int min_number = seq.elements[0].number;
	int max_number = seq.elements[0].number;
	for (int i = 0; i < seq.length; i++) {
		if (seq.elements[i].number < min_number) {
			min_number = seq.elements[i].number;
		}
		if (seq.elements[i].number > max_number) {
			max_number = seq.elements[i].number;
		}
	}
	// printf("Левая граница: %d\n", min_number);
	// printf("Правая граница: %d\n", max_number);
	float step = (max_number - min_number) * 1.0 / intervals_number;
	// printf("Шаг: %f\n", step);
	// вычисляем эмпирические частоты попадания числа в интервал
	int* frequencies = (int*)malloc((intervals_number) * sizeof(int));
	for (int i = 0; i < intervals_number; i++) {
		frequencies[i] = 0;
	}
	for (int i = 0; i < seq.length; i++) {
		for (int j = 0; j < intervals_number; j++) {
			if (seq.elements[i].number >= min_number + j * step && seq.elements[i].number < min_number + (j + 1) * step) {
				frequencies[j]++;
			}
		}
	}
	// теоретическая частота попадания в каждый интервал
	// поскольку равномерный закон распределения
	float theoretical_frequencу = seq.length * 1.0 / intervals_number;
	float stat = 0.0;
	for (int i = 0; i < intervals_number; i++) {
		stat += pow(frequencies[i] - theoretical_frequencу, 2) / theoretical_frequencу;
	}
	int freedom_degrees = 0;
	int k = intervals_number - freedom_degrees - 1;
	float quantile = 0.0;
	for (int i = 0; i < 7; i++) {
		if (fabs(QUANTILIES[0][i] - significance) < 1E-10) {
			quantile = QUANTILIES[k][i];
		}
	}
	if (fabs(quantile - 0.0) < 1E-10) {
		printf("Заданный уровень значимости не поддерживается.\nВозможные варианты: 0.01, 0.025, 0.05, 0.95, 0.975, 0.99.\n");
		exit(EXIT_FAILURE);
	}
	return stat <= quantile;
}


binary_t to_binary_sequence(sequence_t seq) {
	unsigned long long int length = 0;
	for (int i = 0; i < seq.length; i++) {
		length += seq.elements[i].binary_number.length;
	}
	binary_t res = (binary_t){
		.bits = (int*)malloc(length * sizeof(int)),
		.length = length
	};
	int index = 0;
	for (int i = 0; i < seq.length; i++) {
		for (int j = 0; j < seq.elements[i].binary_number.length; j++) {
			res.bits[index++] = seq.elements[i].binary_number.bits[j];
		}
	}
	return res;
}


int test_unlinked_series_method(sequence_t seq, float significance) {
	binary_t bin_seq = to_binary_sequence(seq);
	for (int i = 1; i < floor(log2(bin_seq.length)); i++) {
		int size = pow(2, i);
		int* n = (int*)malloc(size * sizeof(int));
		for (int j = 0; j < size; j++) {
			n[j] = 0;
		}
		int k = 0;
		while (k < floor(bin_seq.length * 1.0 / i)) {
			int j = 0;
			int tmp = 1;
			int sum = 0;
			while (j < i) {
				sum += tmp * bin_seq.bits[k * i + j];
				tmp *= 2;
				j++;
			}
			n[sum]++;
			k++;
		}
		float stat = 0.0;
		float theoretical_frequencу = floor(bin_seq.length * 1.0 / i) * 1.0 / size;
		for (int j = 0; j < size; j++) {
			stat += pow(n[j] - theoretical_frequencу, 2) / theoretical_frequencу;
		}
		int params_num = 0;
		int freedom_degrees = size - params_num - 1;
		float quantile = 0.0;
		if (freedom_degrees <= 30) {
			for (int i = 0; i < 7; i++) {
				if (fabs(QUANTILIES[0][i] - significance) < 1E-10) {
					quantile = QUANTILIES[freedom_degrees][i];
				}
			}
			if (fabs(quantile - 0.0) < 1E-10) {
				printf("Заданный уровень значимости не поддерживается.\nВозможные варианты: 0.01, 0.025, 0.05, 0.95, 0.975, 0.99.\n");
				exit(EXIT_FAILURE);
			}
		} else {
			return 1;
		}
		if (stat > quantile) {
			return 0;
		}
	}
	return 1;
}


int main() {
	setlocale(LC_ALL, "Russian");

	unsigned long long int a = 1103515245;
	unsigned long long int c = 12345;
	// unsigned long long int m = 4294967296;
	unsigned long long int m = 65536;
	// 18446744073709551616

	srand(time(NULL));
	unsigned long long int x = rand() % m;
	printf("%llu\n", x);

	sequence_t seq = generate(x, a, c, m);
	/*printf("\n");
	binary_t bin_seq = to_binary_sequence(seq);
	for (int i = 0; i < bin_seq.length; i++) {
		printf("%d", bin_seq.bits[i]);
	}*/
	
	//print_sequence(seq);

	int is_ok = test_period(seq, m);
	printf("Тестирование периода: %c\n", is_ok ? '+' : '-');
	is_ok = test_dispersion(seq);
	printf("Тестирование разброса: %c\n", is_ok ? '+' : '-');
	is_ok = test_lsb(seq, 0.05);
	printf("Тестирование младших битов: %c\n", is_ok ? '+' : '-');
	is_ok = test_pearson(seq, 10, 0.01);
	printf("Тестирование случайности последовательности по критерию Пирсона: %c\n", is_ok ? '+' : '-');
	is_ok = test_unlinked_series_method(seq, 0.01);
	printf("Тестирование методом несцепленных серий: %c\n", is_ok ? '+' : '-');

	return 0;
}