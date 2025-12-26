import numpy as np
from typing import List, Tuple
from scipy.stats import chi2


class LFSRGenerator:
    """Класс для генерации псевдослучайных последовательностей с помощью LFSR"""

    def __init__(self, polynomial: List[int], initial_state: List[int]):
        """
        polynomial: коэффициенты от МЛАДШЕЙ степени к СТАРШЕЙ [a₀, a₁, ..., a_n]
        где a₀ соответствует x⁰, a₁ - x¹, ..., a_n - xⁿ

        initial_state: начальное состояние G
        """
        self.polynomial = polynomial
        self.state = initial_state.copy()
        self.degree = len(initial_state)

    def next_bit(self) -> int:
        """Генерация следующего бита"""
        # Вычисляем обратную связь (используем старший коэффициент)
        feedback = self.state[0]

        # Сдвигаем регистр
        for i in range(self.degree - 1):
            self.state[i] = self.state[i + 1]

        # Вычисляем новый бит для последней ячейки
        new_bit = feedback
        for i in range(1, self.degree):
            if self.polynomial[i]:
                new_bit ^= self.state[i]

        self.state[self.degree - 1] = new_bit
        return feedback

    def generate_sequence(self, length: int) -> List[int]:
        """Генерация последовательности заданной длины"""
        sequence = []
        for _ in range(length):
            sequence.append(self.next_bit())
        return sequence


class SGGenerator:
    """SG-генератор с псевдослучайным прореживанием"""

    def __init__(self, poly_a: List[int], state_a: List[int],
                 poly_s: List[int], state_s: List[int]):
        self.generator_a = LFSRGenerator(poly_a, state_a)
        self.generator_s = LFSRGenerator(poly_s, state_s)

    def generate_sequence(self, length: int) -> List[int]:
        """Генерация выходной последовательности"""
        output = []
        bits_generated = 0

        # Генерируем последовательность пока не наберем нужную длину
        while bits_generated < length:
            a_bit = self.generator_a.next_bit()
            s_bit = self.generator_s.next_bit()

            if s_bit == 1:
                output.append(a_bit)
                bits_generated += 1
        return output


class PseudoRandomTester:
    """Класс для тестирования псевдослучайных последовательностей"""

    @staticmethod
    def chi_square_test(sequence: List[int]) -> Tuple[float, float, bool]:
        """
        Критерий согласия χ²-Пирсона для бинарной последовательности
        """
        n = len(sequence)

        # Для бинарной последовательности всегда 2 интервала: 0 и 1
        k = 2

        # Считаем частоты
        count_0 = sequence.count(0)
        count_1 = sequence.count(1)
        observed = np.array([count_0, count_1])

        # Ожидаемые частоты (равномерное распределение)
        expected = np.array([n / 2, n / 2])

        # Вычисляем статистику χ²
        chi_square = np.sum((observed - expected) ** 2 / expected)

        # Критическое значение (α = 0.05, степени свободы = k-1 = 1)
        critical_value = chi2.ppf(0.95, k - 1)

        # Проверяем гипотезу
        test_passed = chi_square <= critical_value

        return chi_square, critical_value, test_passed

    @staticmethod
    def find_period_theory(poly_a: List[int], poly_s: List[int]) -> int:
        """
        Вычисление периода SG-генератора по теоретической формуле
        T = (2^n - 1) * 2^(m-1), где:
        n - степень полинома G₁ (генератор данных)
        m - степень полинома G₂ (селектирующий генератор)
        """
        # Вычисляем степени полиномов
        degree_n = len(poly_a) - 1  # степень полинома G₁
        degree_m = len(poly_s) - 1  # степень полинома G₂

        # Для примитивных полиномов 7-й степени период вычисляется по формуле
        theoretical_period = (2**degree_n - 1) * (2**(degree_m - 1))

        # Возвращаем теоретический период
        return theoretical_period

    @staticmethod
    def find_period_practice(sequence: List[int]) -> int:
        n = len(sequence)

        # Проверяем возможные длины периода от 1 до n//2
        for period_len in range(1, n // 2 + 1):
            is_period = True

            # Проверяем, повторяется ли последовательность с этим периодом
            for i in range(period_len, n):
                # Сравниваем каждый элемент с соответствующим элементом в первом периоде
                if sequence[i] != sequence[i % period_len]:
                    is_period = False
                    break

            # Если нашли период, возвращаем его длину
            if is_period:
                return period_len

        # Если период не найден, возвращаем длину всей последовательности
        return n


    @staticmethod
    def save_test_results(sequence: List[int], filename: str = "test_results.txt"):
        """Сохранение результатов тестирования в файл"""
        chi_square, critical_value, test_passed = PseudoRandomTester.chi_square_test(sequence)
        period = PseudoRandomTester.find_period(sequence)

        with open(filename, "w", encoding="utf-8") as f:
            f.write("РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ПОСЛЕДОВАТЕЛЬНОСТИ\n")
            f.write("=" * 50 + "\n\n")
            f.write(f"Длина последовательности: {len(sequence)}\n")
            f.write(f"Период последовательности: {period}\n")
            f.write(f"χ² статистика: {chi_square:.4f}\n")
            f.write(f"Критическое значение: {critical_value:.4f}\n")
            f.write(f"Тест равномерности: {'ПРОЙДЕН' if test_passed else 'НЕ ПРОЙДЕН'}\n")