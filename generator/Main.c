#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#include <math.h>


typedef struct {
	int* bits;
	unsigned long long int length;
} binary_t;


typedef struct {
	unsigned long long int number; // число в десятичном виде
	unsigned long long int frequency; // частость числа в последовательности
	float probability; // вероятность числа в последовательности
	binary_t binary_number; // число в бинарном виде
} element_t;


typedef struct {
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


sequence_t generate(unsigned long long int x, unsigned long long int a, unsigned long long int c, unsigned long long int m) {
	sequence_t res = (sequence_t){
		.elements = (element_t*)malloc((m + 1) * sizeof(element_t)),
		.length = 0
	};
	res.elements[res.length] = (element_t){
		.number = x,
		.frequency = 0,
		.probability = 0,
		.binary_number = to_binary(x)
	};
	unsigned long long int max_bin_length = res.elements[res.length].binary_number.length;
	do {
		unsigned long long int number = lcg(res.elements[res.length].number, a, c, m);
		res.elements[++res.length] = (element_t){
			.number = number,
			.frequency = 0,
			.probability = 0,
			.binary_number = to_binary(number)
		};
		if (res.elements[res.length].binary_number.length > max_bin_length) {
			max_bin_length = res.elements[res.length].binary_number.length;
		}
	} while (res.elements[res.length].number != res.elements[0].number);
	for (unsigned long long int i = 0; i < res.length; i++) {
		binary_t binary_number = (binary_t){
			.bits = (int*)malloc((max_bin_length) * sizeof(int)),
			.length = max_bin_length
		};
		for (int j = binary_number.length - 1, k = res.elements[i].binary_number.length - 1; j >= 0; j--, k--) {
			binary_number.bits[j] = k >= 0 ? res.elements[i].binary_number.bits[k] : 0;
		}
		res.elements[i].binary_number = binary_number;
	}
	calc_probabilities(res);
	return res;
}


unsigned long long int period(sequence_t seq) {
	return seq.length;
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
	unsigned long long int k = period(seq);
	return k == m;
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
	float quantiles[31][7] = {
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
		{24.7, 21.9, 19.7, 4.57, 3.82, 3.05}
	};
	int freedom_degrees = 0;
	int k = intervals_number - freedom_degrees - 1;
	float quantile = 0;
	for (int i = 0; i < 7; i++) {
		if (fabs(quantiles[0][i] - significance) < 1E-10) {
			quantile = quantiles[k][i];
		}
	}
	if (fabs(quantile - 0.0) < 1E-10) {
		printf("Заданный уровень значимости не поддерживается.\nВозможные варианты: 0.01, 0.025, 0.05, 0.95, 0.975, 0.99.\n");
		exit(EXIT_FAILURE);
	}
	return stat <= quantile;
}


int test_unlinked_series_method(sequence_t seq) {
	int res = 1;
	int seq_length = seq.length * seq.elements[0].binary_number.length;
	int* bin_seq = (int*)malloc(seq_length * sizeof(int));
	for (int i = 0; i < seq.length; i++) {
		for (int j = 0; j < seq.elements[i].binary_number.length; j++) {
			bin_seq[i * seq.elements[i].binary_number.length + j] = seq.elements[i].binary_number.bits[j];
		}
	}
	for (int i = 1; i < floor(log2(seq_length)); i++) {
		int size = pow(2, i);
		int* n = (int*)malloc(size * sizeof(int));
		for (int j = 0; j < size - 1; j++) {
			n[j] = 0;
		}
		int k = 0;
		int tmp = 0;
		while (k < floor(seq_length * 1.0 / i)) {
			int j = 1;
			int tmp = 1;
			int sum = 0;
			while (j < i) {
				sum += tmp * bin_seq[k * i + j];
				tmp *= 2;
				j++;
			}
			n[sum]++;
			k++;
		}
		tmp = floor(seq_length * 1.0 / i);
		for (int j = 0; j < size - 1; j++) {
			if (n[j] < tmp - 1 || n[j] < tmp + 1) {
				res = 0;
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
	// print_sequence(seq);

	int is_ok = test_period(seq, m);
	printf("Тестирование периода: %c\n", is_ok ? '+' : '-');
	is_ok = test_dispersion(seq);
	printf("Тестирование разброса: %c\n", is_ok ? '+' : '-');
	is_ok = test_lsb(seq, 0.05);
	printf("Тестирование младших битов: %c\n", is_ok ? '+' : '-');
	is_ok = test_pearson(seq, 10, 0.01);
	printf("Тестирование случайности последовательности по критерию Пирсона: %c\n", is_ok ? '+' : '-');
	is_ok = test_unlinked_series_method(seq);
	printf("Тестирование методом несцепленных серий: %c\n", is_ok ? '+' : '-');

	return 0;
}