import math
import itertools
from typing import Dict, List


def average_code_length(codes, prob_dict):
    """
    Вычисляет среднюю длину кодового слова
    :param codes: массив символов
    :param prob_dict: словарь формата {символ: вероятность}
    """
    return sum(prob_dict[sym] * len(codes[sym]) for sym in codes if sym in prob_dict)


def entropy(prob_dict):
    """
    Вычисляет энтропию источника
    :param prob_dict: словарь формата {символ: вероятность}
    """
    return -sum(prob * math.log2(prob) for prob in prob_dict.values() if prob > 0)


def kraft_inequality(codes):
    """
    Сборка суммы для неравенства Крафта
    :param codes: массив символов
    """
    return sum(2 ** -len(code) for code in codes.values())


def hamming_weight(code_word):
    """
    Вычисляет вес Хэмминга (количество единиц) в кодовом словаре
    :param code_word: кодовое слово в виде строки из 0 и 1
    """
    return sum(1 for bit in code_word if bit == '1')


def hamming_distance(code1, code2):
    """
    Вычисляет расстояние Хэмминга между двумя кодовыми словами
    :param code1: первое кодовое слово
    :param code2: второе кодовое слово
    """
    if len(code1) != len(code2):
        raise ValueError("Кодовые слова должны иметь одинаковую длину")
    return sum(1 for b1, b2 in zip(code1, code2) if b1 != b2)


def min_hamming_distance(code_words):
    """
    Находит минимальное кодовое расстояние для набора кодовых слов
    :param code_words: список кодовых слов
    """
    if len(code_words) < 2:
        return 0

    min_dist = float('inf')
    for i, code1 in enumerate(code_words):
        for j, code2 in enumerate(code_words):
            if i != j:
                dist = hamming_distance(code1, code2)
                if dist < min_dist:
                    min_dist = dist

    return min_dist if min_dist != float('inf') else 0


def calculate_hamming_metrics():
    """
    Вычисляет метрики для кода Хэмминга (7,4)
    """
    # Все возможные кодовые слова Хэмминга (7,4)
    hamming_codewords = [
        '0000000', '1101001', '0101010', '1000011',
        '1001100', '0100101', '1100110', '0001111',
        '1110000', '0011001', '1011010', '0110011',
        '0111100', '1010101', '0010110', '1111111'
    ]

    # Вычисляем минимальное расстояние
    d_min = min_hamming_distance(hamming_codewords)

    # Параметры кода
    n = 7  # длина кодового слова
    k = 4  # число информационных битов
    M = 16  # количество кодовых слов

    # Число исправляемых ошибок
    t = (d_min - 1) // 2

    # Число обнаруживаемых ошибок
    detectable_errors = d_min - 1

    # Проверка совершенности кода
    sum_volume = sum(math.comb(n, i) for i in range(t + 1))
    is_perfect = (M * sum_volume == 2 ** n)

    # Граница Хэмминга (проверка совершенности)
    hamming_bound_check = is_perfect

    # Граница Плоткина для кода (7,4) с d=3
    plotkin_bound = 2

    # Граница Варшамова-Гильберта
    varshamov_gilbert = 16

    # Граница Гилберта
    gilbert_bound = 16

    return {
        'code_distance': d_min,
        'codeword_length': n,
        'info_bits': k,
        'redundancy_bits': n - k,
        'correctable_errors': t,
        'detectable_errors': detectable_errors,
        'hamming_bound': hamming_bound_check,
        'plotkin_bound': plotkin_bound,
        'varshamov_gilbert_bound': varshamov_gilbert,
        'gilbert_bound': gilbert_bound,
        'is_perfect_code': is_perfect,
        'code_rate': round(k / n, 3),
        'all_codewords': hamming_codewords,
        'number_of_codewords': M
    }


def calculate_shannon_parity_metrics(base_codes, codes_with_parity):
    """
    Вычисляет метрики для кода Шеннона с проверкой четности
    :param base_codes: базовые коды Шеннона
    :param codes_with_parity: коды с проверкой четности
    """
    # Получаем все кодовые слова с четностью
    parity_codewords = list(codes_with_parity.values())

    # Вычисляем минимальное расстояние
    d_min = min_hamming_distance(parity_codewords)

    # Анализируем структуру кодов
    code_lengths = [len(code) for code in parity_codewords]
    avg_length = sum(code_lengths) / len(code_lengths) if code_lengths else 0

    # Определяем возможности обнаружения ошибок
    detectable_errors = d_min - 1 if d_min > 0 else 0
    correctable_errors = max(0, (d_min - 1) // 2)

    return {
        'code_distance': d_min,
        'average_length': round(avg_length, 4),
        'min_length': min(code_lengths) if code_lengths else 0,
        'max_length': max(code_lengths) if code_lengths else 0,
        'detectable_errors': detectable_errors,
        'correctable_errors': correctable_errors,
        'is_error_detecting': d_min >= 2,
        'is_error_correcting': d_min >= 3,
        'number_of_codewords': len(parity_codewords)
    }


def calculate_shannon_bounds(shannon_parity_metrics):
    """
    Вычисляет границы для кода Шеннона с проверкой чётности
    """
    n_avg = shannon_parity_metrics['average_length']
    M = shannon_parity_metrics['number_of_codewords']
    d_min = shannon_parity_metrics['code_distance']

    # Граница Хэмминга (используем среднюю длину как приближение)
    t = (d_min - 1) // 2
    if t >= 0:
        # Используем округленную среднюю длину для расчета комбинаций
        n_rounded = max(2, int(round(n_avg)))
        sum_volume = sum(math.comb(n_rounded, i) for i in range(t + 1))
        hamming_bound = M <= (2 ** n_rounded) / sum_volume if sum_volume > 0 else False
    else:
        hamming_bound = False

    # Граница Плоткина (упрощённо)
    plotkin_bound = 2 if d_min > n_avg / 2 else "—"

    # Граница Варшамова-Гильберта (упрощённо)
    varshamov_bound = M

    return {
        'hamming_bound': hamming_bound,
        'plotkin_bound': plotkin_bound,
        'varshamov_bound': varshamov_bound
    }


def calculate_metrics(codes, prob_dict):
    """
    Вычисляет все метрики
    :param codes: массив символов
    :param prob_dict: словарь формата {символ: вероятность}
    """
    avg_len = average_code_length(codes, prob_dict)  # Средняя длина кодового слова
    entropy_val = entropy(prob_dict)  # Энтропия
    redundancy_val = avg_len - entropy_val  # Избыточность
    kraft_sum = kraft_inequality(codes)  # Сумма для неравенства Крафта
    kraft_check = kraft_sum <= 1  # Неравенство Крафта

    return {
        'average_length': round(avg_len, 4),
        'entropy': round(entropy_val, 4),
        'redundancy': round(redundancy_val, 4),
        'kraft_inequality': kraft_check,
        'kraft_sum': round(kraft_sum, 4)
    }


def generate_syndrome_table():
    """
    Генерирует таблицу синдромов для кода Хэмминга (7,4)
    """
    syndrome_table = []

    # Синдромы и соответствующие позиции ошибок
    syndromes = {
        '001': 7, '010': 6, '011': 4, '100': 5,
        '101': 2, '110': 1, '111': 3
    }

    for syndrome, error_pos in syndromes.items():
        syndrome_table.append({
            'Синдром': syndrome,
            'Позиция ошибки': error_pos,
            'Ошибочный бит': f'p{error_pos-4}' if error_pos > 4 else f'd{error_pos}',
            'Тип бита': 'Проверочный' if error_pos > 4 else 'Информационный'
        })

    # Добавляем случай без ошибок
    syndrome_table.insert(0, {
        'Синдром': '000',
        'Позиция ошибки': 0,
        'Ошибочный бит': 'Нет ошибки',
        'Тип бита': 'Без ошибок'
    })

    return syndrome_table